/*
      3DS2POV.C  Copyright (c) 1993 Steve Anger and Jeff Bowermaster

      Reads a 3D Studio .3DS file and writes a POV-Ray, Vivid, or
      Polyray scene file.

      Version 1.8 Written Oct/93
      
      extended by Bernhard Liebl to support uv mapping and csp output

      Compiled with MSDOS GNU C++ 2.4.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <console.h>
#include "vect.h"
#include "smooth.h"
#include "tc.h"
#include "VertexHash.h"

#define FALSE 0
#define TRUE  1

/* Internal bounding modes */
#define OFF  0
#define ON   1
#define AUTO 2

#define MAX_LIB  10
#define ASPECT   1.333

/* Output formats */
#define POV10    0
#define POV20    1
#define VIVID    2
#define POLYRAY  3
#define RAW      99

#define DEG(x) ((180.0/M_PI)*(x))
#define RAD(x) ((M_PI/180.0)*(x))

#ifndef M_PI
#define M_PI (3.1415926535)
#endif

#ifndef MAXFLOAT
#define MAXFLOAT (1e37)
#endif

/* A generic list type */
#define LIST_INSERT(root, node) list_insert ((List **)&root, (List *)node)
#define LIST_FIND(root, name)   list_find   ((List **)&root, name)
#define LIST_DELETE(root, node) list_delete ((List **)&root, (List *)node)
#define LIST_KILL(root)         list_kill   ((List **)&root)

#define LIST_FIELDS  \
    char name[80];   \
    void *next;


typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef struct {
    LIST_FIELDS
} List;


typedef struct {
    int a, b, c;
} Face;


typedef struct {
    float red, green, blue;
} Colour;

/* Transformation command */
typedef struct {
    LIST_FIELDS

    Matrix matrix;
} Transform;


/* Morph command */
typedef struct {
    LIST_FIELDS

    int    count;          /* Number of objects in morph */
    char   names[4][80];   /* Name of n'th object in average */
    float  weight[4];      /* Weight applied to n'th object */

    Matrix matrix;
} Morph;


/* Omni light command */
typedef struct {
    LIST_FIELDS

    Vector pos;            /* Light position */
    Colour col;            /* Light colour */
} OmniLight;


/* Spotlight command */
typedef struct {
    LIST_FIELDS

    Vector pos;            /* Spotlight position */
    Vector target;         /* Spotlight target location */
    Colour col;            /* Spotlight colour */
    float  hotspot;        /* Hotspot angle (degrees) */
    float  falloff;        /* Falloff angle (degrees) */
    int    shadow_flag;    /* Shadow flag (not used) */
} Spotlight;


/* Camera command */
typedef struct {
    LIST_FIELDS

    Vector pos;            /* Camera location */
    Vector target;         /* Camera target */
    float  bank;           /* Banking angle (degrees) */
    float  lens;           /* Camera lens size (mm) */
} Camera;


/* Material list */
typedef struct {
    LIST_FIELDS

    int  external;         /* Externally defined material? */
} Material;


/* Object summary */
typedef struct {
    LIST_FIELDS

    Vector center;         /* Min value of object extents */
    Vector lengths;        /* Max value of object extents */
} Summary;


/* Material property */
typedef struct {
    LIST_FIELDS

    Colour ambient;
    Colour diffuse;
    Colour specular;
    float  shininess;
    float  transparency;
    float  reflection;
    int    self_illum;
    char   tex_map[40];
    float  tex_strength;
    char   bump_map[40];
    float  bump_strength;
} MatProp;


/* Default material property */
MatProp DefaultMaterial = { "Default", NULL, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0},
			     {1.0, 1.0, 1.0}, 70.0, 0.0, 0.0, FALSE };

/* A mesh object */
typedef struct {
    LIST_FIELDS

    int  vertices;         /* Number of vertices */
    Vector *vertex;        /* List of object vertices */

    int  faces;            /* Number of faces */
    Face *face;            /* List of object faces */
    Material **mtl;        /* Materials for each face */

	int   uvs;			   /* Number of uv pairs */
	Param *uv;             /* List of uv values */

    Matrix matrix;         /* Local mesh matrix */
    Matrix invmatrix;
    Vector center;         /* Center of object */
    Vector lengths;        /* Dimensions of object */

    int hidden;            /* Hidden flag */
    int shadow;            /* Shadow flag */
} Mesh;


typedef struct {
    dword start;
    dword end;
    dword length;
    word  tag;
} Chunk;


typedef struct {
    byte red;
    byte green;
    byte blue;
} Colour_24;


Colour Black = {0.0, 0.0, 0.0};

OmniLight *omni_list  = NULL;
Spotlight *spot_list  = NULL;
Camera    *cam_list   = NULL;
Mesh      *mesh_list  = NULL;
Transform *trans_list = NULL;
Morph     *morph_list = NULL;
Material  *mtl_list   = NULL;
List      *excl_list  = NULL;
List      *box_list   = NULL;
MatProp   *mprop_list = NULL;
Summary   *summary    = NULL;


FILE   *in;
FILE   *out;
char   inname[80];
char   outname[80];
char   vuename[80];
char   obj_name[80] = "";
Colour fog_colour = {0.0, 0.0, 0.0};
Colour col        = {0.0, 0.0, 0.0};
Colour global_amb = {0.1, 0.1, 0.1};
Vector pos        = {0.0, 0.0, 0.0};
Vector target     = {0.0, 0.0, 0.0};
float  fog_distance = 0.0;
float  hotspot = -1;
float  falloff = -1;
Mesh   *mesh = NULL;
int    frame = -1;
char   libname[MAX_LIB][80];
float  smooth = 60.0;
int    bound = 0;
int    verbose = 0;
int    format = POV20;
int    internal_bounding = AUTO;
int    box_all = FALSE;
int    cameras = 0;
int    libs = 0;
float  vue_version = 1.0;
Matrix *ani_matrix = NULL;
long	g_mesh_count = 0;


void convert();
void init_globals();
void process_args (int argc, char *argv[]);
void parse_option (char *option);
void list_insert (List **root, List *new_node);
void *list_find (List **root, char *name);
void list_delete (List **root, List *node);
void list_kill (List **root);
Material *update_materials (char *new_material, int ext);
MatProp *create_mprop (void);
void read_library (char *fname);
void write_intro (FILE *f);
void write_material (FILE *f, char *mat);
void write_mesh (FILE *f, Mesh *mesh);
Transform *parse_transform (char *string);
Morph *parse_morph (char *string);
OmniLight *parse_omnilight (char *string);
Spotlight *parse_spotlight (char *string);
Camera *parse_camera (char *string);
void read_frame (char *filename, int frame_no);
void find_frame (FILE *f, int frame_no);
void save_animation (void);
Mesh *create_mesh (char *name, int vertices, int faces, int uvs);
Mesh *copy_mesh (Mesh *mesh);
void free_mesh_data (Mesh *mesh);
void update_limits (Mesh *mesh);
char *before (char *str, char *target);
char *after (char *str, char *target);
char *between (char *str, char *target1, char *target2);
char *parse_string (char *str);
char upcase (char c);
float colour_intens (Colour *colour);
void parse_file (void);
void parse_3ds (Chunk *mainchunk);
void parse_mdata (Chunk *mainchunk);
void parse_fog (Chunk *mainchunk);
void parse_fog_bgnd (void);
void parse_mat_entry (Chunk *mainchunk);
char *parse_mapname (Chunk *mainchunk);
void parse_named_object (Chunk *mainchunk);
void parse_n_tri_object (Chunk *mainchunk);
void parse_point_array (void);
void parse_face_array (Chunk *mainchunk);
void parse_mapping_array();
void parse_msh_mat_group (void);
void parse_smooth_group (void);
void parse_mesh_matrix (void);
void parse_n_direct_light (Chunk *mainchunk);
void parse_dl_spotlight (void);
void parse_n_camera (void);
void parse_colour (Colour *colour);
void parse_colour_f (Colour *colour);
void parse_colour_24 (Colour_24 *colour);
float parse_percentage (void);
short parse_int_percentage (void);
float parse_float_percentage (void);
void start_chunk (Chunk *chunk);
void end_chunk (Chunk *chunk);
byte read_byte (void);
word read_word (void);
dword read_dword (void);
float read_float (void);
void read_point (Vector v);
char *read_string (void);
float findfov (float lens);
void add_ext (char *fname, char *ext, int force);
void cleanup_name (char *name);
void abortmsg (char *msg, int exit_code);


int main (int argc, char *argv[])
{
	argc = ccommand( &argv );

	init_globals();

	if( argc >= 2 && argv[ 1 ][ 0 ] == '*' )
	{
		int framestart = 1;
		int frames = atoi( &argv[ 1 ][ 1 ] );
		int i;
		
		switch (internal_bounding) {
			case OFF:  bound = 2; break;
			case ON:   bound = 0; break;
			case AUTO: bound = (format == POV10) ? 0 : 2; break;
		}

		if( argc == 3 && argv[ 2 ][ 0 ] == '#' )
			framestart = atoi( &argv[ 2 ][ 1 ] );

		for( i = framestart; i <= frames; i++ )
		{
			sprintf( inname, "frame%02d.3ds", i );
			sprintf( outname, "frame%02d.pov", i );
			printf( "*** Now processing %s ***\n", inname );
			convert();
		}
	}
	else
	{
	    process_args (argc, argv);
		convert();
	}

    return 0;
}

void convert()
{
    Material *m;
    int i;

	long matcnt = 0;
	long oldpos = 0;
	
	long length;
	FILE* tmp;
	
    if ((in = fopen (inname, "rb")) == NULL) {
	printf ("Cannot open input file %s!\n", inname);
	exit (1);
    }

    if ((out = fopen ("temp.dat", "wb")) == NULL) {
	printf ("Cannot open output file %s!\n", outname);
	exit (1);
    }

    /* Load the names of pre-defined materials */
    for (i = 0; i < MAX_LIB; i++) {
	if (strlen(libname[i]) > 0)
	    read_library (libname[i]);
    }

    /* Load the instructions for the current frame */
    if (strlen(vuename) > 0)
	read_frame (vuename, frame);

    printf("Output to: %s\n", outname);

    if (frame >= 0)
	printf ("Generating frame #%d\n", frame);

    printf("\nPlease wait; Processing...\n");

	g_mesh_count = 0;
	fwrite( &g_mesh_count, 1, sizeof( g_mesh_count ), out );

    write_intro(out);
	
    parse_file();

	fseek( out, 0, SEEK_SET );
    fwrite( &g_mesh_count, 1, sizeof( g_mesh_count ), out );

    fclose(in);
	fclose (out);
	
    if ((out = fopen (outname, "wb")) == NULL) {
	printf ("Cannot open output file %s!\n", outname);
	exit (1);
    }
	
    fwrite( &matcnt, 1, sizeof( matcnt ), out );

    for (m = mtl_list; m != NULL; m = m->next) {
	if (!m->external)
	    write_material (out, m->name);
	matcnt++;
    }
    oldpos = ftell( out );

    fseek( out, 0, SEEK_SET );
    fwrite( &matcnt, 1, sizeof( matcnt ), out );
    fseek( out, oldpos, SEEK_SET );

	tmp = fopen( "temp.dat", "rb" );
	fseek( tmp, 0, SEEK_END );
	length = ftell( tmp );
	fseek( tmp, 0, SEEK_SET );
	for( i = 0; i < length; i++ )
	{
		char c = fgetc( tmp );
		fwrite( &c, 1, sizeof( char ), out );
	}
	fclose( tmp );

    if (frame >= 0)
	save_animation();

    LIST_KILL (omni_list);
    LIST_KILL (spot_list);
    LIST_KILL (cam_list);
    LIST_KILL (mesh_list);
    LIST_KILL (trans_list);
    LIST_KILL (morph_list);
    LIST_KILL (mtl_list);
    LIST_KILL (excl_list);
    LIST_KILL (box_list);
    LIST_KILL (mprop_list);
    LIST_KILL (summary);
}

void init_globals()
{
	int i;
    char *env_opt, *option;

 	strcpy (inname, "");
    strcpy (outname, "");
    strcpy (vuename, "");

    for (i = 0; i < MAX_LIB; i++)
	strcpy (libname[i], "");

    frame = -1;
    smooth = 70.0;
    bound = 0;
    verbose = 0;
    format = POV20;
    internal_bounding = AUTO;
    box_all = FALSE;
    libs = 0;

    /* Parse the enviroment string options */
    env_opt = getenv ("3DS2POV");

    if (env_opt != NULL) {
	option = parse_string (env_opt);

	while (strlen(option) > 0) {
	    parse_option (option);
	    option = parse_string (NULL);
	}
    }
}

/* Handle the command line args */
void process_args (int argc, char *argv[])
{
	int i;

    printf("\n\nAutodesk 3D Studio to Raytracer file Translator. Oct/93\n");
    printf("Version 1.8 Copyright (c) 1993 Steve Anger and Jeff Bowermaster\n");
#ifdef __GNUC__
    printf ("32 bit version. DOS extender Copyright (c) 1991 DJ Delorie\n");
#endif
	printf ("Modified for uv mapping and other stuff by Bernhard Liebl\n");
    printf ("\n");

    if (argc < 2) {
	printf ("Usage: 3ds2pov inputfile[.3ds] [outputfile] [options]\n\n");
	printf ("Options: -snnn        - Smooth triangles with angles < nnn\n");
	printf ("         -l<filename> - Specifies 3DS texture library\n");
	printf ("         -a<filename> - Use animation information in specified file\n");
	printf ("         -fnnn        - Generate frame nnn of animation\n");
	printf ("         -x<object>   - Exclude this object from conversion\n");
	printf ("         -b<object>   - Convert this object as a box\n");
	printf ("         +i, -i       - Turn internal bounding on or off\n");
	printf ("         +v, -v       - Turn verbose status messages on or off\n");
	printf ("         -op          - Output to POV-Ray 2.0 format (default)\n");
	printf ("         -op1         - Output to POV-Ray 1.0 format\n");
	printf ("         -ov          - Output to Vivid format\n");
	printf ("         -ol          - Output to poLyray format\n");
	printf ("         -or          - Output to RAW triangle format\n\n");
	printf ("ex. 3ds2pov birdshow +v -l3ds.inc\n\n");
	exit(1);
    }

    /* Parse the command line options */
    for (i = 1; i < argc; i++)
	parse_option (argv[i]);

    if (strlen(inname) == 0)
	abortmsg ("No input file specified", 1);

    if (strlen(outname) == 0) {
	strcpy (outname, inname);
	add_ext (outname, "csp", 1);
    }
    else {
	add_ext (outname, "csp", 0);
    }

    switch (internal_bounding) {
	case OFF:  bound = 2; break;
	case ON:   bound = 0; break;
	case AUTO: bound = (format == POV10) ? 0 : 2; break;
    }

    if ((strlen(vuename) > 0) != (frame >= 0))
	abortmsg ("The -a and -f parameters must be used together", 1);
}


void parse_option (char *option)
{
    List *excl, *box;
    char name[80];

    if (option[0] == '-' || option[0] == '+') {
	switch (upcase(option[1])) {
	    case 'A': strcpy (vuename, &option[2]);
		      break;

	    case 'B': strcpy (name, parse_string (&option[2]));
		      if (strlen(name) == 0)
			  box_all = TRUE;
		      else {
			  cleanup_name (name);

			  box = malloc (sizeof (*box));
			  strcpy (box->name, name);

			  LIST_INSERT (box_list, box);
		      }
		      break;

	    case 'F': if (option[2] != '\0')
			  frame = atoi (&option[2]);
		      break;

	    case 'I': if (option[0] == '-')
			  internal_bounding = OFF;
		      else
			  internal_bounding = ON;
		      break;

	    case 'L': if (libs == MAX_LIB)
			  abortmsg ("Too many libraries specified", 1);

		      strcpy (libname[libs++], &option[2]);
		      break;

	    case 'O': switch (upcase(option[2])) {
			  case 'P': if (option[3] == '1')
					format = POV10;
				    else
					format = POV20;
				    break;

			  case 'V': format = VIVID;
				    break;

			  case 'L': format = POLYRAY;
				    break;

			  case 'R': format = RAW;
				    break;

			  default:  printf ("Invalid output format %s specified\n", option);
				    exit(1);
		      }
		      break;

	    case 'S': if (option[2] != '\0')
			  smooth = atof (&option[2]);
		      break;

	    case 'U': printf ("Warning: -u parameter no long has any effect\n");
		      printf ("         use +i or -i instead.\n");
		      break;

	    case 'V': if (option[0] == '-')
			  verbose = 0;
		      else
			  verbose = 1;
		      break;

	    case 'X': strcpy (name, parse_string (&option[2]));
		      cleanup_name (name);

		      excl = malloc (sizeof (*excl));
		      strcpy (excl->name, name);

		      LIST_INSERT (excl_list, excl);
		      break;

	    default : printf ("\nInvalid option %s specified\n", option);
		      exit (1);
	}
    }
    else if (strlen (inname) == 0) {
	strcpy (inname, option);
	add_ext (inname, "3ds", 0);
    }
    else if (strlen (outname) == 0)
	strcpy (outname, option);
    else
	abortmsg ("Too many file names specified.\n", 1);
}


/* Insert a new node into the list */
void list_insert (List **root, List *new_node)
{
    new_node->next = *root;

    *root = new_node;
}


/* Find the node with the specified name */
void *list_find (List **root, char *name)
{
    List *p;

    for (p = *root; p != NULL; p = p->next) {
	if (strcmp (p->name, name) == 0)
	    break;
    }

    return (void *)p;
}


/* Delete the indicated node from the list */
void list_delete (List **root, List *node)
{
    List *prev;

    prev = *root;
    while (prev != NULL && prev->next != node)
	prev = prev->next;

    if (prev == NULL)
	*root = node->next;
    else
	prev->next = node->next;

    free (node);
}


/* Delete the entire list */
void list_kill (List **root)
{
    List *temp;

    while (*root != NULL) {
	temp = *root;
	*root = (*root)->next;
	free (temp);
    }
}


/* Add a new material to the material list */
Material *update_materials (char *new_material, int ext)
{
    Material *p;

    p = LIST_FIND (mtl_list, new_material);

    if (p == NULL) {
	p = malloc (sizeof (*p));

	if (p == NULL)
	    abortmsg ("Out of memory adding material", 1);

	strcpy (p->name, new_material);
	p->external = ext;

	LIST_INSERT (mtl_list, p);
    }

    return p;
}


MatProp *create_mprop()
{
    MatProp *new_mprop;

    new_mprop = malloc (sizeof(*new_mprop));
    if (new_mprop == NULL)
	abortmsg ("Out of memory adding material", 1);

    strcpy (new_mprop->name, "");
    new_mprop->ambient = Black;
    new_mprop->diffuse = Black;
    new_mprop->specular = Black;
    new_mprop->shininess = 0.0;
    new_mprop->transparency = 0.0;
    new_mprop->reflection = 0.0;
    new_mprop->self_illum = FALSE;

    strcpy (new_mprop->tex_map, "");
    new_mprop->tex_strength = 0.0;

    strcpy (new_mprop->bump_map, "");
    new_mprop->bump_strength = 0.0;

    return new_mprop;
}


/* Load in any predefined materials */
void read_library (char *fname)
{
    FILE *lib;
    char string[256], name[80];

    if ((lib = fopen (fname, "r")) == NULL) {
	printf ("Cannot open texture library file %s!\n", fname);
	exit(1);
    }

    switch (format) {
	case POV10:
	case POV20:
	    while (fgets (string, 256, lib) != NULL) {
		if (strstr (string, "#declare")) {
		    strcpy (name, between (string, "#declare", "="));
		    cleanup_name (name);
		    (void)update_materials (name, TRUE);
		}
	    }
	    break;

	case VIVID:
	    while (fgets (string, 256, lib) != NULL) {
		if (strstr (string, "#define")) {
		    (void)parse_string (string);
		    strcpy (name, parse_string (NULL));
		    cleanup_name (name);
		    (void)update_materials (name, TRUE);
		}
	    }
	    break;

	case POLYRAY:
	    while (fgets (string, 256, lib) != NULL) {
		if (strstr (string, "define")) {
		    (void)parse_string (string);
		    strcpy (name, parse_string (NULL));
		    cleanup_name (name);
		    (void)update_materials (name, TRUE);
		}
	    }
	    break;
    }

    fclose (lib);
}


void write_intro (FILE */*f*/)
{
    /*int i;

    switch (format) {
	case POV10:
	case POV20:
	    fprintf (f, "#include \"colors.inc\"\n");
	    fprintf (f, "#include \"shapes.inc\"\n");
	    fprintf (f, "#include \"textures.inc\"\n");

	    for (i = 0; i < MAX_LIB; i++) {
		if (strlen(libname[i]) > 0)
		    fprintf (f, "#include \"%s\"\n", libname[i]);
	    }

	    fprintf (f, "\n");
	    break;

	case VIVID:
	    fprintf (f, "#include color.vc\n");

	    for (i = 0; i < MAX_LIB; i++) {
		if (strlen(libname[i]) > 0)
		    fprintf (f, "#include %s\n", libname[i]);
	    }

	    fprintf (f, "\n");
	    break;

	case POLYRAY:
	    fprintf (f, "include \"colors.inc\"\n");

	    for (i = 0; i < MAX_LIB; i++) {
		if (strlen(libname[i]) > 0)
		    fprintf (f, "include \"%s\"\n", libname[i]);
	    }

	    fprintf (f, "\n");
	    break;
    }*/
}

void write_material (FILE *f, char *mat)
{
	MatProp *mprop = LIST_FIND (mprop_list, mat);
	long length;

	length = strlen( mprop->name );
	fwrite( &length, sizeof( length ), 1, f );
	fwrite( mprop->name, sizeof( char ), length, f );
	
	fwrite( &mprop->diffuse.red, sizeof( float ), 1, f );
	fwrite( &mprop->diffuse.green, sizeof( float ), 1, f );
	fwrite( &mprop->diffuse.blue, sizeof( float ), 1, f );

    /*MatProp *mprop = LIST_FIND (mprop_list, mat);

    if (mprop == NULL)
       mprop = &DefaultMaterial;

    switch (format) {
	case POV10:
	    write_pov10_material (f, mprop);
	    break;

	case POV20:
	    write_pov20_material (f, mprop);
	    break;

	case VIVID:
	    write_vivid_material (f, mprop);
	    break;

	case POLYRAY:
	    write_polyray_material (f, mprop);
	    break;
    }*/
}

#if 0
void write_pov20_material (FILE *f, MatProp *m)
{
    float amb = 0.1, dif = 0.9, spec = 1.0;
    float dist_white, dist_diff, phong, phong_size;
    float red, green, blue;

    /* amb = get_ambient (m); */

    if (m->self_illum) {
	amb = 0.9;
	dif = 0.1;
    }

    dist_white = fabs(1.0 - m->specular.red) +
		 fabs(1.0 - m->specular.green) +
		 fabs(1.0 - m->specular.blue);

    dist_diff  = fabs(m->diffuse.red   - m->specular.red) +
		 fabs(m->diffuse.green - m->specular.green) +
		 fabs(m->diffuse.blue  - m->specular.blue);

    phong_size = 0.7*m->shininess;
    if (phong_size < 1.0) phong_size = 1.0;

    if (phong_size > 30.0)
	phong = 1.0;
    else
	phong = phong_size/30.0;

    fprintf (f, "#declare %s = texture {\n", m->name);
    fprintf (f, "    finish {\n");
    fprintf (f, "        ambient %.2f\n", amb);
    fprintf (f, "        diffuse %.2f\n", dif);
    fprintf (f, "        phong %.2f\n", phong);
    fprintf (f, "        phong_size %.1f\n", phong_size);

    if (dist_diff < dist_white)
	fprintf (f, "        metallic\n");

    if (m->reflection > 0.0) {
	spec = (m->specular.red + m->specular.green + m->specular.blue)/3.0;
	fprintf (f, "        reflection %.3f\n", spec * m->reflection);
    }

    if (m->transparency > 0.0) {
	fprintf (f, "        ior 1.1\n");
	fprintf (f, "        refraction 1.0\n");
    }

    fprintf (f, "    }\n");

    if (m->transparency > 0.0) {
	red   = m->diffuse.red;
	green = m->diffuse.green;
	blue  = m->diffuse.blue;

	/* Saturate the colour towards white as the transparency increases */
	red   = ((1.0 - m->transparency) * red)   + m->transparency;
	green = ((1.0 - m->transparency) * green) + m->transparency;
	blue  = ((1.0 - m->transparency) * blue)  + m->transparency;

	fprintf (f, "    pigment { rgbf <%.3f, %.3f, %.3f, %.3f> }\n",
		    red, green, blue, m->transparency);
    }
    else
	fprintf (f, "    pigment { rgb <%.3f, %.3f, %.3f> }\n",
		    m->diffuse.red, m->diffuse.green, m->diffuse.blue);

    if (strlen (m->tex_map) > 0) {
	fprintf (f, "    /* Image map: %s, Strength: %.2f */\n",
			 m->tex_map, m->tex_strength);
    }

    if (strlen (m->bump_map) > 0) {
	fprintf (f, "    /* Bump map: %s, Strength: %.2f */\n",
			 m->bump_map, m->bump_strength);
    }

    fprintf (f, "}\n\n");
}
#endif

static void write_component( FILE *fp, Mesh *mesh, Material* mtl )
{
	long i;
	Smooth smooth;
    Point3 verts[ 3 ];
	Polygon poly;
	VertexHashT* vhash;
	ACTCData *tc;

	/* Smooth mesh */
	
	smooth = initAllTables();
	for( i = 0; i < mesh->faces; i++ )
	{
		long ia = mesh->face[ i ].a;
		long ib = mesh->face[ i ].b;
		long ic = mesh->face[ i ].c;
		
		if( mesh->mtl[ i ] != mtl )
			continue;

		verts[ 0 ].x = mesh->vertex[ ia ][ 0 ];
		verts[ 0 ].y = mesh->vertex[ ia ][ 1 ];
		verts[ 0 ].z = mesh->vertex[ ia ][ 2 ];

		verts[ 1 ].x = mesh->vertex[ ic ][ 0 ];
		verts[ 1 ].y = mesh->vertex[ ic ][ 1 ];
		verts[ 1 ].z = mesh->vertex[ ic ][ 2 ];

		verts[ 2 ].x = mesh->vertex[ ib ][ 0 ];
		verts[ 2 ].y = mesh->vertex[ ib ][ 1 ];
		verts[ 2 ].z = mesh->vertex[ ib ][ 2 ];

		if( mesh->uv )
		{
			verts[ 0 ].u = mesh->uv[ ia ].u;
			verts[ 0 ].v = mesh->uv[ ia ].v;

			verts[ 1 ].u = mesh->uv[ ic ].u;
			verts[ 1 ].v = mesh->uv[ ic ].v;

			verts[ 2 ].u = mesh->uv[ ib ].u;
			verts[ 2 ].v = mesh->uv[ ib ].v;
		}
		else
		{
			verts[ 0 ].u = 0;
			verts[ 0 ].v = 0;
			verts[ 1 ].u = 0;
			verts[ 1 ].v = 0;
			verts[ 2 ].u = 0;
			verts[ 2 ].v = 0;
		}
		
		includePolygon( 3, verts, smooth, 0 );
	}
	
	disableEdgePreservation( smooth );
	setFuzzFraction( smooth, 0.0001 );
	makeVertexNormals( smooth );
	
	/* Generate tri strips */
	
	vhash = VertexHash_New();
	tc = actcNew();
	actcBeginInput( tc );
	
	poly = smooth->polygonTable;
    while( poly != NULL )
    {
    	long v[ 3 ];
    
    	for( i = 0; i < 3; i++ )
    	{
    		VertexT vertex;
    		
    		vertex.px = poly->vertices[ i ].x;
    		vertex.py = poly->vertices[ i ].y;
    		vertex.pz = poly->vertices[ i ].z;

    		vertex.nx = poly->normals[ i ].x;
    		vertex.ny = poly->normals[ i ].y;
    		vertex.nz = poly->normals[ i ].z;

    		vertex.u = poly->vertices[ i ].u;
    		vertex.v = poly->vertices[ i ].v;
    	
	    	v[ i ] = VertexHash_Add( vhash, &vertex );
		}
    
    	if( v[ 0 ] == v[ 1 ] || v[ 1 ] == v[ 2 ] || v[ 2 ] == v[ 0 ] )
    		;
    	else if( actcAddTriangle( tc, v[ 0 ], v[ 1 ], v[ 2 ] ) != ACTC_NO_ERROR )
			abortmsg( "error in actc", 0 );
    
		poly = poly->next;
	};
	
	actcEndInput( tc );
	freeSmooth( smooth );
	
	/* Output mesh */
	
	{
		const char* matname = mtl->name;
		long len = strlen( matname );
		fwrite( &len, 1, sizeof( len ), fp );
		fwrite( matname, len, sizeof( char ), fp );
	}
	
	/* Output vertices */

	{
		long vtxcount;
		VertexT* vtx;
		
		vtx = VertexHash_ToArray( vhash, &vtxcount );
		fwrite( &vtxcount, 1, sizeof( long ), fp );
		
		for( i = 0; i < vtxcount; i++ )
		{
			fwrite( &vtx[ i ], 1, sizeof( VertexT ), fp );
		}
		free( vtx );
	}
	
	/* Output faces */
		
	{
		int prim;
		int v1, v2;
		long fpos0, fpos1, fpos2;
		long stripcnt = 0;

		fpos0 = ftell( fp );
		fwrite( &stripcnt, 1, sizeof( stripcnt ), fp );
			
		actcBeginOutput(tc);
		while((prim = actcStartNextPrim(tc, &v1, &v2)) != ACTC_DATABASE_EMPTY) {
			/* start a primitive of type "prim" with v1 and v2 */
			int v3;
			short indexcnt = 2;
			short stmp;
			
			fpos1 = ftell( fp );
			fwrite( &indexcnt, 1, sizeof( indexcnt ), fp );
			
			stmp = v1;
			fwrite( &stmp, 1, sizeof( stmp ), fp );
			stmp = v2;
			fwrite( &stmp, 1, sizeof( stmp ), fp );
			
			while(actcGetNextVert(tc, &v3) != ACTC_PRIM_COMPLETE)
			{
				/* continue primitive using v3 */
				stmp = v3;
				fwrite( &stmp, 1, sizeof( stmp ), fp );
				indexcnt += 1;
			}

			fpos2 = ftell( fp );
			fseek( fp, fpos1, SEEK_SET );
			fwrite( &indexcnt, 1, sizeof( indexcnt ), fp );
			fseek( fp, fpos2, SEEK_SET );
			
			stripcnt += 1;
		}
		
		fpos2 = ftell( fp );
		fseek( fp, fpos0, SEEK_SET );
		fwrite( &stripcnt, 1, sizeof( stripcnt ), fp );
		fseek( fp, fpos2, SEEK_SET );
		
		actcEndOutput(tc);
	}

	VertexHash_Dispose( vhash );
	actcDelete( tc );
	
	g_mesh_count += 1;
}

/* Write a mesh file */
void write_mesh (FILE *fp, Mesh *mesh)
{
    long i;
    Summary *new_summary;
    Matrix obj_matrix;
    Material* mtl;
	
    if (mesh->hidden || LIST_FIND (excl_list, mesh->name))
	return;

    /* Add this object's stats to the summary */
    new_summary = malloc (sizeof(*new_summary));
    if (new_summary == NULL)
	abortmsg ("Out of memory adding summary", 1);

    strcpy (new_summary->name, mesh->name);
    vect_copy (new_summary->center,  mesh->center);
    vect_copy (new_summary->lengths, mesh->lengths);

    LIST_INSERT (summary, new_summary);

    /* Compute the object transformation matrix for animations */
    if (ani_matrix != NULL) {
	mat_copy (obj_matrix, *ani_matrix);
	if (vue_version > 2.0)
	    mat_mult (obj_matrix, mesh->invmatrix, obj_matrix);
    }

	if( mesh->uvs && mesh->uvs != mesh->vertices )
	abortmsg ("Invalid mapping coordinates", 1);
	
	/* Output mesh components */
	if( mesh->faces == 0 )
		return;
		
	mtl = 0;
		
	while( 1 )
	{
		for( i = 0; i < mesh->faces; i++ )
		{
			mtl = mesh->mtl[ i ];
			if( mtl )
				break;
		}

		if( !mtl )
			break;
		
		write_component( fp, mesh, mtl );
		
		for( i = 0; i < mesh->faces; i++ )
			if( mesh->mtl[ i ] == mtl )
				mesh->mtl[ i ] = 0;
	}

    /*switch (format) {
	case POV10:
	case POV20:
	case VIVID:
	case POLYRAY:
	    opt_set_vert (mesh->vertices);

	    for (i = 0; i < mesh->faces; i++) {
	    long ia = mesh->face[i].a;
	    long ib = mesh->face[i].b;
	    long ic = mesh->face[i].c;
	    
		vect_copy (va, mesh->vertex[ia]);
		vect_copy (vb, mesh->vertex[ib]);
		vect_copy (vc, mesh->vertex[ic]);

		opt_set_texture (mesh->mtl[i]->name);
		
		if (mesh->uv) {
			opt_add_tri(
				va[X], va[Y], va[Z], &mesh->uv[ia],
				vc[X], vc[Y], vc[Z], &mesh->uv[ic],
				vb[X], vb[Y], vb[Z], &mesh->uv[ib]);
		} else {
			opt_add_tri(
				va[X], va[Y], va[Z], 0,
				vc[X], vc[Y], vc[Z], 0,
				vb[X], vb[Y], vb[Z], 0);
		}
		
	    }

	    fclose (f);

	    if (ani_matrix != NULL)
		opt_set_transform (obj_matrix);

	    if (box_all || LIST_FIND (box_list, mesh->name))
		opt_write_box (mesh->name);
	    else
		opt_write_file (mesh->name);

	    f = fopen (outname, "a");
	    break;

	case RAW:
	    fprintf (f, "%s\n", mesh->name);

	    for (i = 0; i < mesh->faces; i++) {
		vect_copy (va, mesh->vertex[mesh->face[i].a]);
		vect_copy (vb, mesh->vertex[mesh->face[i].b]);
		vect_copy (vc, mesh->vertex[mesh->face[i].c]);

		if (ani_matrix != NULL) {
		    vect_transform (va, va, obj_matrix);
		    vect_transform (vb, vb, obj_matrix);
		    vect_transform (vc, vc, obj_matrix);
		}

		fprintf (f, "%f %f %f   %f %f %f   %f %f %f\n",
			    va[X], va[Y], va[Z], vb[X], vb[Y], vb[Z],
			    vc[X], vc[Y], vc[Z]);
	    }

	    break;
    }*/
}


/* Parses an object transformation and returns a pointer to the
   newly allocated transformation */
Transform *parse_transform (char *string)
{
    Transform *t;
    char      *token;
    int       token_no;

    t = (Transform *)malloc (sizeof(*t));
    if (t == NULL)
	abortmsg ("Out of memory allocating transform", 1);

    mat_identity (t->matrix);

    token = parse_string (string);
    token_no = 0;

    while (strlen(token) > 0) {
	 switch (token_no) {
	     case  0: break;
	     case  1: strcpy (t->name, token); break;
	     case  2: t->matrix[0][0] = atof(token); break;
	     case  3: t->matrix[0][1] = atof(token); break;
	     case  4: t->matrix[0][2] = atof(token); break;
	     case  5: t->matrix[1][0] = atof(token); break;
	     case  6: t->matrix[1][1] = atof(token); break;
	     case  7: t->matrix[1][2] = atof(token); break;
	     case  8: t->matrix[2][0] = atof(token); break;
	     case  9: t->matrix[2][1] = atof(token); break;
	     case 10: t->matrix[2][2] = atof(token); break;
	     case 11: t->matrix[3][0] = atof(token); break;
	     case 12: t->matrix[3][1] = atof(token); break;
	     case 13: t->matrix[3][2] = atof(token); break;

	     default: abortmsg ("Error parsing transform", 1);
	 }

	 token = parse_string (NULL);
	 token_no++;
    }

    t->matrix[0][3] = 0.0;
    t->matrix[1][3] = 0.0;
    t->matrix[2][3] = 0.0;
    t->matrix[3][3] = 1.0;

    cleanup_name (t->name);

    return t;
}


/* Parses a morph command and returns a pointer to the
   newly allocated morph */
Morph *parse_morph (char *string)
{
    Morph  *m;
    char   *token;
    int    i, token_no;

    m = (Morph *)malloc (sizeof(*m));
    if (m == NULL)
	abortmsg ("Out of memory allocating morph", 1);

    mat_identity (m->matrix);

    token = parse_string (string);

    token = parse_string (NULL);
    strcpy (m->name, token);

    token = parse_string (NULL);
    m->count = atoi (token);

    if (strlen (m->name) == 0 || m->count < 1 || m->count > 4)
	abortmsg ("Error parsing morph command", 1);

    cleanup_name (m->name);

    for (i = 0; i < m->count; i++) {
	token = parse_string (NULL);
	strcpy (m->names[i], token);

	token = parse_string (NULL);
	m->weight[i] = atof (token);

	if (strlen (m->names[i]) == 0)
	    abortmsg ("Error parsing morph command", 1);

	cleanup_name (m->names[i]);
    }

    token = parse_string (NULL);
    token_no = 0;

    while (strlen(token) > 0) {
	 switch (token_no) {
	     case  0: m->matrix[0][0] = atof(token); break;
	     case  1: m->matrix[0][1] = atof(token); break;
	     case  2: m->matrix[0][2] = atof(token); break;
	     case  3: m->matrix[1][0] = atof(token); break;
	     case  4: m->matrix[1][1] = atof(token); break;
	     case  5: m->matrix[1][2] = atof(token); break;
	     case  6: m->matrix[2][0] = atof(token); break;
	     case  7: m->matrix[2][1] = atof(token); break;
	     case  8: m->matrix[2][2] = atof(token); break;
	     case  9: m->matrix[3][0] = atof(token); break;
	     case 10: m->matrix[3][1] = atof(token); break;
	     case 11: m->matrix[3][2] = atof(token); break;

	     default: abortmsg ("Error parsing morph command", 1);
	 }

	 token = parse_string (NULL);
	 token_no++;
    }

    m->matrix[0][3] = 0.0;
    m->matrix[1][3] = 0.0;
    m->matrix[2][3] = 0.0;
    m->matrix[3][3] = 1.0;

    return m;
}


/* Parses an omni light and returns a pointer to the
   newly allocated light */
OmniLight *parse_omnilight (char *string)
{
    OmniLight *o;
    char      *token;
    int       token_no;

    o = (OmniLight *)malloc (sizeof(*o));
    if (o == NULL)
	abortmsg ("Out of memory allocating omnilight", 1);

    token = parse_string (string);
    token_no = 0;

    while (strlen(token) > 0) {
	 switch (token_no) {
	     case 0: break;
	     case 1: strcpy (o->name, token); break;
	     case 2: o->pos[X] = atof (token); break;
	     case 3: o->pos[Y] = atof (token); break;
	     case 4: o->pos[Z] = atof (token); break;
	     case 5: o->col.red   = atof (token); break;
	     case 6: o->col.green = atof (token); break;
	     case 7: o->col.blue  = atof (token); break;

	     default: abortmsg ("Error parsing omnilight", 1);
	 }

	 token = parse_string (NULL);
	 token_no++;
    }

    cleanup_name (o->name);

    return o;
}


/* Parses a spotlight and returns a pointer to the
   newly allocated spotlight */
Spotlight *parse_spotlight (char *string)
{
    Spotlight *s;
    char      *token;
    int       token_no;

    s = (Spotlight *)malloc (sizeof(*s));
    if (s == NULL)
	abortmsg ("Out of memory allocating spotlight", 1);

    token = parse_string (string);
    token_no = 0;

    while (strlen(token) > 0) {
	 switch (token_no) {
	     case  0: break;
	     case  1: strcpy (s->name, token); break;
	     case  2: s->pos[X] = atof (token); break;
	     case  3: s->pos[Y] = atof (token); break;
	     case  4: s->pos[Z] = atof (token); break;
	     case  5: s->target[X] = atof (token); break;
	     case  6: s->target[Y] = atof (token); break;
	     case  7: s->target[Z] = atof (token); break;
	     case  8: s->col.red   = atof (token); break;
	     case  9: s->col.green = atof (token); break;
	     case 10: s->col.blue  = atof (token); break;
	     case 11: s->hotspot   = atof (token); break;
	     case 12: s->falloff   = atof (token); break;
	     case 13: break;

	     default: abortmsg ("Error parsing spotlight", 1);
	 }

	 token = parse_string (NULL);
	 token_no++;
    }

    cleanup_name (s->name);

    return s;
}


/* Parses a camera command and returns a pointer to the
   newly allocated camera */
Camera *parse_camera (char *string)
{
    Camera *c;
    char   *token;
    int    token_no;

    c = (Camera *)malloc (sizeof(*c));
    if (c == NULL)
	abortmsg ("Out of memory allocating camera", 1);

    token = parse_string (string);
    token_no = 0;

    while (strlen(token) > 0) {
	 switch (token_no) {
	     case 0: break;
	     case 1: c->pos[X] = atof (token); break;
	     case 2: c->pos[Y] = atof (token); break;
	     case 3: c->pos[Z] = atof (token); break;
	     case 4: c->target[X] = atof (token); break;
	     case 5: c->target[Y] = atof (token); break;
	     case 6: c->target[Z] = atof (token); break;
	     case 7: c->bank = atof (token); break;
	     case 8: c->lens = atof (token); break;

	     default: abortmsg ("Error parsing camera", 1);
	 }

	 token = parse_string (NULL);
	 token_no++;
    }

    return c;
}


/* Load the transforms, camera movements, etc for the specified frame */
void read_frame (char *filename, int frame_no)
{
    FILE  *f;
    char  fname[80];
    char  string[256];
    char  *token;

    /* Open the .vue file */
    strcpy (fname, filename);   /* Make a copy we can mess with */
    add_ext (fname, "vue", 0);

    f = fopen (fname, "r");
    if (f == NULL) {
	printf ("Error opening file '%s'\n", fname);
	exit(1);
    }

    /* Load the specified frame */
    find_frame (f, frame_no);

    while (fgets (string, 256, f) != NULL) {
	token = parse_string (string);

	if (strcmp (token, "frame") == 0)
	    break;
	else if (strcmp (token, "transform") == 0) {
	    LIST_INSERT (trans_list, parse_transform (string));
	}
	else if (strcmp (token, "morph") == 0) {
	    LIST_INSERT (morph_list, parse_morph (string));
	}
	else if (strcmp (token, "light") == 0) {
	    LIST_INSERT (omni_list, parse_omnilight (string));
	}
	else if (strcmp (token, "spotlight") == 0) {
	    LIST_INSERT (spot_list, parse_spotlight (string));
	}
	else if (strcmp (token, "camera") == 0) {
	    if (cam_list != NULL)
		abortmsg ("ERROR - Multiple cameras in .vue file", 1);

	    LIST_INSERT (cam_list, parse_camera (string));
	}
	else if (strcmp (token, "top") == 0)
	    abortmsg ("ERROR - Orthogonal viewports are not supported", 1);
	else if (strcmp (token, "bottom") == 0)
	    abortmsg ("ERROR - Orthogonal viewports are not supported", 1);
	else if (strcmp (token, "left") == 0)
	    abortmsg ("ERROR - Orthogonal viewports are not supported", 1);
	else if (strcmp (token, "right") == 0)
	    abortmsg ("ERROR - Orthogonal viewports are not supported", 1);
	else if (strcmp (token, "front") == 0)
	    abortmsg ("ERROR - Orthogonal viewports are not supported", 1);
	else if (strcmp (token, "back") == 0)
	    abortmsg ("ERROR - Orthogonal viewports are not supported", 1);
	else if (strcmp (token, "user") == 0)
	    abortmsg ("ERROR - User viewports are not supported", 1);
    }

    fclose(f);
}


void find_frame (FILE *f, int frame_no)
{
    char  string[256];
    char  *token;
    int   frame = 0;

    /* Search the .vue file for the required frame */
    while (1) {
	/* Read the next line in the file */
	if (fgets (string, 256, f) == NULL) {
	    printf ("Unable to locate frame #%d in .vue file\n", frame_no);
	    exit(1);
	}

	token = parse_string (string);

	if (strcmp (token, "frame") == 0) {
	    token = parse_string (NULL);

	    if (strlen(token) == 0) {
		printf ("Unable to locate frame #%d in .vue file\n", frame_no);
		exit(1);
	    }

	    frame = atoi (token);

	    if (frame == frame_no)
		break;
	}
	else if (strcmp (token, "VERSION") == 0) {
	    token = parse_string (NULL);

	    vue_version = atoi(token) / 100.0;
	}
    }
}


void save_animation()
{
    Mesh      *mesh, *master;
    Transform *t;
    Morph     *m;
    Vector    temp;
    int       i, j;

    printf ("\n");

    for (t = trans_list; t != NULL; t = t->next) {
	 printf ("Transforming object: %s\n", t->name);

	 ani_matrix = &(t->matrix);

	 mesh = LIST_FIND (mesh_list, t->name);

	 if (mesh == NULL) {
	     printf ("Unable to locate mesh object %s\n", t->name);
	     exit(1);
	 }

	 write_mesh (out, mesh);
    }

    for (m = morph_list; m != NULL; m = m->next) {
	printf ("Morphing object: %s\n", m->name);

	ani_matrix = &(m->matrix);

	mesh = LIST_FIND (mesh_list, m->name);
	if (mesh == NULL) {
	    printf ("Unable to locate mesh object %s\n", m->name);
	    exit(1);
	}

	/* Make a copy to mess with */
	master = copy_mesh (mesh);
	master->hidden = FALSE;

	strcpy (master->name, m->name);

	for (i = 0; i < master->vertices; i++)
	    vect_init (master->vertex[i], 0.0, 0.0, 0.0);

	for (i = 0; i < m->count; i++) {
	    mesh = LIST_FIND (mesh_list, m->names[i]);
	    if (mesh == NULL) {
		printf ("Unable to locate mesh object %s\n", m->names[0]);
		exit(1);
	    }

	    if (mesh->vertices != master->vertices)
		abortmsg ("Morphed objects do not contain the same number of vertices", 1);

	    if (mesh->faces != master->faces)
		abortmsg ("Morphed objects do not contain the same number of faces", 1);

	    for (j = 0; j < master->vertices; j++) {
		vect_transform (temp, mesh->vertex[j], mesh->invmatrix);
		vect_scale (temp, temp, m->weight[i]);
		vect_add (master->vertex[j], master->vertex[j], temp);
	    }
	}

	for (i = 0; i < master->vertices; i++)
	    vect_transform (master->vertex[i], master->vertex[i], master->matrix);

	write_mesh (out, master);

	free_mesh_data (master);
	free (master);
    }

    for (mesh = mesh_list; mesh != NULL; mesh = mesh->next)
	free_mesh_data (mesh);
}


/* Create a new mesh */
Mesh *create_mesh (char *name, int vertices, int faces, int uvs)
{
    Mesh *new_mesh;

    new_mesh = malloc (sizeof(*new_mesh));
    if (new_mesh == NULL)
	abortmsg ("Out of memory allocating mesh", 1);

    strcpy (new_mesh->name, name);

    new_mesh->vertices = vertices;

    if (vertices <= 0)
	new_mesh->vertex = NULL;
    else {
	new_mesh->vertex = malloc (vertices * sizeof(*new_mesh->vertex));
	if (new_mesh->vertex == NULL)
	    abortmsg ("Out of memory allocating mesh", 1);
    }

    new_mesh->faces = faces;

    if (faces <= 0) {
	new_mesh->face = NULL;
	new_mesh->mtl = NULL;
    }
    else {
	new_mesh->face = malloc (faces * sizeof(*new_mesh->face));
	if (new_mesh->face == NULL)
	    abortmsg ("Out of memory allocating mesh", 1);

	new_mesh->mtl = malloc (faces * sizeof(*new_mesh->mtl));
	if (new_mesh->mtl == NULL)
	    abortmsg ("Out of memory allocating mesh", 1);
    }

	new_mesh->uvs = uvs;

    if (uvs <= 0)
	new_mesh->uv = NULL;
    else {
	new_mesh->uv = malloc (uvs * sizeof(*new_mesh->uv));
	if (new_mesh->uv == NULL)
	    abortmsg ("Out of memory allocating mesh", 1);
    }

    vect_init (new_mesh->center,  0.0, 0.0, 0.0);
    vect_init (new_mesh->lengths, 0.0, 0.0, 0.0);

    mat_identity (new_mesh->matrix);
    mat_identity (new_mesh->invmatrix);

    new_mesh->hidden = FALSE;
    new_mesh->shadow = TRUE;

    return new_mesh;
}


/* Creates a duplicate copy of a mesh */
Mesh *copy_mesh (Mesh *mesh)
{
    Mesh *new_mesh;
    int  i;

    new_mesh = create_mesh (mesh->name, mesh->vertices, mesh->faces, mesh->uvs);

    if (new_mesh == NULL)
	abortmsg ("Out of memory allocating mesh", 1);

    for (i = 0; i < mesh->vertices; i++)
	vect_copy (new_mesh->vertex[i], mesh->vertex[i]);

    for (i = 0; i < mesh->uvs; i++) {
    new_mesh->uv[i].u = mesh->uv[i].u;
    new_mesh->uv[i].v = mesh->uv[i].v;
	}

    for (i = 0; i < mesh->faces; i++) {
	new_mesh->face[i] = mesh->face[i];
	new_mesh->mtl[i]  = mesh->mtl[i];
    }

    mat_copy (new_mesh->matrix, mesh->matrix);
    mat_copy (new_mesh->invmatrix, mesh->invmatrix);

    vect_copy (new_mesh->center,  mesh->center);
    vect_copy (new_mesh->lengths, mesh->lengths);

    new_mesh->hidden = mesh->hidden;
    new_mesh->shadow = mesh->shadow;

    return new_mesh;
}


/* Free all data associated with mesh object */
void free_mesh_data (Mesh *mesh)
{
    if (mesh->vertex != NULL)
	free (mesh->vertex);

    if (mesh->face != NULL)
	free (mesh->face);

    if (mesh->mtl != NULL)
	free (mesh->mtl);
}


/* Updates the center (pivot) point of the mesh */
void update_limits (Mesh *mesh)
{
    Vector vmin = {+MAXFLOAT, +MAXFLOAT, +MAXFLOAT};
    Vector vmax = {-MAXFLOAT, -MAXFLOAT, -MAXFLOAT};
    int    i;

    for (i = 0; i < mesh->vertices; i++) {
	vect_min (vmin, vmin, mesh->vertex[i]);
	vect_max (vmax, vmax, mesh->vertex[i]);
    }

    vect_add  (mesh->center, vmin, vmax);
    vect_scale (mesh->center, mesh->center, 0.5);

    vect_sub (mesh->lengths, vmax, vmin);
}


/* Return the sub-string of 'str' that is before 'target' */
char *before (char *str, char *target)
{
    static char result[256];
    char   *search;

    strncpy (result, str, 256);
    result[255] = '\0';

    search = strstr (result, target);

    if (search != NULL)
	*search = '\0';

    return result;
}


/* Return the sub-string of 'str' that is after 'target' */
char *after (char *str, char *target)
{
    static char result[256];
    char   *search;

    search = strstr (str, target);

    if (search == NULL)
	strncpy (result, "", 256);
    else
	strncpy (result, search + strlen(target), 256);

    result[255] = '\0';

    return result;
}


/* Return the sub-string of 'str' that is between 'target1' and 'target2' */
char *between (char *str, char *target1, char *target2)
{
    static char result[256];

    strcpy (result, after (str, target1));
    strcpy (result, before (result, target2));

    return result;
}


/* Works like the C strtok() function except that it can handle */
/* tokens enclosed in double quotes */
char *parse_string (char *str)
{
    static char result[256];
    static char *p;
    char QUOTE = '\"';
    int  index;

    strcpy (result, "");
    index = 0;

    if (str != NULL)
	p = str;

    /* Find the start of the next token */
    while (isspace (*p))
	p++;

    if (*p == QUOTE) {
	p++;

	while (*p != '\0' && *p != QUOTE)
	    result[index++] = *p++;

	if (*p == QUOTE)
	    p++;
    }
    else {
	while (*p != '\0' && !isspace(*p))
	    result[index++] = *p++;
    }

    result[index] = '\0';

    return result;
}


/* Convert character 'c' to upper case */
char upcase (char c)
{
    if (c >= 'a' && c <= 'z')
	c = c - 'a' + 'A';

    return c;
}


float colour_intens (Colour *colour)
{
    return sqrt (colour->red   * colour->red +
		 colour->green * colour->green +
		 colour->blue  * colour->blue);
}


void parse_file()
{
    Chunk chunk;

    start_chunk(&chunk);

    if (chunk.tag == 0x4D4D)
	parse_3ds (&chunk);
    else
	abortmsg ("Error: Input file is not .3DS format", 1);

    end_chunk (&chunk);
}


void parse_3ds (Chunk *mainchunk)
{
    Chunk chunk;

    do  {
	start_chunk (&chunk);

	if( chunk.end < chunk.start )
		break;

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x3D3D: parse_mdata (&chunk);
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);
}


void parse_mdata (Chunk *mainchunk)
{
    Chunk chunk;
    Colour bgnd_colour;

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x2100: parse_colour (&global_amb);
			     break;
		case 0x1200: parse_colour (&bgnd_colour);
			     break;
		/*case 0x1201: write_bgsolid (out, bgnd_colour);
			     break;*/
		case 0x2200: parse_fog (&chunk);
			     break;
		case 0x2210: parse_fog_bgnd();
			     break;
		/*case 0x2201: write_fog (out, fog_colour, fog_distance);
			     break;*/
		case 0xAFFF: parse_mat_entry (&chunk);
			     break;
		case 0x4000: parse_named_object (&chunk);
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);
}


void parse_fog (Chunk *mainchunk)
{
    Chunk chunk;

    (void)read_float();
    (void)read_float();
    fog_distance = read_float();
    (void)read_float();

    parse_colour (&fog_colour);

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x2210: parse_fog_bgnd();
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);
}


void parse_fog_bgnd()
{

}


void parse_mat_entry (Chunk *mainchunk)
{
    Chunk chunk;
    MatProp *mprop;

    mprop = create_mprop();

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0xA000: strcpy (mprop->name, read_string());
			     cleanup_name (mprop->name);
			     break;

		case 0xA010: parse_colour (&mprop->ambient);
			     break;

		case 0xA020: parse_colour (&mprop->diffuse);
			     break;

		case 0xA030: parse_colour (&mprop->specular);
			     break;

		case 0xA040: mprop->shininess = 100.0*parse_percentage();
			     break;

		case 0xA050: mprop->transparency = parse_percentage();
			     break;

		case 0xA080: mprop->self_illum = TRUE;
			     break;

		case 0xA220: mprop->reflection = parse_percentage();
			     (void)parse_mapname (&chunk);
			     break;

		case 0xA310: if (mprop->reflection == 0.0)
				 mprop->reflection = 1.0;
			     break;

		case 0xA200: mprop->tex_strength = parse_percentage();
			     strcpy (mprop->tex_map, parse_mapname (&chunk));
			     break;

		case 0xA230: mprop->bump_strength = parse_percentage();
			     strcpy (mprop->bump_map, parse_mapname (&chunk));
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);

    LIST_INSERT (mprop_list, mprop);
}


char *parse_mapname (Chunk *mainchunk)
{
    static char name[80] = "";
    Chunk chunk;

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0xA300: strcpy (name, read_string());
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);

    return name;
}


void parse_named_object (Chunk *mainchunk)
{
    Chunk chunk;

    strcpy (obj_name, read_string());
    cleanup_name (obj_name);

    printf ("Working on: %s\n", obj_name);

    mesh = NULL;

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x4100: parse_n_tri_object (&chunk);
			     break;
		case 0x4600: parse_n_direct_light (&chunk);
			     break;
		case 0x4700: parse_n_camera();
			     break;
		case 0x4010: if (mesh != NULL) mesh->hidden = TRUE;
			     break;
		case 0x4012: if (mesh != NULL) mesh->shadow = FALSE;
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);

    if (mesh != NULL) {
	update_limits (mesh);

	if (frame >= 0)
	    LIST_INSERT (mesh_list, mesh);
	else {
	    write_mesh (out, mesh);

	    free_mesh_data (mesh);
	    free (mesh);
	}
    }
}


void parse_n_tri_object (Chunk *mainchunk)
{
    Chunk chunk;

    mesh = create_mesh (obj_name, 0, 0, 0);

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x4110: parse_point_array();
			     break;
		case 0x4120: parse_face_array (&chunk);
			     break;
		case 0x4140: parse_mapping_array();
		         break;
		case 0x4160: parse_mesh_matrix();
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);
}


void parse_point_array()
{
    int i;

    mesh->vertices = read_word();
    mesh->vertex = malloc (mesh->vertices * sizeof(*(mesh->vertex)));
    if (mesh->vertex == NULL && mesh->vertices)
	abortmsg ("Out of memory allocating mesh", 1);

    for (i = 0; i < mesh->vertices; i++)
	read_point (mesh->vertex[i]);
}


void parse_face_array (Chunk *mainchunk)
{
    Chunk chunk;
    int i;

    mesh->faces = read_word();
    mesh->face = malloc (mesh->faces * sizeof(*(mesh->face)));
    if (mesh->face == NULL)
	abortmsg ("Out of memory allocating mesh", 1);

    mesh->mtl = malloc (mesh->faces * sizeof(*(mesh->mtl)));
    if (mesh->mtl == NULL)
	abortmsg ("Out of memory allocating mesh", 1);

    for (i = 0; i < mesh->faces; i++) {
	mesh->face[i].a = read_word();
	mesh->face[i].b = read_word();
	mesh->face[i].c = read_word();
	(void)read_word();

	mesh->mtl[i] = NULL;
    }

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x4130: parse_msh_mat_group();
			     break;
		case 0x4150: parse_smooth_group();
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);

    for (i = 0; i < mesh->faces; i++) {
	if (mesh->mtl[i] == NULL)
	    mesh->mtl[i] = update_materials ("Default", 0);
    }
}


void parse_mapping_array()
{
	int i;

    mesh->uvs = read_word();
    mesh->uv = malloc (mesh->uvs * sizeof(*(mesh->uv)));
    if (mesh->uv == NULL && mesh->uvs)
	abortmsg ("Out of memory allocating mesh", 1);

    for (i = 0; i < mesh->uvs; i++) {
	mesh->uv[i].u = read_float();
	mesh->uv[i].v = read_float();
	}
}


void parse_msh_mat_group()
{
    Material *new_mtl;
    char mtlname[80];
    int  mtlcnt;
    int  i, face;

    strcpy (mtlname, read_string());
    cleanup_name (mtlname);

    new_mtl = update_materials (mtlname, 0);

    mtlcnt = read_word();

    for (i = 0; i < mtlcnt; i++) {
	face = read_word();
	mesh->mtl[face] = new_mtl;
    }
}


void parse_smooth_group()
{

}


void parse_mesh_matrix()
{
    int i, j;

    if (mesh != NULL) {
	for (i = 0; i < 4; i++) {
	    for (j = 0; j < 3; j++)
		mesh->matrix[i][j] = read_float();
	}

	mat_inv (mesh->invmatrix, mesh->matrix);
    }
}


void parse_n_direct_light (Chunk *mainchunk)
{
    Chunk chunk;
    Spotlight *s;
    OmniLight *o;
    int light_off = FALSE;
    int spot_flag = FALSE;

    read_point (pos);
    parse_colour (&col);

    do  {
	start_chunk (&chunk);

	if (chunk.end <= mainchunk->end) {
	    switch (chunk.tag) {
		case 0x4620: light_off = TRUE;
			     break;
		case 0x4610: parse_dl_spotlight();
			     spot_flag = TRUE;
			     break;
	    }
	}

	end_chunk (&chunk);
    } while (chunk.end <= mainchunk->end);

    if (light_off)
	return;

    if (!spot_flag) {
	if (frame >= 0) {
	    o = LIST_FIND (omni_list, obj_name);

	    if (o != NULL) {
		pos[X] = o->pos[X];
		pos[Y] = o->pos[Y];
		pos[Z] = o->pos[Z];
		col    = o->col;
	    }
	}

	/*write_light (out, obj_name, pos, col);*/
    }
    else {
	if (frame >= 0) {
	    s = LIST_FIND (spot_list, obj_name);

	    if (s != NULL) {
		pos[X]    = s->pos[X];
		pos[Y]    = s->pos[Y];
		pos[Z]    = s->pos[Z];
		target[X] = s->target[X];
		target[Y] = s->target[Y];
		target[Z] = s->target[Z];
		col       = s->col;
		hotspot   = s->hotspot;
		falloff   = s->falloff;
	    }
	}

	if (falloff <= 0.0)
	    falloff = 180.0;

	if (hotspot <= 0.0)
	    hotspot = 0.7*falloff;

	/*write_spot (out, obj_name, pos, target, col, hotspot, falloff);*/
    }
}


void parse_dl_spotlight()
{
    read_point (target);

    hotspot = read_float();
    falloff = read_float();
}


void parse_n_camera()
{
    float  bank;
    float  lens;

    read_point (pos);
    read_point (target);
    bank = read_float();
    lens = read_float();

    if (frame >= 0 && cam_list != NULL) {
	pos[X]    = cam_list->pos[X];
	pos[Y]    = cam_list->pos[Y];
	pos[Z]    = cam_list->pos[Z];
	target[X] = cam_list->target[X];
	target[Y] = cam_list->target[Y];
	target[Z] = cam_list->target[Z];
	lens      = cam_list->lens;
	bank      = cam_list->bank;
    }

    /*write_camera (out, obj_name, pos, target, lens, bank);*/
}


void parse_colour (Colour *colour)
{
    Chunk chunk;
    Colour_24 colour_24;

    start_chunk (&chunk);

    switch (chunk.tag) {
	case 0x0010: parse_colour_f (colour);
		     break;

	case 0x0011: parse_colour_24 (&colour_24);
		     colour->red   = colour_24.red/255.0;
		     colour->green = colour_24.green/255.0;
		     colour->blue  = colour_24.blue/255.0;
		     break;

	default:     abortmsg ("Error parsing colour", 1);
    }

    end_chunk (&chunk);
}


void parse_colour_f (Colour *colour)
{
    colour->red   = read_float();
    colour->green = read_float();
    colour->blue  = read_float();
}


void parse_colour_24 (Colour_24 *colour)
{
    colour->red   = read_byte();
    colour->green = read_byte();
    colour->blue  = read_byte();
}


float parse_percentage()
{
    Chunk chunk;
    float percent = 0.0;

    start_chunk (&chunk);

    switch (chunk.tag) {
	case 0x0030: percent = parse_int_percentage()/100.0;
		     break;

	case 0x0031: percent = parse_float_percentage();
		     break;

	default:     printf ("WARNING: Error parsing percentage");
    }

    end_chunk (&chunk);

    return percent;
}


short parse_int_percentage()
{
    word percent = read_word();

    return percent;
}


float parse_float_percentage()
{
    float percent = read_float();

    return percent;
}


void start_chunk (Chunk *chunk)
{
    chunk->start  = ftell(in);
    chunk->tag    = read_word();
    chunk->length = read_dword();
    chunk->end    = chunk->start + chunk->length;
}


void end_chunk (Chunk *chunk)
{
    fseek (in, chunk->end, 0);
}


byte read_byte()
{
    byte data;

    data = fgetc (in);

    return data;
}


word read_word()
{
    word data;

    //fread (&data, 2, 1, in);
    data = read_byte();
    data |= ( (word)read_byte() ) << 8;
    
    return data;
}


dword read_dword()
{
    dword data;

    //fread (&data, 4, 1, in);
    data = read_byte();
    data |= ( (dword)read_byte() ) << 8;
    data |= ( (dword)read_byte() ) << 16;
    data |= ( (dword)read_byte() ) << 24;

    return data;
}


float read_float()
{
    float data;

    //fread (&data, 4, 1, in);
	*(dword*)&data = read_dword();

    return data;
}


void read_point (Vector v)
{
    v[X] = read_float();
    v[Y] = read_float();
    v[Z] = read_float();
}


char *read_string()
{
    static char string[80];
    int i;

    for (i = 0; i < 80; i++) {
	string[i] = read_byte();

	if (string[i] == '\0')
	    break;
    }

    return string;
}


float findfov (float lens)
{
    static float lens_table[13] =
		 { 15.0, 17.0, 24.0, 35.0, 50.0, 85.0, 100.0, 135.0, 200.0,
		   500.0, 625.0, 800.0, 1000.0 };
    static float fov_table[13] =
		 { 115.0, 102.0, 84.0, 63.0, 46.0, 28.0, 24.0, 18.0,
		   12.0, 5.0, 4.0, 3.125, 2.5 };

    float fov, f1, f2, l1, l2;
    int   i;

    if (lens < 15.0)
	lens = 15.0;
    else if (lens > 1000.0)
	lens = 1000.0;

    for (i = 0; i < 13; i++)
	if (lens < lens_table[i])
	    break;

    if (i == 13)
	i = 12;
    else if (i == 0)
	i = 1;

    f1 = fov_table[i-1];
    f2 = fov_table[i];
    l1 = lens_table[i-1];
    l2 = lens_table[i];

    fov = f1 + (lens - l1) * (f2 - f1) / (l2 - l1);

    return fov;
}

void add_ext (char *fname, char *ext, int force)
{
    int i;

    for (i = 0; i < strlen(fname); i++)
	if (fname[i] == '.') break;

    if (fname[i] == '\0' || force) {
	if (strlen(ext) > 0)
	    fname[i++] = '.';

	strcpy (&fname[i], ext);
    }
}


void cleanup_name (char *name)
{
    char *tmp = malloc (strlen(name)+1);
    int  i;

    /* Remove any leading blanks or quotes */
    i = 0;
    while ((name[i] == ' ' || name[i] == '"') && name[i] != '\0')
	i++;

    strcpy (tmp, &name[i]);

    /* Remove any trailing blanks or quotes */
    for (i = strlen(tmp)-1; i >= 0; i--) {
	if (isprint(tmp[i]) && !isspace(tmp[i]) && tmp[i] != '"')
	    break;
	else
	    tmp[i] = '\0';
    }

    strcpy (name, tmp);

    /* Prefix the letter 'N' to materials that begin with a digit */
    if (!isdigit (name[0]))
       strcpy (tmp, name);
    else {
       tmp[0] = 'N';
       strcpy (&tmp[1], name);
    }

    /* Replace all illegal charaters in name with underscores */
    for (i = 0; tmp[i] != '\0'; i++) {
       if (!isalnum(tmp[i]))
	   tmp[i] = '_';
    }

    strcpy (name, tmp);

    free (tmp);
}

void abortmsg (char *msg, int exit_code)
{
    printf ("\n%s\n", msg);
    exit (exit_code);
}

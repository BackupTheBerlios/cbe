// ===========================================================================

//	texture_material_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#include "texture_material_t.h"

#include "Bitmap.h"
#include <GL/glu.h>



BEGIN_LINOTTE_NAMESPACE

#define	checkImageWidth 128
#define	checkImageHeight 128
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
static GLubyte otherImage[checkImageHeight][checkImageWidth][4];

static GLuint texName[2];

void makeCheckImages(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
         c = ((((i&0x10)==0)^((j&0x10))==0))*255;
         otherImage[i][j][0] = (GLubyte) c;
         otherImage[i][j][1] = (GLubyte) 0;
         otherImage[i][j][2] = (GLubyte) 0;
         otherImage[i][j][3] = (GLubyte) 255;
      }
   }
}



texture_material_t::texture_material_t(

	const char*				name,

	const char*				path ) :

	

	material_t( name ),

	m_texture( 0 )

{
  /*makeCheckImages();
  glGenTextures( 1, &m_texture );
  
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
	       checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
	       otherImage);
  glEnable(GL_TEXTURE_2D);
  return;*/


	Bitmap					bmp( path );



	if( !bmp.load() )

	  throw runtime_error( "bitmap load failed" );



	glGenTextures( 1, &m_texture );

	

	try {

		glBindTexture( GL_TEXTURE_2D, m_texture );

		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


		/*glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp.getWidth(),

			bmp.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,

			bmp.getData() );*/
			
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA,
			bmp.getWidth(), bmp.getHeight(), GL_RGBA,
			GL_UNSIGNED_BYTE, bmp.getData() );


	}

	catch(...)

	{

		glDeleteTextures( 1, &m_texture );

		throw;

	}

}



texture_material_t::~texture_material_t()

{

	glDeleteTextures( 1, &m_texture );

}



void

texture_material_t::submit()

{

	// FIXME check if material is already set

	// if( not material_manager_t::switch_to_material( this ) )

	//	return;



	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, m_texture );

}



END_LINOTTE_NAMESPACE


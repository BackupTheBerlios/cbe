// ===========================================================================

//	model_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#include "model_t.h"

#include "mesh_t.h"



#include "simple_material_t.h"

#include "material_manager_t.h"



BEGIN_LINOTTE_NAMESPACE



static int

compare_mesh_materials(

	const void*				pa,

	const void*				pb )

{

	mesh_t*					ma = *(mesh_t**)pa;

	mesh_t*					mb = *(mesh_t**)pb;



	long					addra = (long)ma->material();

	long					addrb = (long)mb->material();



	if( addra < addrb )

		return -1;

	else if( addra == addrb )

		return 0;



	return 1;

}



model_t::model_t(

	input_stream_t&			stream,

	material_manager_t*		matmgr )

{

	load_materials( stream, matmgr );



	u32						mesh_count;

	mesh_t**				meshes;


	mesh_count = stream.read_u32();

	m_mesh_count = mesh_count;
	

	meshes = (mesh_t**)( std::calloc(

		mesh_count, sizeof( mesh_t* ) ) );

	if( !meshes )

		throw std::bad_alloc();

	m_meshes = meshes;

			

	try {

		// load each mesh from stream

		for( u32 i = 0; i < mesh_count; i++ )

			meshes[ i ] = new mesh_t( stream, matmgr );

			

		// sort meshes by material to speed up OpenGL pipeline

		std::qsort( meshes, mesh_count,

			sizeof( mesh_t** ),

			compare_mesh_materials );

	}

	catch(...)

	{

		clear();

		throw;

	}

}



model_t::~model_t()

{

	clear();

}



void

model_t::draw()

{

	long					count = m_mesh_count;

	

	if( !count )

		return;



	mesh_t**				mesh = m_meshes;



	do {

		( *mesh )->draw();

		mesh++;

	} while( --count );

}



void

model_t::load_materials(

	input_stream_t&			stream,

	material_manager_t*		matmgr )

{

	u32						material_count = stream.read_u32();


	for( u32 i = 0; i < material_count; i++ )

	{

		u32					length;

		char				name[ 80 ];

		GLfloat				color[ 4 ];

	
		length = stream.read_u32();

		if( length >= 80 )

			throw_linotte_error( "invalid material name" );

		

		stream.read_bytes( name, length * sizeof( char ) );

		name[ length ] = '\0';

		
		color[ 0 ] = stream.read_f32();

		color[ 1 ] = stream.read_f32();

		color[ 2 ] = stream.read_f32();
		
		color[ 3 ] = 1;

		

		material_t*			material;

		
		material = new simple_material_t( name, color );

		
		matmgr->add_material( material );

	}

}



void

model_t::clear()

{

	for( u32 i = 0; i < m_mesh_count; i++ )

		if( m_meshes[ i ] )

			delete m_meshes[ i ];



	std::free( m_meshes );

	m_meshes = 0;

}



END_LINOTTE_NAMESPACE


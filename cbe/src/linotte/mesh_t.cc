// ===========================================================================
//	mesh_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "mesh_t.h"
#include "material_t.h"
#include "material_manager_t.h"

BEGIN_LINOTTE_NAMESPACE

static void
read_vertices(
	input_stream_t&			stream,
	GLfloat*				vertices,
	long					count )
{
	if( LINOTTE_IS_BIG_ENDIAN &&
		sizeof( GLfloat ) == sizeof( float ) )
	{
		stream.read_bytes( vertices,
			count * sizeof( float ) );
	}
	else
	{
		for( long i = 0; i < count; i++ )
			vertices[ i ] = stream.read_f32();
	}
}

static void
read_tri_strip(
	input_stream_t&			stream,
	tristrip_t*				strip )
{
	long					count;

	count = stream.read_u16();
	strip->count = count;

	GLushort*				indices;

	indices = new GLushort[ count ];
	strip->indices = indices;
	
	if( LINOTTE_IS_BIG_ENDIAN &&
		sizeof( GLuint ) == sizeof( u16 ) )
	{
		stream.read_bytes( indices,
			count * sizeof( u16 ) );
	}
	else
	{
		for( long i = 0; i < count; i++ )
			indices[ i ] = stream.read_u16();
	}
}

mesh_t::mesh_t(
	input_stream_t&			stream,
	material_manager_t*		matmgr ) :
	
	m_vertices( 0 ),
	m_strips( 0 ),
	m_strip_count( 0 )
{
	std::string				material_name;

	stream.read_string( material_name );

	m_material = matmgr->material_by_name(
		material_name.c_str() );

	try {
		u32					vertex_count;
		u32					elem_count;

		vertex_count = stream.read_u32();
		elem_count = vertex_count * ( 2 + 3 + 3 );
		m_vertices = new GLfloat[ elem_count ];
		read_vertices( stream, m_vertices, elem_count );
		
		u32					strip_count;
		
		strip_count = stream.read_u32();
		m_strip_count = strip_count;
		m_strips = (tristrip_t*)std::calloc(
			sizeof( tristrip_t ), strip_count );
		if( !m_strips )
			throw std::bad_alloc();
		
		for( long i = 0; i < (long)strip_count; i++ )
			read_tri_strip( stream, &m_strips[ i ] );
	}
	catch(...)
	{
		clear();
		throw;
	}
}

mesh_t::~mesh_t()
{
	clear();
}

void
mesh_t::draw()
{
	long					count = m_strip_count;
	
	if( !count )
		return;

	if( m_material )
		m_material->submit();

	const tristrip_t*		strip = m_strips;
	
	glInterleavedArrays( GL_T2F_N3F_V3F, 0, m_vertices );
	
	do {
		glDrawElements( GL_TRIANGLE_STRIP, strip->count,
			GL_UNSIGNED_SHORT, strip->indices );
		strip++;
	} while( --count );
}

void
mesh_t::clear()
{
	delete[] m_vertices;
	m_vertices = 0;
	
	for( long i = 0; i < (long)m_strip_count; i++ )
	{
		tristrip_t*			strip = &m_strips[ i ];
	
		if( strip->indices )
			delete[] strip->indices;
	}
	
	std::free( m_strips );
	m_strips = 0;
}

END_LINOTTE_NAMESPACE

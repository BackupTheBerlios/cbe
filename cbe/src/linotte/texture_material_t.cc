// ===========================================================================

//	texture_material_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#include "texture_material_t.h"

#include "Bitmap.h"



BEGIN_LINOTTE_NAMESPACE



texture_material_t::texture_material_t(

	const char*				name,

	const char*				path ) :

	

	material_t( name ),

	m_texture( 0 )

{

	Bitmap					bmp( path );



	if( !bmp.load() )

	  throw runtime_error( "bitmap load failed" );



	glGenTextures( 1, &m_texture );

	

	try {

		glBindTexture( GL_TEXTURE_2D, m_texture );

		

		glTexImage2D( GL_TEXTURE_2D, 0, 4, bmp.getWidth(),

			bmp.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,

			bmp.getData() );

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


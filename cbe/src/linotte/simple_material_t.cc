// ===========================================================================

//	simple_material_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#include "simple_material_t.h"



BEGIN_LINOTTE_NAMESPACE



simple_material_t::simple_material_t(

	const char*				name,

	const GLfloat*			color ) :

	

	material_t( name )

{

	std::memcpy( m_color, color, 4 * sizeof( GLfloat ) );

}



void

simple_material_t::submit()

{

	// FIXME

	/*glDisable( GL_TEXTURE_2D );
	glEnable(GL_COLOR_MATERIAL);


	glColor3fv( m_color );

	glEnable( GL_LIGHTING );
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_color);
    glMaterialfv(GL_BACK, GL_DIFFUSE, m_color);*/
    
   glDisable( GL_TEXTURE_2D );
   glColor3fv( m_color );
    
   /*glMaterialfv(GL_FRONT, GL_DIFFUSE, m_color);

   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);*/
 }



END_LINOTTE_NAMESPACE


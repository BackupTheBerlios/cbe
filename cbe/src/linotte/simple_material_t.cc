// ===========================================================================
//	simple_material_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

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
	glDisable( GL_TEXTURE_2D );

	glColor3fv( m_color );

	
	/*GLfloat mat_shininess[] = { 0.0 };
	
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m_color);*/
 }

END_LINOTTE_NAMESPACE

// ===========================================================================
//	material_manager_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "material_manager_t.h"
#include "material_t.h"

BEGIN_LINOTTE_NAMESPACE

material_manager_t::material_manager_t() :
	
	m_materials( sizeof( material_t* ) )
{
}

material_manager_t::~material_manager_t()
{
	long					count = m_materials.count();

	for( long i = 0; i < count; i++ )
	{
		material_t*			material;
		
		material = *(material_t**)m_materials[ i ];
		
		delete material;
	}
}

material_t*
material_manager_t::material_by_name(
	const char*				name ) const
{
	long					count = m_materials.count();

	for( long i = 0; i < count; i++ )
	{
		material_t*			material;
		
		material = *(material_t**)m_materials[ i ];
		
		if( strcmp( material->name(), name ) == 0 )
			return material;
	}
	
	return 0;
}

void
material_manager_t::add_material(
	material_t*				material )
{
	m_materials.append( &material );
}

END_LINOTTE_NAMESPACE

// ===========================================================================

//	material_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#include "material_t.h"

#include <iostream>



BEGIN_LINOTTE_NAMESPACE



material_t::material_t(

	const char*				name ) :



	m_name( name )

{

	std::cout << "created material \"" << name << "\"" << endl;

}



material_t::~material_t()

{

}



END_LINOTTE_NAMESPACE


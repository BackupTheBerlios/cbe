// ===========================================================================

//	simple_material_t.h		   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_SIMPLE_MATERIAL_HH

#define LINOTTE_SIMPLE_MATERIAL_HH



#include "material_t.h"



BEGIN_LINOTTE_NAMESPACE



class simple_material_t : public material_t {

public:

						simple_material_t(

							const char*				name,

							const GLfloat*			color );



	virtual void		submit();

	

protected:

	GLfloat				m_color[ 4 ];

};



END_LINOTTE_NAMESPACE



#endif // LINOTTE_SIMPLE_MATERIAL_HH


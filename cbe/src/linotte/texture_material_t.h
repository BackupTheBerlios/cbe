// ===========================================================================

//	texture_material_t.h	   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_TEXTURE_MATERIAL_HH

#define LINOTTE_TEXTURE_MATERIAL_HH



#include "material_t.h"



BEGIN_LINOTTE_NAMESPACE



class texture_material_t : public material_t {

public:

						texture_material_t(

							const char*				name,

							const char*				path );



	virtual				~texture_material_t();

	

	virtual void		submit();

	

protected:

	GLuint				m_texture;

};



END_LINOTTE_NAMESPACE



#endif // LINOTTE_TEXTURE_MATERIAL_HH


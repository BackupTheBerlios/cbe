// ===========================================================================

//	material_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_MATERIAL_HH

#define LINOTTE_MATERIAL_HH



#include "linotte.h"

#include <string>



BEGIN_LINOTTE_NAMESPACE



class material_t {

public:

						material_t(

							const char*				name );



	virtual				~material_t();

	

	virtual void		submit() = 0;

	

	const char*			name() const;

	

protected:

	std::string			m_name;

};



inline const char*

material_t::name() const

{

	return m_name.c_str();

}



END_LINOTTE_NAMESPACE



#endif // LINOTTE_MATERIAL_HH


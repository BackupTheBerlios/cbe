// ===========================================================================

//	material_manager_t.h	   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_MATERIAL_MANAGER_HH

#define LINOTTE_MATERIAL_MANAGER_HH



#include "linotte.h"

#include "array_t.h"



BEGIN_LINOTTE_NAMESPACE



class material_t;



class material_manager_t {

public:

						material_manager_t();



	virtual				~material_manager_t();

	

	material_t*			material_by_name(

							const char*				name ) const;



	void				add_material(

							material_t*				material );



protected:

	array_t				m_materials;

};



END_LINOTTE_NAMESPACE



#endif // LINOTTE_MATERIAL_MANAGER_HH


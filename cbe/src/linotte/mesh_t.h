// ===========================================================================

//	mesh_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_MESH_HH

#define LINOTTE_MESH_HH



#include "linotte.h"

#include "input_stream_t.h"



BEGIN_LINOTTE_NAMESPACE



struct tristrip_t {

	GLushort*			indices;

	u32					count;

};



class material_t;

class material_manager_t;



class mesh_t {

public:

						mesh_t(

							input_stream_t&			stream,

							material_manager_t*		matmgr );



	virtual				~mesh_t();



	material_t*			material() const;



	void				draw();



protected:

	void				clear();



	material_t*			m_material;

	GLfloat*			m_vertices;

	tristrip_t*			m_strips;

	u32					m_strip_count;

};



inline material_t*

mesh_t::material() const

{

	return m_material;

}



END_LINOTTE_NAMESPACE



#endif // LINOTTE_MESH_HH




// ===========================================================================

//	model_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_MODEL_HH

#define LINOTTE_MODEL_HH



#include "linotte.h"

#include "input_stream_t.h"



BEGIN_LINOTTE_NAMESPACE



class material_manager_t;

class mesh_t;



class model_t {

public:

						model_t(

							input_stream_t&			stream,

							material_manager_t*		matmgr );



	virtual				~model_t();



	void				draw();



protected:

	void				load_materials(
							input_stream_t&			stream,
							material_manager_t*		matmgr );

	void				clear();

	

	u32					m_mesh_count;

	mesh_t**			m_meshes;

};



END_LINOTTE_NAMESPACE



#endif // LINOTTE_MODEL_HH


// ===========================================================================

//	array_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_ARRAY_HH

#define LINOTTE_ARRAY_HH



#include "linotte.h"



BEGIN_LINOTTE_NAMESPACE



class array_t {

public:

							array_t(

								long				size,

								long				alloc = 0 );



	virtual					~array_t();

	

	long					count() const;

						

	const void*				operator[](

								long				index ) const;



	void					insert_at(

								long				index,

								const void*			item );

								

	void					append(

								const void*			item );

								

	void					remove(

								long				index );



	void					clear();



protected:

	void					grow();

	

	u32						m_elem_size;

	u08*					m_elems;

	u32						m_count;

	u32						m_alloc;

};



// ---------------------------------------------------------------------------



inline const void*

array_t::operator[](

	long				index ) const

{

	return &m_elems[ index * m_elem_size ];

}



inline long

array_t::count() const

{

	return m_count;

}



END_LINOTTE_NAMESPACE



#endif // LINOTTE_ARRAY_HH


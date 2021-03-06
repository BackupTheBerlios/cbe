// ===========================================================================

//	file_input_stream_t.h	   �2000-2000 Bernhard Liebl. All rights reserved.

// ===========================================================================



#ifndef LINOTTE_FILE_INPUT_STREAM_HH

#define LINOTTE_FILE_INPUT_STREAM_HH



#include "input_stream_t.h"
#include <stdio.h>



BEGIN_LINOTTE_NAMESPACE



class file_input_stream_t : public input_stream_t {

public:

						file_input_stream_t(

							const char*			filename );

							

	virtual				~file_input_stream_t();



	virtual pfpos_t		tell();

	

	virtual pfpos_t		read_some_bytes(

							void*				buffer,

							pfpos_t				count );

							

private:

	FILE*				m_fp;

};



END_LINOTTE_NAMESPACE



#endif // LINOTTE_FILE_INPUT_STREAM_HH


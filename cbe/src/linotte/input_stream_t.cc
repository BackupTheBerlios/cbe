// ===========================================================================

//	input_stream_t.cp			   ©2000-2000 Bernhard Liebl. All rights reserved.

// ===========================================================================



#include "input_stream_t.h"



BEGIN_LINOTTE_NAMESPACE



const int				TEMP_BUFFER_SIZE = 1024;



static u08*				s_temp_buffer = 0;



input_stream_t::~input_stream_t()

{

}



void

input_stream_t::read_string(

	std::string&		string )

{

	u32					length = read_u32();



	string = "";

	string.reserve( length );

		

	u08*				buffer = s_temp_buffer;

	const int			buffer_size = TEMP_BUFFER_SIZE / sizeof( char );

	

	if( !buffer )

	  {

	    s_temp_buffer = new u08[ TEMP_BUFFER_SIZE ];

	    buffer = s_temp_buffer;

	  }

	

	while( (long)length >= buffer_size )

	  {

		read_bytes( buffer, buffer_size * sizeof( char ) );

		length -= buffer_size;

		string.append( (char*)buffer, buffer_size );

	}

	

	if( length > 0 )

	{

		read_bytes( buffer, length * sizeof( char ) );

		string.append( (char*)buffer, length );

	}

}



void

input_stream_t::read_bytes(

	void*				buffer,

	pfpos_t				count )

{

	if( read_some_bytes( buffer, count ) != count )

		throw_linotte_error( "io read error" );

}



void

input_stream_t::skip(

	pfpos_t				count )

{

	if( !count )

		return;



	if( !s_temp_buffer )

		s_temp_buffer = new u08[ TEMP_BUFFER_SIZE ];



	while( count >= TEMP_BUFFER_SIZE )

	{

		read_bytes( s_temp_buffer, count );

		count -= TEMP_BUFFER_SIZE;

	}

	

	if( count > 0 )

		read_bytes( s_temp_buffer, count );

}



END_LINOTTE_NAMESPACE


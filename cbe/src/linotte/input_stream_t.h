// ===========================================================================
//	input_stream_t.h       	    ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef LINOTTE_INPUT_STREAM_HH
#define LINOTTE_INPUT_STREAM_HH

#include "linotte.h"
#include <string>

BEGIN_LINOTTE_NAMESPACE

class input_stream_t {
public:

	virtual				~input_stream_t();



	virtual pfpos_t		read_some_bytes(
							void*				buffer,
							pfpos_t				count ) = 0;



	void				read_bytes(
							void*				buffer,
							pfpos_t				count );



	virtual void		skip(

							pfpos_t				count );



	char				read_char();



	u08					read_u08();



	u16					read_u16();



	u32					read_u32();



	u64					read_u64();



	f32					read_f32();



	f64					read_f64();

	

	void				read_string(

							std::string&		string );

};



inline char

input_stream_t::read_char()

{

	return read_u08();

}



inline u08

input_stream_t::read_u08()

{

	u08 x;

	read_bytes( &x, sizeof( u08 ) );

	return x;

}



inline u16

input_stream_t::read_u16()

{

	u16 x;

	read_bytes( &x, sizeof( u16 ) );

	return cond_swap_endian16( x );

}



inline u32

input_stream_t::read_u32()

{

	u32 x;

	read_bytes( &x, sizeof( u32 ) );

	return cond_swap_endian32( x );

}



inline u64

input_stream_t::read_u64()

{

	u32 hi, lo;

	hi = read_u32();

	lo = read_u32();

	return ( ( (u64)hi ) << 32 ) | lo;

}



inline f32

input_stream_t::read_f32()

{

	u32 x = read_u32();

	return *( (float*)&x );

}



inline f64

input_stream_t::read_f64()

{

	u32 x[ 2 ];

	x[ 0 ] = read_u32();

	x[ 1 ] = read_u32();

	return *( (double*)x );

}



END_LINOTTE_NAMESPACE



#endif // LINOTTE_INPUT_STREAM_HH


// ===========================================================================
//	file_input_stream_t.cp	   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "file_input_stream_t.h"

BEGIN_LINOTTE_NAMESPACE

using namespace std;

file_input_stream_t::file_input_stream_t(
	const char*			filename )
{
	m_fp = fopen( filename, "rb" );
	if( !m_fp )
		throw_linotte_error( "file could not be opened" );
}

file_input_stream_t::~file_input_stream_t()
{
	fclose( m_fp );
}

pfpos_t
file_input_stream_t::tell()
{
	return ftell( m_fp );
}

pfpos_t
file_input_stream_t::read_some_bytes(
	void*				buffer,
	pfpos_t				count )
{
	return fread( buffer, sizeof( u08 ), count, m_fp );
}


END_LINOTTE_NAMESPACE

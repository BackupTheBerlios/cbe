// GLinotteObject.cc - source file for the CBE project
// Copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen
//                     http://www.uni-muenchen.de/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#include "GLinotteObject.h"
#include "linotte/material_manager_t.h"
#include "linotte/file_input_stream_t.h"

GLinotteObject::GLinotteObject() :
	m_model( 0 )
{
	m_matmgr = new linotte::material_manager_t;
}

GLinotteObject::~GLinotteObject()
{
	delete m_model;
	delete m_matmgr;
}

void GLinotteObject::submit()
{
	m_model->draw();
}

void GLinotteObject::load( const char* path )
{
	if( m_model )
	{
		delete m_model;
		m_model = 0;
	}

	linotte::file_input_stream_t stream( path );
	
	m_model = new linotte::model_t( stream, m_matmgr );
	
	m_bsphere.radius = 0;
	m_model->get_bsphere( m_bsphere );
}

void GLinotteObject::add_material( linotte::material_t* material )
{
	m_matmgr->add_material( material );
}

linotte::material_t* GLinotteObject::get_material( const char* name )
{
	return m_matmgr->material_by_name( name );
}

float GLinotteObject::radius() const
{
	return m_bsphere.radius;
}

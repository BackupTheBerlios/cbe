// SedanCar.cc - source file for the CBE project
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

#include <GL/glu.h>
#include "config.h"
#include "SedanCar.h"
#include "linotte/simple_material_t.h"

using namespace linotte;

SedanCar::SedanCar()
{
  string datadirectory = (string)DATADIR + "/cbe/sedan.csp";

  load( datadirectory.c_str() );

  m_car_color = 0;
  update_car_color();

  m_brake_lights = false;
  update_brake_lights();
}

void
SedanCar::submit()
{
	/*if( mSpeed > 0 )
	  glTranslatef( 0, 0, -1.75 );
  	else
	  glTranslatef( 0, 0, 1.75 );*/
  	
  glRotatef( -mRotation, 0, 1, 0 );
  
  glRotatef( -90, 1, 0, 0 );
  
  glScalef( 0.6, 0.6, 1.0 );
  
  GLinotteObject::submit();
  
  /*static GLUquadricObj* q = 0;
    if( q == 0 )
    q = gluNewQuadric();
    
    gluSphere( q, 1, 8, 8 );*/
}

void
SedanCar::change( int changeNum )
{
	switch( changeNum )
	{
		case Car::change_toggleBrakeLight:
		{
			m_brake_lights = not m_brake_lights;
			update_brake_lights();
			break;
		}
	
		case Car::change_nextColor:
		{
			m_car_color += 1;
			update_car_color();
			break;
		}
	}
}

void
SedanCar::update_brake_lights()
{
	simple_material_t* mat;
	mat = dynamic_cast<simple_material_t*>(
		get_material( "BRAKELIGHTS" ) );
	if( mat )
	{
		GLfloat col[ 4 ] = { 0, 0, 0, 1 };
		if( m_brake_lights )
			col[ 0 ] = 1;
		mat->set_color( col );
	}
}

void
SedanCar::update_car_color()
{
	static GLfloat s_car_color[ 3 ][ 4 ] = {
		{ 0, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0.5, 0, 0.5, 0 } };

	simple_material_t* mat;
	mat = dynamic_cast<simple_material_t*>(
		get_material( "BODY_black_32" ) );
	if( mat )
	{
		mat->set_color( s_car_color[ m_car_color % 3 ] );
	}
}

// Car.cc - source file for the CBE project
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

#include "config.h"
#include <cmath>
#include <iostream>
extern "C" {
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
}
#include "Point.h"
#include "Car.h"
#include "street.h"

Car::Car()
{
	mRotation = 0;
	mSpeed = 0.005;
	mOffset = 0;
}

void Car::move( street* street, double time )
{
	GLfloat location[ 3 ];
	float phi = mOffset + time * mSpeed;

	street->getstreetLocation( phi, location );
	
	GLfloat p[ 3 ];
	GLfloat v[ 3 ];
	street->getstreetLocation( phi + mSpeed / 5, p );
	v[ 0 ] = p[ 0 ] - location[ 0 ];
	v[ 1 ] = p[ 1 ] - location[ 1 ];
	v[ 2 ] = p[ 2 ] - location[ 2 ];
	
	mRotation = atan2( v[ 2 ], v[ 0 ] ) * ( 180 / 3.1415 );
	
	/*float d = sqrt( v[ 2 ] * v[ 2 ] + v[ 0 ] * v[ 0 ] );
    float nx, nz;
    nx = -v[ 2 ] / d;
    nz = v[ 0 ] / d;
    nx *= 5.0 / 3;
    nz *= 3.0 / 3;*/
    
    float angle = ( mRotation + 90.0 ) * ( 3.14159 / 180.0 );
    float nx = cos( angle ) * 1.5;
    float nz = sin( angle ) * 1.5;
    
	setPos( location[ 0 ] + nx,
		location[ 1 ] + 0.5,
		location[ 2 ] + nz );
}

void Car::setSpeed( float speed )
{
	mSpeed = speed;
}

void Car::setOffset( float offset )
{
	mOffset = offset;
}

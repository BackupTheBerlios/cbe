// Point.cc - source file for the CBE project
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

extern "C" {
#ifdef _WIN32
#include <GL/glaux.h>
#endif
#include <GL/gl.h>
}
#include "Point.h"


Point::Point() {
  set( 0, 0, 0 );
}

Point::Point(GLfloat X, GLfloat Y, GLfloat Z) {
  set( X, Y, Z );
}

void Point::set( Point p ) {
	set( p.x, p.y, p.z );
}

void Point::set(GLfloat X, GLfloat Y, GLfloat Z) {
  x = X;
  y = Y;
  z = Z;
}

void Point::move( Point p ) {
	move( p.x, p.y, p.z );
}

void Point::move(GLfloat X, GLfloat Y, GLfloat Z) {
  x += X;
  y += Y;
  z += Z;
}

Point::~Point() {
}

// GMovableObject.cc - source file for the CBE project
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
#include <GL/gl.h>    // OpenGL
}
#include "GMovableObject.h"


GMovableObject::GMovableObject() {
  move( 0, 0, 0 );
  angle = 0;
}

GMovableObject::~GMovableObject() {}

void GMovableObject::move( Point p ) {
  position.move( p );
}

void GMovableObject::move( GLfloat x, GLfloat y, GLfloat z ) {
  position.move( x, y, z );
}

void GMovableObject::setPos( Point p ) {
  position.set( p );
}

void GMovableObject::setPos( GLfloat x, GLfloat y, GLfloat z ) {
  position.set( x, y, z );
}

void GMovableObject::setAngle( GLfloat r ) {
  angle = r;
}

void GMovableObject::rotate( GLfloat r ) {
  angle += r;
}

void GMovableObject::draw() {
  if ( !isHidden() ) {
    glPushMatrix();
    // Move the object to its position
    glTranslatef( position.x, position.y, position.z );
    // Rotate the object by the object angle around the vertical axis
    glRotatef( angle, 0, 1, 0 );
    // Draw the object
    glCallList( getList() );
    glPopMatrix();
  }
}

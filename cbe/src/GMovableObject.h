// GMovableObject.h - source file for the CBE project
//
// Copyright given to the public domain
// Please email comments to stetten@acpub.duke.edu
//
// Modifications are copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen
//                                       http://www.uni-muenchen.de/
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

#ifndef __G_MOVABLE_OBJECT_H
#define __G_MOVABLE_OBJECT_H

#ifdef _WIN32
  #include <GL/glaux.h>
#endif

#include <GL/gl.h>
#include "Point.h"
#include "GObject.h"
#include "time.h"

class GMovableObject : public GObject {
private:
	Point position;
	GLfloat angle;
public:
	GMovableObject();
	~GMovableObject();
	// Move relatively to the given coordinates
	void move( Point p ); 
	void move( GLfloat x, GLfloat y, GLfloat z );
	// Set new object position
	void setPos( Point p );
	void setPos( GLfloat x, GLfloat y, GLfloat z );
	// Set object rotation angle around the vertical axis
	void setAngle( GLfloat r );
	// Rotate object around the vertical axis relatively to the current angle
	void rotate( GLfloat r );
	// The draw() procedure must be adjusted for object movements
	void draw();
};

#endif
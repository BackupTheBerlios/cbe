// street.h - source file for the CBE project
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

#ifndef __STREET_H_
#define __STREET_H_

#include "Point.h"
#include "GObject.h" // The base class for graphic objects

class Street: public GObject
{
 private:
  Point points[1010];
  long length;
  GLfloat step;
 public:
  GLfloat startx, starty, startz;
  GLfloat broadness;
  
  Point getPointOfStreet(GLfloat); // Gets Point near location (0<=location<=1)
  Point getNormalOfStreet(GLfloat); // Gets the normal near location
  void writeList();
  void createPoles(GLfloat size, GLfloat x, GLfloat y, GLfloat z);
  
  Street(GLfloat x, GLfloat y, GLfloat z, GLfloat broad);
  ~Street();
};

#endif

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
#include "GListObject.h" // The base class for graphic objects
#include "linotte/texture_material_t.h"

class street: public GListObject
{
 private:
  Point points[5050];
  long length;
  GLfloat step;
  linotte::texture_material_t* mTreeMaterial;
  GLuint mTreeList;
 
  void makeTreeList();
public:
  GLfloat startx, starty, startz;
  GLfloat broadness;
  
  Point getPointOfstreet(GLfloat); // Gets Point near location (0<=location<=1)
  Point getNormalOfstreet(GLfloat); // Gets the normal near location
  void writeList();
  virtual void draw();
  void createPoles(GLfloat size, GLfloat x, GLfloat y, GLfloat z);
  void createTree(GLfloat, GLfloat, GLfloat, GLfloat);
  void getstreetLocation( double alpha, GLfloat* location );
  
  street(GLfloat x, GLfloat y, GLfloat z, GLfloat broad);
  ~street();
};

#endif

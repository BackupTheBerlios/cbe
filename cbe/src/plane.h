// plane.h - source file for the CBE project
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

#include "GListObject.h"
#include "linotte/texture_material_t.h"
#include <GL/glu.h>

class Plane : public GListObject
{
  Point a,b;
  linotte::texture_material_t* mGrassMaterial;
  linotte::texture_material_t* mCloudsMaterial;
  GLfloat mCloudMotion;
  GLUquadricObj* mQuadratic;
  
  void drawSky();

public:
  Plane(Point,Point);
  ~Plane();
  virtual void writeList();
  virtual void submit();
};

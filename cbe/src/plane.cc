// plane.cc - source file for the CBE project
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
#include <GL/glut.h>  // GLUT
}

#include "common.h"
#include "plane.h"

Plane::Plane(Point p1, Point p2)
{
  a.x=p1.x; a.y=p1.y; a.z=p1.z;
  b.x=p2.x; b.y=p2.y; b.z=p2.z;
}

Plane::~Plane()
{
}

GLuint Plane::getPlane()
{
  GLuint list=glGenLists(1);
  glNewList(list, GL_COMPILE);
    glBegin(GL_QUADS);
      glColor3f(0,1,0);
      glVertex3f(a.x,a.y,a.z);
      glVertex3f(a.x,a.y,b.z);
      glVertex3f(b.x,b.y,b.z);
      glVertex3f(b.x,a.y,a.z);
    glEnd();
  glEndList();
  return list;
}

// street.cc - source file for the CBE project
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

#include <cmath>
#include <iostream>
#include <stdlib.h>

extern "C"
{
  #include <GL/gl.h>
  #include <GL/glut.h>
}
#include "config.h"
#include "common.h"
#include "street.h"

Street::Street(GLfloat x, GLfloat y, GLfloat z, GLfloat broad) 
{
  length=1000L;
  step=0.1;
  startx=x;
  starty=y;
  startz=z;
  broadness=broad;
  createStreet();
}

Street::~Street()
{
}

// fills the points-Array which holds the points for the center of the street
void Street::createStreet()
{
  points[0].x=startx;
  points[0].y=starty;
  points[0].z=startz;

  points[1].x=startx+step;
  points[1].y=starty;
  points[1].z=startz;

  for (long n=2;n<length;n+=10)
    {
      int r;
      r=rand()%100;
      if (r<80)
	for (long i=n;i<n+10;i++)
	  {
	    points[i].x=points[i-1].x+step;
	    points[i].z=points[i-1].z;
	    points[i].y=starty;
	  }
	
      if (r>=80 && r<90)
	for (int i=n;i<n+10;i++)
	    {
	      points[i].x=points[i-1].x+step;
	      points[i].z=points[i-1].z+step*2;
	      points[i].y=starty;
	    }
	
      if (r>=90)
	for (int i=n;i<n+10;i++)
	    {
 	      points[i].x=points[i-1].x+step;
	      points[i].z=points[i-1].z-step*2;
	      points[i].y=starty;
	    }
	
    }
}

GLuint Street::getStreet()
{
  GLuint list=glGenLists(1);

  glNewList(list, GL_COMPILE);
  
  glBegin(GL_TRIANGLE_STRIP);
    glColor3f(.3,.3,.3);

    for (long i=1;i<999;i++)
      {
	GLfloat vx,vy,vz,nx,ny,nz;

	vx=points[i+1].x-points[i-1].x;
	//vy=0;
	vz=points[i+1].z-points[i-1].z;
	
	nx=-vz/sqrt(vx*vx+vz*vz);
	//ny=vy;
	nz=vx/sqrt(vx*vx+vz*vz);

	glVertex3f(points[i].x+nx*broadness/2,
		   points[i].y,
		   points[i].z+nz*broadness/2);
	glVertex3f(points[i].x-nx*broadness/2,
		   points[i].y,
		   points[i].z-nz*broadness/2);
      }
  glEnd();
  glEndList();

  return list;
}

Point Street::getPointOfStreet(GLfloat t)
{
  Point p;
  if (t<0 || t>1)
    p=points[0];
  else
    p=points[(int)t*1000];
  return p;
}

Point Street::getNormalOfStreet(GLfloat t)
{
  Point p;

  //To be implemented

  return p;
}

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
extern "C" {
#include <stdlib.h>

#ifdef _WIN32
  #include <GL/glaux.h>
#endif

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
  long i;
  for (i=1;i<999;i++)
	{
		GLfloat vx,vz,nx,nz;
		//claculate normal of street
		vx=points[i+1].x-points[i-1].x;
		vz=points[i+1].z-points[i-1].z;
		nx=-vz/sqrt(vx*vx+vz*vz);
		nz=vx/sqrt(vx*vx+vz*vz);
		//draw street out of triangles
		glVertex3f(points[i].x+nx*broadness/2,
			   points[i].y,
			   points[i].z+nz*broadness/2);
		glVertex3f(points[i].x-nx*broadness/2,
			   points[i].y,
			   points[i].z-nz*broadness/2);
	}
  glEnd();

  // Draw the TingsNextToStreet
  for (i=1;i<999;i+=20)
    {
      GLfloat vx,vz,nx,nz;
      
      // Calculate the normal to the street
      vx=points[i+1].x-points[i-1].x;
      vz=points[i+1].z-points[i-1].z;
      nx=-vz/sqrt(vx*vx+vz*vz);
      nz=vx/sqrt(vx*vx+vz*vz);
      
      createThingsNextToStreet(1,points[i].x+nx*broadness/2,points[i].y,points[i].z+nz*broadness/2);
      createThingsNextToStreet(1,points[i].x-nx*broadness/2,points[i].y,points[i].z-nz*broadness/2);
    }
  glEndList();

  return list;
}


// getPointOfStreet takes the parameter 0<=t<=1 and returns the equivalent
// centre-point of street
Point Street::getPointOfStreet(GLfloat t)
{
  Point p;
  if (t<0 || t>1)
    p=points[0];
  else
    p=points[(int)(t*1000)];
  return p;
}

Point Street::getNormalOfStreet(GLfloat t)
{
  Point p;

  //To be implemented

  return p;
}

// draws ThingsNextToStreet (don't know how they are called) -- quite simple and very static
void Street::createThingsNextToStreet(GLfloat size, GLfloat x, GLfloat y, GLfloat z)
{
  glColor3f(.9,.9,.9);
  glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-0.2*size+x,0*size+y,-0.2*size+z);
    glVertex3f(-0.2*size+x,1*size+y,-0.2*size+z);
    glVertex3f(0.2*size+x,0*size+y,-0.2*size+z);
    glVertex3f(0.2*size+x,1*size+y,-0.2*size+z);
    glVertex3f(0*size+x,0*size+y,0.2*size+z);
    glVertex3f(0*size+x,1*size+y,0.2*size+z);
    glVertex3f(-0.2*size+x,0*size+y,-0.2*size+z);
    glVertex3f(-0.2*size+x,1*size+y,-0.2*size+z);
  glEnd();
  glColor3f(.1,.1,.1);
  glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-0.2*size+x,1*size+y,-0.2*size+z);
    glVertex3f(-0.2*size+x,1.3*size+y,-0.2*size+z);
    glVertex3f(0.2*size+x,1*size+y,-0.2*size+z);
    glVertex3f(0.2*size+x,1.3*size+y,-0.2*size+z);
    glVertex3f(0*size+x,1*size+y,0.2*size+z);
    glVertex3f(0*size+x,1.3*size+y,0.2*size+z);
    glVertex3f(-0.2*size+x,1*size+y,-0.2*size+z);
    glVertex3f(-0.2*size+x,1.3*size+y,-0.2*size+z);
  glEnd();
  glBegin(GL_TRIANGLES);
    glVertex3f(-0.2*size+x,1.3*size+y,-0.2*size+z);
    glVertex3f(0.2*size+x,1.3*size+y,-0.2*size+z);
    glVertex3f(0*size+x,1.3*size+y,0.2*size+z);
  glEnd();
}

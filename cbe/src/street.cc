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
#include <GL/gl.h>
#include <GL/glut.h>
}
#include "config.h"
#include "Point.h"
#include "street.h"
#include "config.h"
#include "stdio.h"

street::street(GLfloat x, GLfloat y, GLfloat z, GLfloat broad) {
  length=2500L;
  step=0.2;
  startx=x;
  starty=y;
  startz=z;
  broadness=broad;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  string path = (string)DATADIR + "/pixmaps/cbe/tree.tif";
  mTreeMaterial = new linotte::texture_material_t( "tree", path.c_str() );

  int err = glGetError();
  cout << "gl error: " << err << endl;

  makeList();

  /*glDisable(GL_COLOR_MATERIAL);
  glEnable( GL_TEXTURE_2D );
  glEnable(GL_BLEND);*/

  makeTreeList();
}


street::~street() {
  // The destructor of the base class is called automatically
  // as the destructors are declared virtual.
  glDeleteLists( mTreeList, 1 );
  delete mTreeMaterial;
}

void street::getstreetLocation( double alpha, GLfloat* location )
{
	alpha = 1 - fmod( alpha, 1 );

	double index = alpha * length;
	long intindex = index;
	
	double delta = index - intindex;
	double v0, v1;
	
	v0 = points[ intindex ].x;
	v1 = points[ intindex + 1 ].x;
	location[ 0 ] = v0 + ( v1 - v0 ) * delta;

	v0 = points[ intindex ].y;
	v1 = points[ intindex + 1 ].y;
	location[ 1 ] = v0 + ( v1 - v0 ) * delta;

	v0 = points[ intindex ].z;
	v1 = points[ intindex + 1 ].z;
	location[ 2 ] = v0 + ( v1 - v0 ) * delta;
	
	//printf( "%f %f %f\n", location[ 0 ], location[ 1 ], location[ 2 ] );
}

void street::writeList() {
  points[0].x=startx;
  points[0].y=starty;
  points[0].z=startz;

  points[1].x=startx+step;
  points[1].y=starty;
  points[1].z=startz;
  int a;

  for (long n=2;n<length;n+=50) {
    int r;
    r=rand()%100;
    a=0;
    if (r<80)
      for (long i=n;i<n+50;i++) {
	points[i].x=points[i-1].x+step;
	points[i].z=points[i-1].z;
	points[i].y=starty;
      }
    
    if (r>=80 && r<90)
      for (int i=n;i<n+50;i++) {
	points[i].x=points[i-1].x+step;
	points[i].z=points[i-1].z+step*sin((double)a/17);
	points[i].y=starty;
	a++;
      }
    
    if (r>=90)
      for (int i=n;i<n+50;i++) {
	points[i].x=points[i-1].x+step;
	points[i].z=points[i-1].z-step*sin((double)a/17);
	points[i].y=starty;
	a++;
      }
  }

  GLfloat dy0 = 0.02;
  // introduced this parameter to elevate the
  // street over the landscape due to Z buffer
  // precision problems on my ATI rage (BL101901)
  
  glBegin(GL_QUAD_STRIP);
  glColor3f(.3,.3,.3);
  long i;
  for (i=1;i<2449;i++) {
    GLfloat vx,vz,nx,nz;
    //claculate normal of street
    vx=points[i+1].x-points[i].x;
    vz=points[i+1].z-points[i].z;
    nx=-vz/sqrt(vx*vx+vz*vz);
    nz=vx/sqrt(vx*vx+vz*vz);
    //draw street out of triangles
    glVertex3f(points[i].x,
	       points[i].y+dy0,
	       points[i].z+broadness/2);
    glVertex3f(points[i].x,
	       points[i].y+dy0,
	       points[i].z-broadness/2);
  }
  glEnd();
  
  // Draw the poles
  for (i=1;i<2449;i+=20) {
    GLfloat vx,vz,nx,nz;
    
    // Calculate the normal to the street
    vx=points[i+1].x-points[i].x;
    vz=points[i+1].z-points[i].z;
    nx=-vz/sqrt(vx*vx+vz*vz);
    nz=vx/sqrt(vx*vx+vz*vz);
    
    createPoles(0.6,points[i].x,points[i].y,points[i].z+broadness/2+.5);
    createPoles(0.6,points[i].x,points[i].y,points[i].z-broadness/2-.5);
  }

  // Draw Middlelines
  glColor3f(.9,.9,.9);
  for (i=1;i<2449;i+=10){
    glBegin(GL_POLYGON);
    glVertex3f(points[i].x,
	       points[i].y+dy0+.01,
	       points[i].z+broadness/30);
    glVertex3f(points[i].x,
	       points[i].y+dy0+.01,
	       points[i].z-broadness/30);
    glVertex3f(points[i+6].x,
	       points[i+6].y+dy0+.01,
	       points[i+6].z-broadness/30);
    glVertex3f(points[i+6].x,
	       points[i+6].y+dy0+.01,
	       points[i+6].z+broadness/30);
    glEnd();
  } 
  
}

void street::makeTreeList()
{
  mTreeList = glGenLists( 1 );
  glNewList( mTreeList, GL_COMPILE );
  
  srandom( 1234 );

  // Draw the trees
  for (int i=1;i<2449;i+=50) {
    GLfloat vx,vz,nx,nz;
    
    // Calculate the normal to the street
    vx=points[i+1].x-points[i].x;
    vz=points[i+1].z-points[i].z;
    nx=-vz/sqrt(vx*vx+vz*vz);
    nz=vx/sqrt(vx*vx+vz*vz);

    int r = random();
    float dz = ( r / (float)RAND_MAX ) * 1.5;
    
    r = random();
    float dx = ( ( r - RAND_MAX / 2 ) / (float)( RAND_MAX / 2 ) ) * 3;

    createTree(1,points[i].x + dx, points[i].y, points[i].z+broadness/2+.5 + 2 + dz);
    createTree(1,points[i].x + dx, points[i].y, points[i].z-broadness/2-.5 - 2 - dz);
  }

  glEndList();
}

void street::draw()
{
  glDisable(GL_COLOR_MATERIAL);
  glEnable( GL_TEXTURE_2D );
  glEnable(GL_BLEND);

  //mTreeMaterial->submit();
  glCallList( mTreeList );
  //makeTreeList();

  glDisable( GL_TEXTURE_2D );

  GListObject::draw();
}

// getPointOfstreet takes the parameter 0<=t<=1 and returns the equivalent
// centre-point of street
Point street::getPointOfstreet(GLfloat t) {
  Point p;
  
  if (t<0 || t>1)
    p=points[0];
  else
    p=points[(int)(t*1000)];
  
  return p;
}


//To be implemented
Point street::getNormalOfstreet(GLfloat t) {
  Point p;
  return p;
}


// draws poles (don't know how they are called) -- quite simple and very static
void street::createPoles(GLfloat size, GLfloat x, GLfloat y, GLfloat z) {
  glColor3f(.9,.9,.9);
  glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-0.1*size+x,0*size+y,-0.1*size+z);
    glVertex3f(-0.1*size+x,1*size+y,-0.1*size+z);
    glVertex3f(0.1*size+x,0*size+y,-0.1*size+z);
    glVertex3f(0.1*size+x,1*size+y,-0.1*size+z);
    glVertex3f(0*size+x,0*size+y,0.1*size+z);
    glVertex3f(0*size+x,1*size+y,0.1*size+z);
    glVertex3f(-0.1*size+x,0*size+y,-0.1*size+z);
    glVertex3f(-0.1*size+x,1*size+y,-0.1*size+z);
  glEnd();
  glColor3f(.1,.1,.1);
  glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-0.1*size+x,1*size+y,-0.1*size+z);
    glVertex3f(-0.1*size+x,1.3*size+y,-0.1*size+z);
    glVertex3f(0.1*size+x,1*size+y,-0.1*size+z);
    glVertex3f(0.1*size+x,1.3*size+y,-0.1*size+z);
    glVertex3f(0*size+x,1*size+y,0.1*size+z);
    glVertex3f(0*size+x,1.3*size+y,0.1*size+z);
    glVertex3f(-0.1*size+x,1*size+y,-0.1*size+z);
    glVertex3f(-0.1*size+x,1.3*size+y,-0.1*size+z);
  glEnd();
  glBegin(GL_TRIANGLES);
    glVertex3f(-0.1*size+x,1.3*size+y,-0.1*size+z);
    glVertex3f(0.1*size+x,1.3*size+y,-0.1*size+z);
    glVertex3f(0*size+x,1.3*size+y,0.1*size+z);
  glEnd();
}


void street::createTree(GLfloat size, GLfloat x, GLfloat y, GLfloat z) {
  float width = 2;
  float height = 10;

  glColor3f( 1, 1, 1 );

  glBegin( GL_QUADS );
  glTexCoord2f( 1, 1 );  
  glVertex3f( x, y + height, z + width );
  glTexCoord2f( 1, 0 );
glVertex3f( x, y, z + width);  
  glTexCoord2f( 0, 0 );
glVertex3f( x, y, z );
  glTexCoord2f( 0, 1 );  
  glVertex3f( x, y + height, z );
  glEnd();
}

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

  mMiddlelineToggle = true;

  int err = glGetError();
  cout << "gl error: " << err << endl;

  makeList();

  /*glDisable(GL_COLOR_MATERIAL);
  glEnable( GL_TEXTURE_2D );
  glEnable(GL_BLEND);*/

  makeTreeList();
  
  makeMiddlelineLists();
}


street::~street() {
  // The destructor of the base class is called automatically
  // as the destructors are declared virtual.
  glDeleteLists( mTreeList, 1 );
  delete mTreeMaterial;
}

void street::getstreetLocation( double alpha, GLfloat* location )
{
	alpha = fmod( alpha, 1 );
	
	if( alpha < 0 )
		alpha += 1;

	alpha = 1 - alpha;

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
}

const GLfloat dy0 = 0.05;

void street::writeList() {  
  float xradius = 1000;
  float yradius = 200;
  const int pointcount = length;
  
  for( int i = 0; i < pointcount; i++ )
  {
      float angle = ( 3.14159 * 2 ) * ( i / (float)( pointcount - 60 ) );
      points[ i ].x = startx + cos( angle ) * xradius;
      points[ i ].z = ( startz - yradius - 0.1 ) + sin( angle ) * yradius;  	
      points[ i ].y = starty;
  }

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
    
    nx *= broadness / 2;
    nz *= broadness / 2;
    
    //draw street out of triangles
    glVertex3f(points[i].x+nx,
	       points[i].y+dy0,
	       points[i].z+nz);
    glVertex3f(points[i].x-nx,
	       points[i].y+dy0,
	       points[i].z-nz);
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
    
    nx *= ( broadness / 2 + 0.5 );
    nz *= ( broadness / 2 + 0.5 );
    
    createPoles(0.6,points[i].x+nx,points[i].y,points[i].z+nz);
    createPoles(0.6,points[i].x-nx,points[i].y,points[i].z-nz);
  }
}

void street::makeMiddlelineHalf()
{
  const GLfloat dy1 = dy0 + 0.01;

  // Draw Middlelines
  glColor3f(.9,.9,.9);
  for (int i=1;i<2449;i+=10){

    GLfloat vx,vz,nx,nz;
    
    // Calculate the normal to the street
    vx=points[i+1].x-points[i].x;
    vz=points[i+1].z-points[i].z;
    nx=-vz/sqrt(vx*vx+vz*vz);
    nz=vx/sqrt(vx*vx+vz*vz);

	nx *= broadness/50;
	nz *= broadness/50;

    glBegin(GL_POLYGON);
    glVertex3f(points[i].x+nx,
	       points[i].y+dy1+.01,
	       points[i].z+nz);
    glVertex3f(points[i].x-nx,
	       points[i].y+dy1+.01,
	       points[i].z-nz);
    glVertex3f(points[i+6].x-nx,
	       points[i+6].y+dy1+.01,
	       points[i+6].z-nz);
    glVertex3f(points[i+6].x+nx,
	       points[i+6].y+dy1+.01,
	       points[i+6].z+nz);
    glEnd();
  } 
}

void street::makeMiddlelineThrough()
{
  int step = 10;
  const GLfloat dy1 = dy0 + 0.05;

  // Draw Middlelines
  glColor3f(.9,.9,.9);
  for (int i=1;i<2449;i+=step){

    GLfloat vx,vz,nx,nz;
    
    // Calculate the normal to the street
    vx=points[i+1].x-points[i].x;
    vz=points[i+1].z-points[i].z;
    nx=-vz/sqrt(vx*vx+vz*vz);
    nz=vx/sqrt(vx*vx+vz*vz);

	nx *= broadness/50;
	nz *= broadness/50;

    glBegin(GL_POLYGON);
    glVertex3f(points[i].x+nx,
	       points[i].y+dy1+.01,
	       points[i].z+nz);
    glVertex3f(points[i].x-nx,
	       points[i].y+dy1+.01,
	       points[i].z-nz);
    glVertex3f(points[i+step].x-nx,
	       points[i+step].y+dy1+.01,
	       points[i+step].z-nz);
    glVertex3f(points[i+step].x+nx,
	       points[i+step].y+dy1+.01,
	       points[i+step].z+nz);
    glEnd();
  } 
}

void street::makeMiddlelineLists()
{
  mMiddlelineHalf = glGenLists( 1 );
  glNewList( mMiddlelineHalf, GL_COMPILE );
  makeMiddlelineHalf();
  glEndList();  

  mMiddlelineThrough = glGenLists( 1 );
  glNewList( mMiddlelineThrough, GL_COMPILE );
  makeMiddlelineThrough();
  glEndList();  
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

	nx *= ( broadness / 2 + 0.5 + 2 + dx );
	nz *= ( broadness / 2 + 0.5 + 2 + dz );

    createTree(1,points[i].x + nx, points[i].y, points[i].z+nz);
    createTree(1,points[i].x - nx, points[i].y, points[i].z-nz);
  }

  glEndList();
}

void street::draw()
{
  glDisable(GL_COLOR_MATERIAL);
  glEnable( GL_TEXTURE_2D );
  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // added 042402

  mTreeMaterial->submit();
  glCallList( mTreeList );
  //makeTreeList();

  glDisable( GL_TEXTURE_2D );

  GListObject::draw();

  if( mMiddlelineToggle )
	glCallList( mMiddlelineHalf );
  else
	glCallList( mMiddlelineThrough );
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

void street::toggleMiddleline()
{
  mMiddlelineToggle = !mMiddlelineToggle;
}

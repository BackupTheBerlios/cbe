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
#include <GL/gl.h>    // OpenGL
#include <GL/glut.h>  // GLUT
}
#include "Point.h"
#include "plane.h"
#include "config.h"

extern GLfloat gLocation[ 3 ];

Plane::Plane(Point p1, Point p2) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  string path = (string)DATADIR + "/pixmaps/cbe/grass.tif";
  mGrassMaterial = new linotte::texture_material_t( "grass", path.c_str() );

  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  path = (string)DATADIR + "/pixmaps/cbe/tree.tif";
  mCloudsMaterial = new linotte::texture_material_t( "clouds", path.c_str() );
  mCloudMotion = 0;
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
  
  mQuadratic = gluNewQuadric();
  gluQuadricNormals( mQuadratic, GLU_SMOOTH );
  gluQuadricTexture( mQuadratic, GL_TRUE );

  a.x=p1.x; a.y=p1.y; a.z=p1.z;
  b.x=p2.x; b.y=p2.y; b.z=p2.z;
  makeList(); // Create the GL list for drawing the object
}


Plane::~Plane() {
	delete mGrassMaterial;
	delete mCloudsMaterial;
	gluDeleteQuadric( mQuadratic );
}


void Plane::writeList() {
  GLfloat count = 100;

  glBegin(GL_QUADS);
    glColor3f(0,1,0);
    glTexCoord2f( 0, 0 );
    glVertex3f(a.x,a.y,a.z);
    glTexCoord2f( count, 0 );
    glVertex3f(a.x,a.y,b.z);
    glTexCoord2f( count, count );
    glVertex3f(b.x,b.y,b.z);
    glTexCoord2f( 0, count );
    glVertex3f(b.x,a.y,a.z);
  glEnd();
}

void Plane::submit()
{
	drawSky();
	mGrassMaterial->submit();
	GListObject::submit();
}

void Plane::drawSky()
{
	static const GLdouble gClipPlane[] = { 0, 1, 0, 0 };

	glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
	glColor4f( 1.0, 1.0, 1.0, 0.5 );
	glEnable( GL_BLEND );

	glPushMatrix();
	
	glTranslatef(
		-gLocation[ 0 ],
		-gLocation[ 1 ],
		-gLocation[ 2 ] );
	
	mCloudMotion = clock() * ( 1 / (float)CLOCKS_PER_SEC );
	
	glClipPlane( GL_CLIP_PLANE0, gClipPlane );
	glEnable( GL_CLIP_PLANE0 );
	
	glRotatef( mCloudMotion, 0, 1, 0 );
	glRotatef( 90, 1, 0, 0 );
	mCloudsMaterial->submit();
	gluSphere( mQuadratic, 90, 15, 15 );
	
	glDisable( GL_CLIP_PLANE0 );
	
	glPopMatrix();
	
	// animate
	//mCloudMotion += 0.5;
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable( GL_BLEND );
}

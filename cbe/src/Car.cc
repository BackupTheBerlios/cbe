// Car.cc - source file for the CBE project
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
#include "Point.h"
#include "Car.h"


const GLfloat Car::color0[] = { 0.75, 0.75, 0.75 };
const GLfloat Car::color1[] = { 0.8, 0.4, 0.0 };
const GLfloat* Car::colors[] = { Car::color0, Car::color1 };
const GLfloat Car::brakeLightColors[2][3] = { { 0.7, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } };

Car::Car() {
  currColorNumber = 0;         // Start with the first car color
  currBrakeLightNumber = 0;    // The brake lights are off at the beginning
  
  width = 1.8;
  length = 4.0;
  x1Back = -length / 2;
  x2Back = x1Back + 0.1;
  x3Back = x1Back + 0.7;
  x4Back = x3Back + 0.2;
  x1Front = length / 2;
  x2Front = x1Front - 0.2;
  x3Front = x3Back + 2.2;
  x4Front = x3Front - 0.4;
	
  h1Back = 0.2;
  h1Front = 0.15;
  h2Back = 0.8;
  h2Front = 0.7;
  h3Back = 0.9;
  h3Front = 0.8;
  h4Back = 1.5;
  h4Front = 1.4;

  wheelRadius1 = 0.3;
  wheelRadius2 = 0.2;
  wheelWidth1 = 0.14;
  wheelWidth2 = 0.10;
  frontWheelx = x1Front - 0.6;
  backWheelx = x1Back + 0.6;

  brakeLightWidth = 0.2;
  brakeLightHeight = 0.1;
  brakeLightx = width/2 - 0.3;
  brakeLighty = h2Back - 0.3;

  gl_wheelList = glGenLists( 1 );
  gl_brakeLightList = glGenLists( 1 );
  makeList();
}


Car::~Car() {
  glDeleteLists( gl_wheelList, 1 );
  glDeleteLists( gl_brakeLightList, 1 );
}


void Car::change( int changeNum ) {
  if ( ( changeNum >= change_minColor ) && ( changeNum <= change_nextColor ) ) {
    changeColor( changeNum );
  }
  else {
    switch ( changeNum ) {
    case change_brakeLightOn:
      currBrakeLightNumber = 1;
      break;
    case change_brakeLightOff:
      currBrakeLightNumber = 0;
      break;
    case change_toggleBrakeLight:
      currBrakeLightNumber = 1 - currBrakeLightNumber;
    }
  }
}

void Car::changeColor( int changeNum ) {
  if ( ( changeNum >= change_minColor ) && ( changeNum <= change_maxColor ) )
    currColorNumber = changeNum - change_minColor;
  else if ( changeNum == change_nextColor ) {
    ++currColorNumber;                   // Next color
    
    // From the last color we go again to the first color
    if ( currColorNumber > change_maxColor - change_minColor )
      currColorNumber = 0;
  }
}


void Car::makeList() {
  glNewList( gl_wheelList, GL_COMPILE); // Initialize the wheel list
  writeWheelList();                     // Save the GL operations for a wheel to the list
  glEndList();                          // Finish the list
  glNewList( gl_brakeLightList, GL_COMPILE );
  writeBrakeLightList();
  glEndList();
  GMovableObject::makeList();           // Call inherited function to write the list gl_list.
}


void Car::writeList() {
  glPushMatrix();
  drawCoachwork();
  glPopMatrix();
}


void Car::drawObjectLists() {
  glColor3fv( brakeLightColors[ currBrakeLightNumber ] );
  glCallList( gl_brakeLightList );
  glColor3fv( colors[ currColorNumber ] );
  glCallList( getList() );         // Draw the object
}


void Car::drawCoachwork() {
  glBegin( GL_QUADS );

  // The left and the right side of the car
  for(int i = 0; i <= 1; i++ ) {
    // i == 0: left side, i == 1: right side
    glVertex3f( (i-0.5)*width, h1Back, x1Back );
    glVertex3f( (i-0.5)*width, h2Back, x2Back );
    glVertex3f( (i-0.5)*width, h2Front, x2Front );
    glVertex3f( (i-0.5)*width, h1Front, x1Front );

    glVertex3f( (i-0.5)*width, h2Back, x2Back );
    glVertex3f( (i-0.5)*width, h3Back, x3Back );
    glVertex3f( (i-0.5)*width, h3Front, x3Front );
    glVertex3f( (i-0.5)*width, h2Front, x2Front );
  }

  // The back of the car
  glVertex3f( 0.5*width, h1Back, x1Back );
  glVertex3f( 0.5*width, h2Back, x2Back );
  glVertex3f( -0.5*width, h2Back, x2Back );
  glVertex3f( -0.5*width, h1Back, x1Back );

  // The front of the car
  glVertex3f( -0.5*width, h1Front, x1Front );
  glVertex3f( -0.5*width, h2Front, x2Front );
  glVertex3f( 0.5*width, h2Front, x2Front );
  glVertex3f( 0.5*width, h1Front, x1Front );
	
  // The sides of the cabin
  // Front side
  glVertex3f( 0.5*width, h4Front, x4Front );
  glVertex3f( 0.5*width, h3Front, x3Front );
  glVertex3f( -0.5*width, h3Front, x3Front );
  glVertex3f( -0.5*width, h4Front, x4Front );

  // Back side
  glVertex3f( -0.5*width, h3Back, x3Back );
  glVertex3f( -0.5*width, h4Back, x4Back );
  glVertex3f( 0.5*width, h4Back, x4Back );
  glVertex3f( 0.5*width, h3Back, x3Back );

  for(int i = 0; i <= 1; i++ ) {
    // i == 0: left side, i == 1: right side
    glVertex3f( (i-0.5)*width, h3Back, x3Back );
    glVertex3f( (i-0.5)*width, h4Back, x4Back );
    glVertex3f( (i-0.5)*width, h4Front, x4Front );
    glVertex3f( (i-0.5)*width, h3Front, x3Front );
  }

  // The top of the car
  // Back
  glVertex3f( 0.5*width, h2Back, x2Back );
  glVertex3f( 0.5*width, h3Back, x3Back );
  glVertex3f( -0.5*width, h3Back, x3Back );
  glVertex3f( -0.5*width, h2Back, x2Back );
  // Cabin
  glVertex3f( 0.5*width, h4Back, x4Back );
  glVertex3f( 0.5*width, h4Front, x4Front );
  glVertex3f( -0.5*width, h4Front, x4Front );
  glVertex3f( -0.5*width, h4Back, x4Back );
  // Front
  glVertex3f( 0.5*width, h3Front, x3Front );
  glVertex3f( 0.5*width, h2Front, x2Front );
  glVertex3f( -0.5*width, h2Front, x2Front );
  glVertex3f( -0.5*width, h3Front, x3Front );
  glEnd();
  glColor3f( 0.3, 0.3, 0.3 );
  glBegin( GL_QUADS );
  // The bottom of the car
  glVertex3f( 0.5*width, h1Front, x1Front );
  glVertex3f( -0.5*width, h1Front, x1Front );
  glVertex3f( -0.5*width, h1Back, x1Back );
  glVertex3f( 0.5*width, h1Back, x1Back );
  glEnd();

  // Draw the wheels
  for( int j = 0; j <= 1; j++ ) { 
    // j = 0: two front wheels, j = 1: twho back wheels
    GLdouble wheelX = ( j == 0 ) ? frontWheelx:backWheelx;

    for(int i = 0; i <= 1; i++ ) { 
      // i = 0: left wheel, i = 1: right wheel
      glPushMatrix();
      // ...+0.02 to move the wheel a little bit outside the car
      GLdouble wheelCenterX = -wheelWidth1/2 + ((i==0)?-1:1)*(width/2-wheelWidth2/2 + 0.02);
      glTranslatef( wheelCenterX, wheelRadius1, wheelX );
      glRotatef( 90, 0, 1, 0 );
      glCallList( gl_wheelList );
      glPopMatrix();
    }
  }
}


void Car::writeWheelList() {
  int slices = 16;               // Number of slices approximating a circle

  glColor3f( 0.35, 0.35, 0.35 );
  
  // The outer part of the wheel
  gluCylinder( gluNewQuadric(),
	       wheelRadius1,     // base radius
	       wheelRadius1,     // top radius
	       wheelWidth1,      // height
	       slices,           // slices
	       1 );              // stacks
  
  // Wheel sides
  for( int i = 0; i <= 1; i++ ) {	
    glPushMatrix();

    if ( i > 0 )
      glTranslatef( 0, 0, wheelWidth1 );

    // The tire
    glColor3f( 0.25, 0.25, 0.25 );
    gluDisk( gluNewQuadric(),
	     wheelRadius2,       // innerRadius
	     wheelRadius1,       // outerRadius
	     slices,             // slices
	     1 );                // loops
    glPopMatrix();
    glPushMatrix();

    if ( i > 0 )
      glTranslatef( 0, 0, wheelWidth2 );

    glColor3f( 0.2, 0.2, 0.2 );
    // The strip between the tire and the wheel rim ("und der Felge")
    gluCylinder( gluNewQuadric(),
		 wheelRadius2,            // base radius
		 wheelRadius2,            // top radius
		 wheelWidth1-wheelWidth2, // height
		 slices,                  // slices
		 1 );                     // stacks
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0, 0, (wheelWidth1 - wheelWidth2)/2 + i*wheelWidth2);
    // The wheel rim (Die Felge zeichnen)
    glColor3f( 0.4, 0.4, 0.4 );
    gluDisk( gluNewQuadric(),
	     0,             // innerRadius
	     wheelRadius2,  // outerRadius
	     slices,        // slices
	     1 );           // loops
    glPopMatrix();
  }
}


void Car::writeBrakeLightList() {
  GLfloat w = brakeLightWidth,
    h = brakeLightHeight,
    x = brakeLightx,
    y = brakeLighty;
  
  glBegin( GL_QUADS );

  for(int i = 0; i <= 1; ++i ) {
    int f = 2 * i-1; // -1, if i==0, 1 if i==1
    glVertex3f( f*x + w/2, y + h/2, x1Back );
    glVertex3f( f*x + w/2, y - h/2, x1Back );
    glVertex3f( f*x - w/2, y - h/2, x1Back );
    glVertex3f( f*x - w/2, y + h/2, x1Back );
  }

  glEnd();
}


TestCar::TestCar() {
  oldTime = clock();
  makeList();
  deltaX = 0;
  deltaAngle = 0;
}


TestCar::TestCar( double dx, double dr ) {
  oldTime = clock();
  makeList();
  deltaX = dx;
  deltaAngle = dr;
}


TestCar::~TestCar() {}


void TestCar::draw() {
  // Just for testing
  double timePassed = (clock() - oldTime)/(double)CLOCKS_PER_SEC;

  oldTime = clock();
  rotate( deltaAngle * timePassed );
  move( deltaX * timePassed, 0, 0 );
  Car::draw();
}

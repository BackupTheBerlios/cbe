// cbe.cc - source file for the CBE project
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


#include <iostream>
extern "C" {
#include <GL/gl.h>    // OpenGL
#include <GL/glut.h>  // GLUT
}
#include "config.h"
#include "cbe.hh"
#include "glutMaster.h"
#include "glutWindow.h"
#include "demoWindow.h"

#define COS(X)   cos( (X) * 3.14159/180.0 )
#define SIN(X)   sin( (X) * 3.14159/180.0 )

#define RED 1
#define WHITE 2
#define CYAN 3

using namespace std;

GlutMaster* glutMaster;
DemoWindow* firstWindow = 0;
DemoWindow* secondWindow = 0;

static GLuint createWorld()
{
  GLuint list;
  GLfloat a, b;
  GLfloat da = 18.0, db = 18.0;
  GLfloat radius = 3.0;
  GLuint color;
  GLfloat x, y, z;
  GLfloat street[1000][2];
  const GLfloat step=0.1;

  list = glGenLists(1);

  street[0][0]=0;street[0][1]=-50;
  street[1][0]=0;street[1][1]=-49.9;
  for (int n = 2; n < 1000; n += 10) {
      int r=rand()%100;
      if (r<80) {
	for (int i=n;i<n+10;i++) {
	  street[i][0]=street[i-1][0];
	  street[i][1]=street[i-1][1]+step;
	}
      }
      if (r>=80 && r<90) {
	for (int i=n;i<n+10;i++) {
	  street[i][0]=street[i-1][0]+step*2;
	  street[i][1]=street[i-1][1]+step;
	}
      }
      if (r>=90) {
	for (int i=n;i<n+10;i++) {
	  street[i][0]=street[i-1][0]-step*2;
	  street[i][1]=street[i-1][1]+step;
	}
      }
  }
  
  glNewList(list, GL_COMPILE);
  
  //Plain
  glBegin(GL_QUADS);
  glColor3f(0,1,0);
  glVertex3f(-50,-50,-20);
  glColor3f(0,1,0.5);
  glVertex3f(50,-50,-20);
  glColor3f(0,1,1);
  glVertex3f(50,50,-20);
  glColor3f(0,1,0.5);
  glVertex3f(-50,50,-20);
  glEnd();
  
  //street
  glBegin(GL_TRIANGLE_STRIP);
  glColor3f(1,1,0);
  for (int i=0;i<1000;i++) {
    glVertex3f(street[i][0]-2,street[i][1],-19);
    glVertex3f(street[i][0]+2,street[i][1],-19);
  }
  glEnd();
  
  //Objects next to street
  for (int i=0;i<1000;i+=100) {
    color = 0;
    glBegin(GL_QUAD_STRIP);
    for (a = -90.0; a + da <= 90.0; a += da) {
      for (b = 0.0; b <= 360.0; b += db) {
	if (color) {
	  glIndexi(RED);
	  glColor3f(1, 0, 0);
	}
	else {
	  glIndexi(WHITE);
	  glColor3f(1, 1, 1);
	}
	
	x = radius * COS(b) * COS(a);
	y = radius * SIN(b) * COS(a);
	z = radius * SIN(a);
	if (i%200==0)
	  glVertex3f(x+street[i][0]+5, y+street[i][1], z-17);
	else
	  glVertex3f(x+street[i][0]-5, y+street[i][1], z-17);
	
	x = radius * COS(b) * COS(a + da);
	y = radius * SIN(b) * COS(a + da);
	z = radius * SIN(a + da);
	if (i%200==0)
	  glVertex3f(x+street[i][0]+5, y+street[i][1], z-17);
	else
	  glVertex3f(x+street[i][0]-5, y+street[i][1], z-17);
	
	color = 1 - color;
      }
    }
    glEnd();
  }
  
  glEndList();

  return list;
}

int main(int argc, char *argv[]) {
  cout << "Entering main in cbe.cc\n";
  glutMaster   = new GlutMaster(&argc, argv);  
  cout << "created GlutMaster in cbe.cc\n";
  //firstWindow  = new DemoWindow(glutMaster,
  //				200, 200,        // height, width
  //				200, 100,        // initPosition (x,y)
  //				(string)PACKAGE + (string)" " + (string)VERSION);  // title
  cout << "created DemoWindow firstWindow in cbe.cc\n";
  secondWindow = new DemoWindow(glutMaster,
				500, 500,         // height, width
				200, 400,         // initPosition (x,y)
				(string)PACKAGE + (string)" " + (string)VERSION);   // title
  cout << "created DemoWindow secondWindow in cbe.cc\n";
  
  GLuint list=createWorld();
  secondWindow->SetPolygonList(&list);
  
  secondWindow->StartSpinning(glutMaster);        // enable idle function
  cout << "Started Spinning in secondWindow in cbe.cc\n";
  glutMaster->CallGlutMainLoop();
  cout << "CallGlutMainLoop() success\n";

  return 0;
}

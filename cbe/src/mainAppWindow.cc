// mainAppWindow.cc - source file for the CBE project
//
// Original code by George Stetten and Korin Crawford
// Copyright given to the public domain
// Please email comments to stetten@acpub.duke.edu
//
// Modifications are copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen
//                                       http://www.uni-muenchen.de/
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
#ifdef _WIN32
#include <GL/glaux.h>
#endif
#include <GL/glut.h>
#include <time.h>
}
#include "mainAppWindow.h"
#include "glutMaster.h"
#include "glutWindow.h"
#include "Point.h"
#include <cmath>

#define M_PI 3.1415926535

using namespace std;

mainAppWindow::mainAppWindow(GlutMaster * glutMaster, int setWidth, int setHeight, int setInitPositionX, int setInitPositionY, string title) {
  
  // Set default viewing and movement vectors
  movementVector = new Point(1,0,0);
    
  // Set default speed and viewingAngle
  speed = 0.0;
  viewingAngle=0.0;

  // Set current clock-value to oldTime
  oldTime=clock();

  // Disable fog by default
  isFog = false;

  width  = setWidth;               
  height = setHeight;
  initPositionX = setInitPositionX;
  initPositionY = setInitPositionY;
  
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(initPositionX, initPositionY);
  glViewport(0, 0,(GLint) width,(GLint) height); 
  
  glutMaster->CallGlutCreateWindow(title.c_str(), this);
  
  glEnable(GL_DEPTH_TEST);  
	
  // Switch to camera matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // perspective
  gluPerspective(120.0, 1.0, 0.1, 100.0);
  
  // Set viewing direction from z-Axis to x-axis
  glRotatef(90.0, 0, -1, 0);
  
  // point of view
  glTranslatef(0.0, -1.5, 0.0);

  // set background color
  glClearColor(0.3, 0.3, 1,0);

  // Save ProjectionMatrix (for futher use in Idle-Function)
  glPushMatrix();

  // Switch to scenery matrix
  glMatrixMode(GL_MODELVIEW);
}


mainAppWindow::~mainAppWindow() {
  cout << "Destroying Demo-Window" << endl;
  delete movementVector;
  glutDestroyWindow(windowID);
}


void mainAppWindow::CallBackDisplayFunc(void) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   plane->draw();
   street->draw();

   glutSwapBuffers();
}


// Call back function for reshape state
void mainAppWindow::CallBackReshapeFunc(int w, int h) {
   width = w;
   height= h;

   glViewport(0,0,width,height); 
   CallBackDisplayFunc();
}


// Call back function for idle state
void mainAppWindow::CallBackIdleFunc(void) {
  // Switch to camera matrix
  glMatrixMode(GL_PROJECTION);

  // Get projection Matrix from Stack, but leave a copy there
  glPopMatrix();
  glPushMatrix();

  // Now we can simply turn the camera in the direction we want without considering the next frame
  glRotatef(viewingAngle,0,1,0);

  // Switch back to world matrix
  glMatrixMode(GL_MODELVIEW);
  
  GLfloat latenz=(double) speed * getTimePassed();
  
  // Now move the world according to our viewing direction
  glTranslatef(movementVector->x*latenz, movementVector->y*latenz, movementVector->z*latenz);

  // Finished with "camera-actions" advance to draw the world

  CallBackDisplayFunc();
}


// Call back function for keyboard events
void mainAppWindow::CallBackKeyboardFunc(unsigned char key, int x, int y) {
  // Determine key
  switch (key) {
  case 'Q':
  case 'q':
  case 27:
    cout << "Exit key was pressed. Bye bye." << endl;
    exit(0);  // exit is not like return - we need a nicer way to exit the program and call all destructors!
    break;
  case 'U':  // faster
  case 'u':
    speed += 0.05;
    break;
  case 'N': // slower
  case 'n':
    speed -= 0.05;
    break;
  case 'H': // left
  case 'h':
    viewingAngle--;
    if (viewingAngle>180)
      viewingAngle-=360;
    movementVector->x=cos(viewingAngle*M_PI/180);
    movementVector->z=sin(viewingAngle*M_PI/180);
    break;
  case 'J': // right
  case 'j':
    viewingAngle++;
    if (viewingAngle<-180)
      viewingAngle+=360;
    movementVector->x=cos(viewingAngle*M_PI/180);
    movementVector->z=sin(viewingAngle*M_PI/180);
    break;
  case 'F':
  case 'f':
    if (!isFog) {
      glEnable(GL_FOG);
      cout << "Fog enabled." << endl;
      isFog = true;
    }
    else {
      glDisable(GL_FOG);
      cout << "Fog disabled." << endl;
      isFog = false;
    }
    break;
  default:
    cout << "A normal key was pressed. Hurra!" << endl;
    break;
  }
}


// Call back function for special keyboard events (cursor keys, etc.)
void mainAppWindow::CallBackSpecialKeyboardFunc(unsigned char key, int x, int y) {
  cout << "A special key was pressed. Double hurra." << endl;
}


void mainAppWindow::StartSpinning(GlutMaster * glutMaster){
   glutMaster->SetIdleToCurrentWindow();
   glutMaster->EnableIdleFunction();
}

void mainAppWindow::setStreet(GObject *s) {
  street = s;
}

void mainAppWindow::setPlane(GObject *p) {
  plane = p;
}

// Calculates time passed since last call of this function
double mainAppWindow::getTimePassed() {
  clock_t tnew,ris;
  tnew=clock();
  ris=tnew-oldTime;
  oldTime=tnew;
  return(ris/(double)CLOCKS_PER_SEC);
}

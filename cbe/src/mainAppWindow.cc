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
#include <GL/glut.h>
#include "mainAppWindow.h"
#include "glutMaster.h"
#include "glutWindow.h"

using namespace std;

mainAppWindow::mainAppWindow(GlutMaster * glutMaster, int setWidth, int setHeight, int setInitPositionX, int setInitPositionY, string title) {
  // Set default rotation speed
  xRotationSpeed = 0.25;

  cout << "Entered mainAppWindow::mainAppWindow in mainAppWindow.cc" << endl;
  width  = setWidth;               
  height = setHeight;
  initPositionX = setInitPositionX;
  initPositionY = setInitPositionY;
  
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  cout << "  InitDisplayMode" << endl;
  glutInitWindowSize(width, height);
  cout << "  InitWindowSize" << endl;
  glutInitWindowPosition(initPositionX, initPositionY);
  cout << "  InitWindowPosition" << endl;
  glViewport(0, 0,(GLint) width,(GLint) height); 
  cout << "  glViewport" << endl;
  
  glutMaster->CallGlutCreateWindow(title.c_str(), this);
  
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glOrtho(-80.0, 80.0, -80.0, 80.0, -500.0, 500.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glRotatef(60, 1, 1, 1);
  glColor4f(1.0, 0.0, 0.0, 1.0);

}


mainAppWindow::~mainAppWindow() {
  cout << "Destroying Demo-Window" << endl;
  glutDestroyWindow(windowID);
}


void mainAppWindow::CallBackDisplayFunc(void) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glColor4f(1.0, 1.0, 0.0, 1.0);
   glCallList(*plane);
   glCallList(*street);
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
  glRotatef(xRotationSpeed, 1, 1, 2);
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
  case 'U':
  case 'u':
    xRotationSpeed += 0.1;
    break;
  case 'D':
  case 'd':
    xRotationSpeed -= 0.1;
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

   
void mainAppWindow::setStreet(GLuint *list) {
  street=list;
}

void mainAppWindow::setPlane(GLuint *list) {
  plane=list;
}

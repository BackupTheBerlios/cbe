// demoWindow.cc - source file for the CBE project
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

#include "demoWindow.h"

DemoWindow::DemoWindow(GlutMaster * glutMaster, int setWidth, int setHeight, int setInitPositionX, int setInitPositionY, string title) {
  width  = setWidth;               
  height = setHeight;
  
  initPositionX = setInitPositionX;
  initPositionY = setInitPositionY;
  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(initPositionX, initPositionY);
  glViewport(0, 0, width, height); 
  
  glutMaster->CallGlutCreateWindow(title.c_str(), this);
  
  glEnable(GL_DEPTH_TEST);
  
  glMatrixMode(GL_PROJECTION);
  glOrtho(-80.0, 80.0, -80.0, 80.0, -500.0, 500.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glRotatef(60, 1, 1, 1);
  glColor4f(1.0, 0.0, 0.0, 1.0);
}

DemoWindow::~DemoWindow(){

   glutDestroyWindow(windowID);
}

void DemoWindow::CallBackDisplayFunc(void){

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glColor4f(1.0, 0.0, 0.0, 1.0);
   glutWireSphere(50, 10, 10);

   glutSwapBuffers();
}

void DemoWindow::CallBackReshapeFunc(int w, int h){

   width = w;
   height= h;

   glViewport(0, 0, width, height); 
   CallBackDisplayFunc();
}

void DemoWindow::CallBackIdleFunc(void){

   glRotatef(0.25, 1, 1, 2);
   CallBackDisplayFunc();
}

void DemoWindow::StartSpinning(GlutMaster * glutMaster){

   glutMaster->SetIdleToCurrentWindow();
   glutMaster->EnableIdleFunction();
}
   







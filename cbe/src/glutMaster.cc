// glutMaster.cc - source file for the CBE project
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
#include "glutMaster.h"
#include "glutWindow.h"
                                                       
GlutWindow* viewPorts[MAX_NUMBER_OF_WINDOWS]; 

int GlutMaster::currentIdleWindow   = 0;
int GlutMaster::idleFunctionEnabled = 0;


GlutMaster::GlutMaster(int* argc, char** argv) {
  glutInit(argc, argv);
}

GlutMaster::~GlutMaster(){
}

void GlutMaster::CallBackDisplayFunc(void){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackDisplayFunc();
}

void GlutMaster::CallBackIdleFunc(void){
  if(idleFunctionEnabled && currentIdleWindow){
    glutSetWindow(currentIdleWindow);
    viewPorts[currentIdleWindow]->CallBackIdleFunc();
  }
}
 
void GlutMaster::CallBackKeyboardFunc(unsigned char key, int x, int y) {
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackKeyboardFunc(key, x, y);
}

void GlutMaster::CallBackKeyboardUpFunc(unsigned char key, int x, int y) {
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackKeyboardFunc(key, x, y);
}

void GlutMaster::CallBackMotionFunc(int x, int y){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackMotionFunc(x, y);
}

void GlutMaster::CallBackMouseFunc(int button, int state, int x, int y){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackMouseFunc(button, state, x, y);
}

void GlutMaster::CallBackPassiveMotionFunc(int x, int y){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackPassiveMotionFunc(x, y);
}

void GlutMaster::CallBackReshapeFunc(int w, int h){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackReshapeFunc(w, h);
}

void GlutMaster::CallBackSpecialKeyboardFunc(int key, int x, int y){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackSpecialKeyboardFunc(key, x, y);
}   

void GlutMaster::CallBackVisibilityFunc(int visible){
  int windowID = glutGetWindow();
  viewPorts[windowID]->CallBackVisibilityFunc(visible);
}

void GlutMaster::CallGlutCreateWindow(const char* setTitle, GlutWindow * glutWindow){
  // Open new window, record its windowID
  int windowID = glutCreateWindow(setTitle);
  glutWindow->SetWindowID(windowID);
  
  // Store the address of new window in global array 
  // so GlutMaster can send events to propoer callback functions.
  viewPorts[windowID] = glutWindow;
  
  // Hand address of universal static callback functions to Glut.
  // This must be for each new window, even though the address are constant.
  glutDisplayFunc(CallBackDisplayFunc);
  glutIdleFunc(CallBackIdleFunc); 
  glutKeyboardFunc(CallBackKeyboardFunc);
  glutSpecialFunc(CallBackSpecialKeyboardFunc);
  glutMouseFunc(CallBackMouseFunc);
  glutMotionFunc(CallBackMotionFunc);
  glutPassiveMotionFunc(CallBackPassiveMotionFunc);
  glutReshapeFunc(CallBackReshapeFunc); 
  glutVisibilityFunc(CallBackVisibilityFunc);
}

void GlutMaster::CallGlutMainLoop(void) {
  glutMainLoop();
}

void GlutMaster::DisableIdleFunction(void){
  idleFunctionEnabled = 0;
}

void GlutMaster::EnableIdleFunction(void){
  idleFunctionEnabled = 1;
}

int GlutMaster::IdleFunctionEnabled(void){
  // Is idle function enabled?
  return(idleFunctionEnabled);
}

int GlutMaster::IdleSetToCurrentWindow(void){
  // Is current idle window same as current window?
  return( currentIdleWindow == glutGetWindow() );
}

void GlutMaster::SetIdleToCurrentWindow(void){
  currentIdleWindow = glutGetWindow();
}

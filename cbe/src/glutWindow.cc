// glutWindow.cc - source file for the CBE project
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

#include "glutWindow.h"

GlutWindow::GlutWindow(void){
}

GlutWindow::~GlutWindow(){
}

void GlutWindow::CallBackDisplayFunc(void){
}

void GlutWindow::CallBackIdleFunc(void){
}

void GlutWindow::CallBackKeyboardFunc(unsigned char key, int x, int y){
}

void GlutWindow::CallBackMotionFunc(int x, int y){
}

void GlutWindow::CallBackMouseFunc(int button, int state, int x, int y){
}

void GlutWindow::CallBackPassiveMotionFunc(int x, int y){
}

void  GlutWindow::CallBackReshapeFunc(int w, int h){
}
   
void GlutWindow::CallBackSpecialKeyboardFunc(int key, int x, int y){
}   

void GlutWindow::CallBackVisibilityFunc(int visible){
}

void GlutWindow::SetWindowID(int newWindowID){
   windowID = newWindowID;
}

int GlutWindow::GetWindowID(void){
   return( windowID );
}

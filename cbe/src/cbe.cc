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

extern "C" {
#ifdef _WIN32
  #include <GL/glaux.h>
#endif
#include <GL/gl.h>    // OpenGL
#include <GL/glut.h>  // GLUT
}

#include <iostream>

#include "common.h"
#include "config.h"
#include "cbe.hh"
#include "glutMaster.h"
#include "glutWindow.h"
#include "mainAppWindow.h"
#include "street.h"
#include "plane.h"

using namespace std;

GlutMaster* glutMaster;
mainAppWindow* secondWindow = 0;


int main(int argc, char *argv[]) {
  Plane *p;
  Street *s;

  cout << "Entering main in cbe.cc" << endl;
  
  try {
    glutMaster   = new GlutMaster(&argc, argv);  
    cout << "created GlutMaster in cbe.cc" << endl;

    secondWindow = new mainAppWindow(glutMaster,
				     500, 500,         // height, width
				     200, 400,         // initPosition (x,y)
#ifndef _WIN32
					 (string)PACKAGE + (string)" " + (string)VERSION);   // title
#else // Für Win32
// In the Windows version PACKAGE is not defined.
					 "Second window" ); // title
#endif
    cout << "created mainAppWindow secondWindow in cbe.cc" << endl;
    s=new Street(-50,0,0,2);
  }
  catch (...) {
    return -1;
  }
  
  GLuint streetList=s->getStreet();
  secondWindow->setStreet(&streetList);
  Point a,b;

  a.x=-60; a.y=-0.1; a.z=-60;
  b.x=60; b.y=-0.1; b.z=60;
  try {
    p=new Plane(a,b);
  }
  catch (...) {
    return -1;
  }
  
  GLuint Planelist=p->getPlane();
  secondWindow->setPlane(&Planelist);
  
  secondWindow->StartSpinning(glutMaster);        // enable idle function
  cout << "Started Spinning in secondWindow in cbe.cc" << endl;
  glutMaster->CallGlutMainLoop();
  cout << "CallGlutMainLoop() success" << endl;

  // Clean up street and plane
  delete s;
  delete p;

  return 0;
}

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

#include "cbe.hh"
#include "glutMaster.h"
#include "glutWindow.h"
#include "mainAppWindow.h"
#include "street.h"
#include "plane.h"
#include "Point.h"
#include "config.h"

using namespace std;

int main(int argc, char *argv[]) {
  GlutMaster* glutMaster;
  mainAppWindow* secondWindow;
  Plane *plane;
  Street *street;
  
  // Size of plane
  Point a(-60.0, -0.01, -60.0);
  Point b(160.0, -0.01, 60.0);
  
  cout << endl;
  cout << "Keyboard functions:" << endl;
  cout << "  u: Increase speed" << endl;
  cout << "  d: Decrease speed" << endl;
  cout << "  f: Toggles fog (quite useless until now, find out for yourself)" << endl;
  cout << "  q / ESC: Exits program" << endl << endl;
  
  try {
    glutMaster   = new GlutMaster(&argc, argv);  

#ifndef _WIN32
    secondWindow = new mainAppWindow(glutMaster,
				     500, 500,         // height, width
				     200, 400,         // initPosition (x,y)
				     (string)PACKAGE + (string)" " + (string)VERSION);   // title
#else
    secondWindow = new mainAppWindow(glutMaster,
				     500, 500,          // height, width
				     200, 400,          // initPosition (x,y)
				     "Second window" ); // title
#endif

    // Create street and plane
    street = new Street(-50, 0, 0, 4);
    plane = new Plane(a, b);
  }
  catch (...) {
    return -1;
  }
  
  // Init scenery
  secondWindow->setStreet(street);
  secondWindow->setPlane(plane);
  
  secondWindow->StartSpinning(glutMaster);        // enable idle function
  glutMaster->CallGlutMainLoop();
  
  // Clean up street and plane
  delete street;
  delete plane;

  return 0;
}

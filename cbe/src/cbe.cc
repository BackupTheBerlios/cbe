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
#ifdef _WIN32
#include <GL/glaux.h>
#endif
#include <stdlib.h>
#include <GL/gl.h>    // OpenGL
#include <GL/glut.h>  // GLUT
}
#include "cbe.hh"
#include "glutMaster.h"
#include "glutWindow.h"
#include "mainAppWindow.h"
#include "street.h"
#include "plane.h"
#include "Car.h"
#include "Point.h"
#include "config.h"

using namespace std;

// Global functions
void cleanUp(void);

// Global objects
GlutMaster* glutMaster;
mainApp::mainAppWindow* driversWindow;
Plane* plane;
Street* street;
Car* car1;
Car* car2;

// Main program
int main(int argc, char *argv[]) {
  // Set the size of the plane
  Point x(-60.0, -0.01, -60.0);
  Point y(160.0, -0.01, 60.0);
  
  cout << endl << "CBE keyboard functions:" << endl;
  cout <<         "  u          Increase speed" << endl;
  cout <<         "  n          Decrease speed" << endl;
  cout <<         "  h          Turn left" << endl;
  cout <<         "  j          Turn right" << endl;
  cout <<         "  d          Hide moving objects" << endl;
  cout <<         "  s          Show moving objects" << endl;
  cout <<         "  f          Toggles fog (currently pretty useless)" << endl;
  cout <<         "  q / ESC    Exits program" << endl << endl;
  
  try {
    glutMaster = new GlutMaster(&argc, argv);  

#ifndef _WIN32
    driversWindow = new mainApp::mainAppWindow(glutMaster,
					       500, 500,                                           // height, width
					       200, 100,                                           // initPosition (x,y)
					       (string)PACKAGE + (string)" " + (string)VERSION);   // title
#else
    driversWindow = new mainApp::mainAppWindow(glutMaster,
					       500, 500,            // height, width
					       200, 400,            // initPosition (x,y)
					       "Driver's Window" ); // title
#endif
    
    // Create street and plane
    street = new Street(-50, 0, 0, 4);
    plane = new Plane(x, y);
	car1 = new TestCar( 0, 30 );
	car1->setPos( -20, 2, 10 );
	car2 = new TestCar( -1, 0 );
	car2->setPos( -3, 0, -5 );
	car2->rotate( -90 );
  }
  catch (...) {
    cout << "Exception was thrown on program initialization. Not enough memory?!" << endl;
    return -1;
  }
  
  // Init scenery
  driversWindow->setStreet(street);
  driversWindow->setPlane(plane);
  driversWindow->addGraphicObject( car1 );
  driversWindow->addGraphicObject( car2 );
  atexit(cleanUp);

  try {
    // Enable event loops
    driversWindow->StartSpinning(glutMaster);
    glutMaster->CallGlutMainLoop();
  }
  catch (mainApp::ExitKeyPressed) {
    cout << "Exit key was pressed. CBE now cleans up and waves good-bye." << endl;
  }
  catch (...) {
    cout << "Exception was thrown during the event loop. CBE aborted." << endl;
    return -1;
  }

  return 0;
}


// Clean up memory
void cleanUp() {
  delete plane;
  delete street;
  delete car1;
  delete car2;
  delete driversWindow;
  delete glutMaster;
}

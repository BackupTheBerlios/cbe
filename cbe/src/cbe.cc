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
#include <stdlib.h>
#include <GL/gl.h>    // OpenGL
#include <GL/glut.h>  // GLUT
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>                                                         
#include <sys/types.h>
}

#include <iostream>
#include "Preferences.h"
#include "cbe.hh"
#include "glutMaster.h"
#include "glutWindow.h"
#include "mainAppWindow.h"
#include "street.h"
#include "plane.h"
#include "Car.h"
#include "Point.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_GETOPT_H
extern "C" {
#include <getopt.h>
}
#else
extern "C" {
#include "getopt.h"
}
#endif


using namespace std;

// Global functions
void cleanUp(void);

// Global objects
GlutMaster* glutMaster;
mainApp::mainAppWindow* driversWindow;
Plane* plane;
Street* street;
Car* car1; Car* car2;

// Main program
int main(int argc, char *argv[]) {
  string prefsFile = "";      // Path to the cbe preferences file
  int c = 0, option_index = 0;
  extern char *optarg;
  static struct option long_options[] = {
    {"help", 0, NULL, VALUE_HELP},
    {"rcfile", 1, NULL, VALUE_RCFILE},
    {"version", 0, NULL, VALUE_VERSION},
    {0, 0, 0, 0}
  };
  
  // Set the size of the plane
  Point x(-60.0, -0.01, -60.0);
  Point y(160.0, -0.01, 60.0);
  
  // Scan command line parameters and options
  while ( (c = getopt_long(argc, argv, "hr:V", long_options, &option_index)) != -1 ) {
    switch (c) {
    case 'h':
    case VALUE_HELP:
      cout << "CBE is the Change Blindness Experiment/Environment." << endl;
      cout << endl;
      cout << "Usage: " << PACKAGE << " [OPTION]..." << endl;
      cout << endl;
      cout << "If a long option shows an argument as mandatory, then it is mandatory" << endl;
      cout << "for the equivalent short option also." << endl;
      cout << endl;
      cout << "Options:" << endl;
      cout << "  -h, --help          Display this help information" << endl;
      cout << "  -r, --rcfile=FILE   Specify rcfile location" << endl;
      cout << "  -V, --version       Display version information" << endl;
      cout << endl;
      cout << "Keyboard map:" << endl;
      cout << "  u  Increase speed     d  Hide moveable objects" << endl;
      cout << "  n  Decrease speed     s  Show moveable objects" << endl;
      cout << "  h  Turn left          1  Change car color randomly" << endl;
      cout << "  j  Turn right         2  Toggle brake lights randomly" << endl;
      cout << "  f  Toggle fog         3  Hide/unhide car randomly" << endl;
      cout << "  b  Toggle blending    e  Toggle eye-position-server communication" << endl;
      cout << "  q  (Escape) Exit" << endl;
      cout << endl;
      cout << "Report bugs to <cbe-development@lists.berlios.de>" << endl;
      return 0;
      break;
    case 'V':
    case VALUE_VERSION:
      cout << PACKAGE << " " << VERSION << endl;
      cout << endl;
      cout << "Copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen, Germany" << endl;
      cout << endl;
      cout << "This is free software; see the source for copying conditions.  There is NO" << endl;
      cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
      return 0;
      break;
    case 'r':
    case VALUE_RCFILE:
      prefsFile = optarg;
      break;
    default:
      // Unrecognized option
      cerr << "Try '" << argv[0] << " --help' for more information." << endl;
      return -1;
    }
  }
  
  // Determine path for rcfile
  if (!prefsFile.length()) {
    if (getenv("HOME"))
      prefsFile = (string)getenv("HOME") + (string)"/.cbe";
    else
      prefsFile = (string)"/home/" + (string)getpwuid(getuid())->pw_name + (string)"/.cbe";
  }

  try {
    // Try reading the cbe preferences file
    pref::Preferences prefs(prefsFile);
    
    // Try initializing the main Glut objects and window
    glutMaster = new GlutMaster(&argc, argv);  
    driversWindow = new mainApp::mainAppWindow(glutMaster,
					       &prefs,
					       WINDOW_WIDTH, WINDOW_HEIGHT,                        // height, width
					       10, 10,                                             // initPosition (x,y)
					       (string)PACKAGE + (string)" " + (string)VERSION);   // title

    // Create street, plane and car
    street = new Street( -50, 0, 0, 4 );
    plane = new Plane( x, y );
    car1 = new TestCar( 0, 30 );
    car1->setPos( -20, 2, 10 );
    car1->rotate( -90 );
    car2 = new TestCar( -1, 0 );
    car2->setPos( -3, 0, -5 );
    car2->rotate( -90 );
  }
  catch(pref::IOException) {
    cerr << "ERROR: Could not read cbe preferences file." << endl;
    return -1;
  }
  catch(...) {
    cerr << "ERROR: Exception was thrown on program initialization. Not enough memory maybe?!" << endl;
    return -1;
  }
  
  // Init scenery
  driversWindow->setStreet( street );
  driversWindow->setPlane( plane );
  driversWindow->addCar( car1 );
  driversWindow->addCar( car2 );
  atexit(cleanUp);

  try {
    // Enable event loops
    driversWindow->StartSpinning(glutMaster);
    glutMaster->CallGlutMainLoop();
  }
  catch(mainApp::ExitKeyPressed) {
    cout << "Exit key was pressed. CBE now cleans up and waves good-bye." << endl;  // Can this ever happen?
  }
  catch(...) {
    cerr << "Exception was thrown during the event loop. CBE aborted." << endl;     // Can this ever happen? Suppose not, mainLoop just crashes instead..
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

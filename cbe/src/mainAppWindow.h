// mainAppWindow.h - source file for the CBE project
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

#ifndef __MAINAPPWINDOW_WINDOW_H__
#define __MAINAPPWINDOW_WINDOW_H__

#define WINDOW_HEIGHT 538
#define WINDOW_WIDTH 717

#define COCKPIT_HEIGHT 161
#define COCKPIT_WIDTH 717

extern "C" {
#include <GL/gl.h>
#include <time.h>
}
#include <string>
#include <vector>
#include "random.h"
#include "Preferences.h"
#include "glutMaster.h"
#include "GObject.h"
#include "Car.h"
#include "Point.h"
#include "Bitmap.h"
#include "JoystickDriver.hh"
#include "SerialClient.hh"

using namespace std;

namespace mainApp {
  typedef vector<GObject*> GObjectVector;
  typedef vector<Car*> CarVector;

  class mainAppWindow : public GlutWindow {
  private:
    pref::Preferences* prefs;       // Preferences object
    GObject *plane;                 // holds the plane
    GObject *curStreet;                // holds the street
    GObjectVector graphicObjects;	// Objects to be drawn
    CarVector carVector;			// Cars
    GLfloat speed;                  // Speed of movement
    bool isFog;                     // Flag for fog
    Point *movementVector;          // Movement direction
    GLfloat viewingAngle;           // Indicates the angle of view in the x-z-plane
    clock_t oldTime;                // Used to determine the Frames/s (also for constant movement speed)
    double getTimePassed();         // Reports seconds since last call
    Bitmap cockpitIMG;              // Cockpit image
    JoystickDriver *joystick;       // Joystick-Object
    SerialClient *serialclient;     // Serial-Object
    bool isSerial;                  // Flag for use of serialport
    bool fullscreen;                // Flag to determine screen mode
    int frameCount;                 // Used to compute frames/sec
    GLfloat latenz;                 // stores frames/sec for computig correct movement-speed
    bool showFramerate;             // Used to decide wether framerate is shown or not
	long zeroClock;

  public:
    int height, width;
    int initPositionX, initPositionY;
    
    mainAppWindow(GlutMaster*, pref::Preferences*, int, int, int, int, string);
    ~mainAppWindow();
    
    void HandleEyeTrackerEvent(int event);

    // Some call back functions
    void CallBackDisplayFunc(void);
    void CallBackReshapeFunc(int, int);   
    void CallBackIdleFunc(void);
    void CallBackKeyboardFunc(unsigned char, int, int);
    void CallBackSpecialKeyboardFunc(int, int, int);
    
    // Window functionality functions
    void StartSpinning(GlutMaster*); // ???
    void setstreet(GObject*);        // used to alter the polygonList
    void setPlane(GObject*);         // ???
    void addGraphicObject(GObject*); // ???
    void addCar(Car*);               // ???
    Car* pickCar();
  };
  
  // Exceptions
  class ExitKeyPressed { };
  
}

#endif

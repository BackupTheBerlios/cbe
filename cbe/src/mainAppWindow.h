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

extern "C" {
#include <GL/gl.h>
#include <time.h>
}
#include <string>
#include <list>
#ifdef _WIN32
using namespace std;
#endif
#include "glutMaster.h"
#include "GObject.h"
#include "Point.h"
#include "Bitmap.h"
#include "JoystickDriver.hh"
#include "SerialClient.hh"

namespace mainApp {
  typedef list<GObject*> GObjectList;
  
  class mainAppWindow : public GlutWindow{
  private:
    GObject *plane;                 // holds the plane
    GObject *street;                // holds the street
    GObjectList graphicObjectsList; // Objects to be drawn
    GLfloat speed;                  // Speed of movement
    bool isFog;                     // Flag for fog
    int blend;                      // Flag for blending
    Point *movementVector;          // Movement direction
    GLfloat viewingAngle;           // Indicates the angle of view in the x-z-plane
    clock_t oldTime;                // Used to determine the Frames/s (also for constant movement speed)
    double getTimePassed();         // Reports seconds since last call
    Bitmap cockpitIMG;              // Cockpit image
    JoystickDriver *joystick;       // Joystick-Object
    SerialClient *serialclient;     // Serial-Object
    bool isSerial;                  // Flag for use of serialport

  public:
    int height, width;
    int initPositionX, initPositionY;
    
    mainAppWindow(GlutMaster* glutMaster,
		  int setWidth, int setHeight,
		  int setInitPositionX, int setInitPositionY,
		  string title);
    ~mainAppWindow();
    
    // Some call back functions
    void CallBackDisplayFunc(void);
    void CallBackReshapeFunc(int, int);   
    void CallBackIdleFunc(void);
    void CallBackKeyboardFunc(unsigned char, int, int);
    void CallBackSpecialKeyboardFunc(unsigned char, int, int);
    
    // Window functionality functions
    void StartSpinning(GlutMaster*); // ???
    void setStreet(GObject*);        // used to alter the polygonList
    void setPlane(GObject*);         // ???
    void addGraphicObject( GObject* obj);
  };
  
  // Exceptions
  class ExitKeyPressed { };
  
}

#endif

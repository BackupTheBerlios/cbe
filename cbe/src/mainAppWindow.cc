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
#include <cmath>
extern "C" {
#ifdef _WIN32
#include <GL/glaux.h>
#endif
#include <GL/glut.h>
#include <time.h>
}
#include "cbe.hh"
#include "mainAppWindow.h"
#include "glutMaster.h"
#include "glutWindow.h"
#include "Point.h"
#include "Bitmap.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

using namespace std;

namespace mainApp {

  mainAppWindow::mainAppWindow(GlutMaster* glutMaster, int setWidth, int setHeight, int setInitPositionX, int setInitPositionY, string title) {
    // Where is the cockpit?
    cockpitIMG.setPath( (string)DATADIR + "/pixmaps/cbe/cockpit.bmp" );  // Size: 717 x 538 pixels
    
    // Load the cockpit
    if (cockpitIMG.load())
      cout << "Cockpit image loaded." << endl;
    else
      cout << "ERROR: Cockpit image not loaded. Did you forget 'make install'?" << endl;
    
    // Set default viewing and movement vectors
    movementVector = new Point(1,0,0);
    
    // Set default speed and viewingAngle
    speed = 0.0;
    viewingAngle=0.0;

    // Set current clock-value to oldTime
    oldTime=clock();

    // Set some defaults
    isFog = false;
    blend = 1;

    width  = setWidth;               
    height = setHeight;
    initPositionX = setInitPositionX;
    initPositionY = setInitPositionY;
  
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(initPositionX, initPositionY);

    // Set viewport to 0, cockpit height, cockpit width, driver's window height
    glViewport(0, 161, (GLint)width, 377);

    glutMaster->CallGlutCreateWindow(title.c_str(), this);
  
    glEnable(GL_DEPTH_TEST);  
    
    // Switch to camera matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // perspective
    gluPerspective(120.0, 1.0, 0.1, 100.0);
  
    // Set viewing direction from z-Axis to x-axis
    glRotatef(90.0, 0, -1, 0);
  
    // point of view
    glTranslatef(0.0, -1.5, 0.0);

    // set background color
    glClearColor(0.3, 0.3, 1,0);

    // Save ProjectionMatrix (for futher use in Idle-Function)
    glPushMatrix();

    // Switch to scenery matrix
    glMatrixMode(GL_MODELVIEW);

    // Initialize Joystick
    joystick = new JoystickDriver();

    // Initialize SerialPort
    serialclient = new SerialClient("/dev/ttyS1"); // Open com2
    isSerial=false; // Disable serialport by default
  }


  mainAppWindow::~mainAppWindow() {
    delete movementVector;
    delete joystick;
    delete serialclient;
    glutDestroyWindow(windowID);
    cout << "Destructor of mainAppWindow called." << endl;
  }


  void mainAppWindow::CallBackDisplayFunc(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    plane->draw();
    street->draw();
    
    // Draw all graphic objects in the list
    for( GObjectList::iterator itr = graphicObjectsList.begin(); itr != graphicObjectsList.end(); itr++ )
      (*itr)->draw();
    
    // Blend cockpit
    glBlendColor(1.0f, 1.0f, 1.0f, 0.0);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    glBlendFunc(GL_SRC_COLOR,GL_SRC_COLOR);
    
    // Draw cockpit
    glDrawPixels(717, 538, GL_RGB, GL_UNSIGNED_BYTE, cockpitIMG.getData());
    
    // Finally draw everything on the screen that we just created and constructed
    glutSwapBuffers();   
  }


  // Call back function for reshape state
  void mainAppWindow::CallBackReshapeFunc(int w, int h) {
    int cockpitHeight = 377;

    width  = w;                   // Full window width
    height = h - cockpitHeight;   // Window height - cockpit height

    // 0, cockpit height, cockpit width, driver's window height
    glViewport(0, height, width, cockpitHeight); 
    CallBackDisplayFunc();
  }


  // Call back function for idle state
  void mainAppWindow::CallBackIdleFunc(void) {
    GLfloat latenz = getTimePassed(); // save passed time

    // Make blink-detection (only if serialport is activated)
    if (isSerial) {
      serialclient->requestNewData();
      if (serialclient->isBlink())
	cout << "Blink" << endl;
    }
    
    // Make Joysick-Calls
    joystick->refreshJoystick();
    viewingAngle+= joystick->getXaxis() * 200 * latenz;
    
    // Keep viewingAngle in -180<x<180
    if (viewingAngle<-180)
      viewingAngle+=360;
    if (viewingAngle>=180)
      viewingAngle-=360;
  
    speed -= joystick->getYaxis() * 200 * latenz;
  
    // recalculate the new movementVector
    movementVector->x=cos(viewingAngle*M_PI/180);
    movementVector->z=sin(viewingAngle*M_PI/180);

    // Switch to camera matrix
    glMatrixMode(GL_PROJECTION);

    // Get projection Matrix from Stack, but leave a copy there
    glPopMatrix();
    glPushMatrix();

    // Now we can simply turn the camera in the direction we want without considering the next frame
    glRotatef(viewingAngle,0,1,0);

    // Switch back to world matrix
    glMatrixMode(GL_MODELVIEW);
  
    // Now move the world according to our viewing direction
    glTranslatef(movementVector->x * latenz * speed, 
		 movementVector->y * latenz * speed, 
		 movementVector->z * latenz * speed);

    // Finished with "camera-actions" advance to draw the world
    CallBackDisplayFunc();
  }


  // Call back function for keyboard events
  void mainAppWindow::CallBackKeyboardFunc(unsigned char key, int x, int y) {
    GObjectList::iterator itr;
    // Determine key
    switch (key) {
    case 'Q':
    case 'q':
    case 27:
      exit(0);
      // throw ExitKeyPressed();
      break;
    case 'B':
    case 'b':
      blend = blend ? 0 : 1;
      if (blend) {
	glDisable(GL_BLEND);
      }
      else {
	glEnable(GL_BLEND);
      }
      break;
    case 'U':  // faster
    case 'u':
      if ( speed >= 0 )  // Acceleration in forward movement
	speed += 0.05;
      else { // Braking
	speed += 0.2; // brake
	if (speed > 0 ) // started reverse movement while braking
	  speed = 0; // brake only untill stopping
      }
      break;
    case 'N': // slower
    case 'n':
      if ( speed <= 0 )  // Acceleration in backward movement
	speed -= 0.05;
      else { // Braking
	speed -= 0.2; // brake
	
	if (speed < 0 ) // started reverse movement while braking
	  speed = 0; // brake only untill stopping
      }
      break;
    case 'H': // left
    case 'h':
      viewingAngle--;

      if (viewingAngle>180)
	viewingAngle-=360;

      break;
    case 'J': // right
    case 'j':
      viewingAngle++;

      if (viewingAngle<-180)
	viewingAngle+=360;
      break;
    case 'F':
    case 'f':
      if (!isFog) {
	glEnable(GL_FOG);
	cout << "Fog enabled." << endl;
	isFog = true;
      }
      else {
	glDisable(GL_FOG);
	cout << "Fog disabled." << endl;
	isFog = false;
      }
      break;
	case 'd':
    case 'D':
      for( itr = graphicObjectsList.begin(); itr != graphicObjectsList.end(); itr++ )
	(*itr)->hide();
      break;
    case 's':
    case 'S':
      for( itr = graphicObjectsList.begin(); itr != graphicObjectsList.end(); itr++ )
	(*itr)->unhide();
      break;
    case 'e':
    case 'E':
      if (isSerial)
	isSerial=false;
      else
	isSerial=true;
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


  void mainAppWindow::StartSpinning(GlutMaster* glutMaster){
    glutMaster->SetIdleToCurrentWindow();
    glutMaster->EnableIdleFunction();
  }

  void mainAppWindow::setStreet(GObject* s) {
    street = s;
  }

  void mainAppWindow::setPlane(GObject* p) {
    plane = p;
  }
  void mainAppWindow::addGraphicObject( GObject* obj) {
	  // Append the passed graphic object pointer to the list
	  graphicObjectsList.push_back( obj );
  }
  // Calculates time passed since last call of this function
  double mainAppWindow::getTimePassed() {
    clock_t tnew,ris;
    tnew=clock();
    ris=tnew-oldTime;
    oldTime=tnew;
    return(ris/(double)CLOCKS_PER_SEC);
  }

}

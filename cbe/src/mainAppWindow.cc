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
#include <GL/glut.h>
#include <time.h>
}
#include "Preferences.h"
#include "cbe.hh"
#include "mainAppWindow.h"
#include "glutMaster.h"
#include "glutWindow.h"
#include "Point.h"
#include "Bitmap.h"
#include "street.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#ifndef BRAKE_FACTOR
#define BRAKE_FACTOR 3
#endif

GLfloat gLocation[ 3 ];

using namespace std;

namespace mainApp {

  mainAppWindow::mainAppWindow(GlutMaster* glutMaster,
			       pref::Preferences* newPrefs,
			       int setWidth, int setHeight,
			       int setInitPositionX, int setInitPositionY,
			       string title) {
    // Set preferences
    prefs = newPrefs;
    
    // Where is the cockpit?
    cockpitIMG.setPath( (string)DATADIR + "/pixmaps/cbe/cockpit.tif" );       // Size: 717 x 538 pixels
    
    // Load the cockpit
    if (cockpitIMG.load()) {
#ifdef DEBUG
      cout << "Cockpit image loaded." << endl;
#endif
    }
    else
      cerr << "ERROR: Cockpit image not loaded. Did you forget 'make install'?" << endl;
    
    try {
      movementVector = new Point(1,0,0);         // Set default viewing and movement vectors
    }
    catch(...) {
      throw;
    }

    // Set default speed and viewingAngle
    speed = 0.0;
    viewingAngle = 0.0;
    
    // Set current clock-value to oldTime
    oldTime = clock();
    zeroClock = clock();

    // Set some defaults concerning framerate
    frameCount = 0;
    latenz = 1/prefs->getFramerate();
    showFramerate=false;

    fullscreen = false;
    isFog = false;
    width = setWidth;               
    height = setHeight;
    initPositionX = setInitPositionX;
    initPositionY = setInitPositionY;
  
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(initPositionX, initPositionY);

    // Set viewport to 0, cockpit height, cockpit width, driver's window height
    glViewport((GLint)0, (GLint)COCKPIT_HEIGHT, (GLint)width, (GLint)height);

    glutMaster->CallGlutCreateWindow(title.c_str(), this);

    glEnable(GL_DEPTH_TEST);  
        
    gLocation[ 0 ] = 0;
    gLocation[ 1 ] = 0;
    gLocation[ 2 ] = 0;
    
    // Switch to camera matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // perspective
    gluPerspective(45/*120.0*/, 1.0, 0.1, 90.0);
    
  	//glEnable(GL_FOG);
  	

    // Set viewing direction from z-Axis to x-axis
    glRotatef(90.0, 0, -1, 0);
  
    // point of view
    glTranslatef(0.0, -1.5, 0.0);

    // set background color
    //glClearColor(0.3, 0.3, 1,0);
	glClearColor( 0.23, 0.35, 0.78, 1.0 );

    // Save ProjectionMatrix (for futher use in Idle-Function)
    glPushMatrix();

    // Switch to scenery matrix
    glMatrixMode(GL_MODELVIEW);

    if (!prefs->fullscreen())
      fullscreen = false;
    else {
      glutFullScreen();
      fullscreen = true;
    }

    try {
      joystick = new JoystickDriver(prefs->getJoystick().c_str());           // Initialize Joystick
      serialclient = new SerialClient(prefs->getSerial().c_str());           // Initialize Serial port
      isSerial = false;                                                      // Disable serialport by default
    }
    catch (...) {
      throw;
    }
    
  	// clipping
  	/*GLdouble plane[ 4 ];
  	plane[ 0 ] = 0;
  	plane[ 1 ] = 0;
  	plane[ 2 ] = 1;
  	plane[ 3 ] = 10;
  	glClipPlane( 4, plane );
  	plane[ 3 ] = 20;
  	glClipPlane( 5, plane );
  	glEnable( GL_CLIP_PLANE4 );
  	glEnable( GL_CLIP_PLANE5 );*/

  }
  

  mainAppWindow::~mainAppWindow() {
    delete movementVector;
    delete joystick;
    delete serialclient;
  
    glutDestroyWindow(windowID);
#ifdef DEBUG
    cout << "Destructor of mainAppWindow called." << endl;
#endif
  }


  void mainAppWindow::CallBackDisplayFunc(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    plane->draw();
    curStreet->draw();
    
    // Draw all graphic objects in the list
    GObjectVector::iterator listEnd = graphicObjects.end();
    
    for( GObjectVector::iterator itr = graphicObjects.begin(); itr != listEnd; ++itr )
      (*itr)->draw();
    
    // Check if blending is enabled
    if (prefs->useBlending()) {	
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDrawPixels(717, 538, GL_RGBA, GL_UNSIGNED_BYTE, cockpitIMG.getData());
      glEnable(GL_BLEND);
    }

    // Finally draw everything on the screen that we just created and constructed
    glutSwapBuffers();   
  }


  // Call back function for reshape state
  void mainAppWindow::CallBackReshapeFunc(int w, int h) {
    // 0, cockpit height, cockpit width, driver's window height
    glViewport((GLint)0, (GLint)COCKPIT_HEIGHT, (GLint)w, (GLint)(h - COCKPIT_HEIGHT));
    CallBackDisplayFunc();
  }


  // Call back function for idle state
  void mainAppWindow::CallBackIdleFunc(void) {
    frameCount++;
    
    if (frameCount==10) {                   // recalculate framerate every 10 frames and set latenz according      
      latenz = getTimePassed()/10;          // save passed time
      frameCount=0;

      if (showFramerate) 
	cout << 1/latenz << endl;
    }

    // Make blink-detection (only if serialport is activated)
    if (isSerial) {
      serialclient->requestData();
      
      switch ( serialclient->getChange() ) {
      case CHANGE_HIDE_CAR: 
	for(GObjectVector::iterator itr = graphicObjects.begin(); itr != graphicObjects.end(); itr++ )
	  (*itr)->toggleVisibility();
	
	break;
      case CHANGE_TOGGLE_BREAKLIGHTS:
	for(GObjectVector::iterator itr = graphicObjects.begin(); itr != graphicObjects.end(); itr++ )
	  (*itr)->change( Car::change_toggleBrakeLight );
	
	break;
      default:
	break;
      }
    }
    
    // Make Joysick-Calls
    joystick->refreshJoystick();
    viewingAngle+= joystick->getXaxis() * 20 * latenz; // Joystick turning speed
    
    // Keep viewingAngle in -180<x<180
    if (viewingAngle<-180)
      viewingAngle+=360;
    
    if (viewingAngle>=180)
      viewingAngle-=360;
    
    GLfloat speedDiff = -joystick->getYaxis() * 10 * latenz; //Joystick acceleration speed

    if ( (( speed >= 0 ) && ( speedDiff >= 0 )) || (( speed <= 0 ) && ( speedDiff <= 0 )) )
      speed += speedDiff;                // Normal acceleration
    // Otherwise we are braking
    else if ( speed > 0 ) {              // and speedDiff < 0, braking from forward movement
      speed += speedDiff * BRAKE_FACTOR;
      
      if ( speed < 0 )
	speed = 0;                       // Brake not more than until stopping.
    }
    else {                               // ( speed < 0 and speedDiff > 0, braking from backward movement
      speed += speedDiff * BRAKE_FACTOR;
      
      if ( speed > 0 )
	speed = 0;                       // Brake not more than until stopping.
    }
    
    // recalculate the new movementVector
    movementVector->x=cos(viewingAngle*M_PI/180);
    movementVector->y=0;
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
		 
	gLocation[ 0 ] += movementVector->x * latenz * speed;
	gLocation[ 1 ] += movementVector->y * latenz * speed;
	gLocation[ 2 ] += movementVector->z * latenz * speed;
		     
    // advance cars
    double time = ( clock() - zeroClock ) / (double)CLOCKS_PER_SEC;
    for( CarVector::iterator itr = carVector.begin(); itr != carVector.end(); ++itr )
    {
    	(*itr)->move( (street*)curStreet, time );
	}

    
    // Finished with "camera-actions" advance to draw the world
    CallBackDisplayFunc();
  }


  // Call back function for keyboard events
  void mainAppWindow::CallBackKeyboardFunc(unsigned char key, int x, int y) {
    GObjectVector::iterator itr;
    GObjectVector::iterator listEnd;
    Car* car;
    
    // Determine key
    switch (key) {
    case 'Q':
    case 'q':
    case 27:
      exit(0);                   // or throw ExitKeyPressed();
      break;
    case 'B':
    case 'b':
      if (prefs->useBlending()) {
	glDisable(GL_BLEND);
	prefs->setBlending(false);
      }
      else {
	glEnable(GL_BLEND);
	prefs->setBlending(true);
      }
      break;
    
    case 't':					// Bernhard's Turbo
      speed += 1;
      break;
    case ' ':					// full stop
      speed = 0;
      break;
    case '#':
    	viewingAngle += 180;
    	if (viewingAngle>180)
	viewingAngle-=360;
    	break;
    
    case 'U':                   // faster
    case 'u':
      if ( speed >= 0 )         // Acceleration in forward movement
	speed += 0.05;
      else {                    // Braking
	speed += 0.2;           // brake
	if (speed > 0 )         // started reverse movement while braking
	  speed = 0;            // brake only untill stopping
      }
      break;
    case 'N':                   // slower
    case 'n':
      if ( speed <= 0 )         // Acceleration in backward movement
	speed -= 0.05;
      else {                    // Braking
	speed -= 0.2;           // brake
	if (speed < 0 )         // started reverse movement while braking
	  speed = 0;            // brake only untill stopping
      }
      break;
    case 'H':                   // left
    case 'h':
      viewingAngle--;
      if (viewingAngle>180)
	viewingAngle-=360;
      break;
    case 'J':                   // right
    case 'j':
      viewingAngle++;
      if (viewingAngle<-180)
	viewingAngle+=360;
      break;
    case 'F':
    case 'f':
      if (!isFog) {
	glEnable(GL_FOG);
	isFog = true;
      }
      else {
	glDisable(GL_FOG);
	isFog = false;
      }
      break;
    case 'd':
    case 'D':
      listEnd = graphicObjects.end();
      for( itr = graphicObjects.begin(); itr != listEnd; ++itr )
	(*itr)->hide();
      break;
    case 's':
    case 'S':
      listEnd = graphicObjects.end();
      for( itr = graphicObjects.begin(); itr != listEnd; ++itr )
	(*itr)->unhide();
      break;
    case 'r':
    case 'R':
      if (showFramerate)
	showFramerate=false;
      else
	showFramerate=true;
      break;
    case 'e': 
    case 'E':
      if (isSerial)
	isSerial=false;
      else
	isSerial=true;
      break;
    case 'w':
    case 'W':
      if (!fullscreen) {
	glutFullScreen();
	fullscreen = true;
      }
      else {
	glutReshapeWindow(width, height);
	fullscreen = false;
      }
      break;
    case '1':
      car = pickCar();
      if( car )
	    car->change( Car::change_nextColor );
      break;
    case '2':
      carVector[ rndInt( carVector.size() ) ]->change( Car::change_toggleBrakeLight );
      break;
    case '3':
      carVector[ rndInt( carVector.size() ) ]->toggleVisibility();
      break;
    case '4':
      ( (street*)curStreet )->toggleMiddleline();
   	  break;
    default:
#ifdef DEBUG
      cout << "A normal key was pressed. Hurra!" << endl;
#endif
      break;
    }
  }


  // Call back function for special keyboard events (cursor keys, etc.)
  void mainAppWindow::CallBackSpecialKeyboardFunc(int key, int x, int y) {
    cout << "A special key was pressed. Double hurra." << endl;
  }


  void mainAppWindow::StartSpinning(GlutMaster* glutMaster){
    glutMaster->SetIdleToCurrentWindow();
    glutMaster->EnableIdleFunction();
  }


  void mainAppWindow::setstreet(GObject* s) {
    curStreet = s;
    GLfloat loc[ 3 ];
    ((street*)s)->getstreetLocation( 0.05, loc );
    
    glLoadIdentity();
  	glTranslatef( -loc[ 0 ], -loc[ 1 ], -loc[ 2 ] );
  }


  void mainAppWindow::setPlane(GObject* p) {
    plane = p;
  }


  void mainAppWindow::addGraphicObject( GObject* obj) {
    // Append the passed graphic object pointer to the list
    graphicObjects.push_back( obj );
  }


  void mainAppWindow::addCar( Car* c ) {
    carVector.push_back( c );
    addGraphicObject( c );
  }
  
  typedef float frustum_t[ 6 ][ 4 ];

	static void extractFrustum( frustum_t& frustum )
	{
	   float   proj[16];
	   float   modl[16];
	   float   clip[16];
	   float   t;

	   /* Get the current PROJECTION matrix from OpenGL */
	   glGetFloatv( GL_PROJECTION_MATRIX, proj );

	   /* Get the current MODELVIEW matrix from OpenGL */
	   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	   /* Combine the two matrices (multiply projection by modelview) */
	   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	   /* Extract the numbers for the RIGHT plane */
	   frustum[0][0] = clip[ 3] - clip[ 0];
	   frustum[0][1] = clip[ 7] - clip[ 4];
	   frustum[0][2] = clip[11] - clip[ 8];
	   frustum[0][3] = clip[15] - clip[12];

	   /* Normalize the result */
	   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	   frustum[0][0] /= t;
	   frustum[0][1] /= t;
	   frustum[0][2] /= t;
	   frustum[0][3] /= t;

	   /* Extract the numbers for the LEFT plane */
	   frustum[1][0] = clip[ 3] + clip[ 0];
	   frustum[1][1] = clip[ 7] + clip[ 4];
	   frustum[1][2] = clip[11] + clip[ 8];
	   frustum[1][3] = clip[15] + clip[12];

	   /* Normalize the result */
	   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	   frustum[1][0] /= t;
	   frustum[1][1] /= t;
	   frustum[1][2] /= t;
	   frustum[1][3] /= t;

	   /* Extract the BOTTOM plane */
	   frustum[2][0] = clip[ 3] + clip[ 1];
	   frustum[2][1] = clip[ 7] + clip[ 5];
	   frustum[2][2] = clip[11] + clip[ 9];
	   frustum[2][3] = clip[15] + clip[13];

	   /* Normalize the result */
	   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	   frustum[2][0] /= t;
	   frustum[2][1] /= t;
	   frustum[2][2] /= t;
	   frustum[2][3] /= t;

	   /* Extract the TOP plane */
	   frustum[3][0] = clip[ 3] - clip[ 1];
	   frustum[3][1] = clip[ 7] - clip[ 5];
	   frustum[3][2] = clip[11] - clip[ 9];
	   frustum[3][3] = clip[15] - clip[13];

	   /* Normalize the result */
	   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	   frustum[3][0] /= t;
	   frustum[3][1] /= t;
	   frustum[3][2] /= t;
	   frustum[3][3] /= t;

	   /* Extract the FAR plane */
	   frustum[4][0] = clip[ 3] - clip[ 2];
	   frustum[4][1] = clip[ 7] - clip[ 6];
	   frustum[4][2] = clip[11] - clip[10];
	   frustum[4][3] = clip[15] - clip[14];
	   //frustum[4][3] = 0; // HACK

	   /* Normalize the result */
	   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	   frustum[4][0] /= t;
	   frustum[4][1] /= t;
	   frustum[4][2] /= t;
	   frustum[4][3] /= t;

	   /* Extract the NEAR plane */
	   frustum[5][0] = clip[ 3] + clip[ 2];
	   frustum[5][1] = clip[ 7] + clip[ 6];
	   frustum[5][2] = clip[11] + clip[10];
	   frustum[5][3] = clip[15] + clip[14];

	   /* Normalize the result */
	   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	   frustum[5][0] /= t;
	   frustum[5][1] /= t;
	   frustum[5][2] /= t;
	   frustum[5][3] /= t;
	   
	   /*for( int i = 0; i < 4; i++ )
	   		printf( "%lf\n", frustum[ 4 ][ i ] );
  		printf( "f5\n" );
	   for( int i = 0; i < 4; i++ )
	   		printf( "%lf\n", frustum[ 5 ][ i ] );*/
	}

	int carVisible( frustum_t& frustum, Car* car )
	{
		Point pos;
		float x, y, z;
		float radius;
		car->getPos( pos );
		x = pos.x;
		y = pos.y;
		z = pos.z;
		radius = car->radius();
		
		//printf( "%f,%f,%f,%f\n", x, y, z, radius );
	
		int p;
		int c = 0;
		float d;

		for( p = 0; p < 6; p++ )
		{
			d = frustum[p][0] * x +
				frustum[p][1] * y +
				frustum[p][2] * z + frustum[p][3];
			if( d <= -radius )
				return 0;
			if( d > radius )
				c++;
		}
		return (c == 6) ? 2 : 1;
	}

  Car* mainAppWindow::pickCar() {
  	frustum_t frustum;
  	int numCars = carVector.size();
  	extractFrustum( frustum );
  	vector<int> array;
  	for( int i = 0; i < numCars; i++ )
  		array.push_back( i );
  		
    for( int i = 0; i < numCars; i++ )
    {
	  	Car* car;
    	int index = rndInt( array.size() );
	    car = carVector[ array[ index ] ];
	    array.erase( array.begin() + index );
  		if( carVisible( frustum, car ) != 0 )
    		return car;
  	}
  	
  	return 0;
  }

  // Calculates time passed since last call of this function
  double mainAppWindow::getTimePassed() {
    clock_t tnew, ris;

    tnew = clock();
    ris = tnew-oldTime;
    oldTime = tnew;

    return (ris/(double)CLOCKS_PER_SEC);
  }
}

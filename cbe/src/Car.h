// Car.h - source file for the CBE project
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

#ifndef __CAR_H
#define __CAR_H

#include "GMovableObject.h"
#include "Point.h"

class Car: public GMovableObject {
 private:
  GLuint gl_wheelList;
  
  GLfloat width; // The car width
  GLfloat length; // The car length
  // See the scheme for meaning of the following variables
  GLfloat x1Front, x2Front, x3Front, x4Front;
  GLfloat x1Back, x2Back, x3Back, x4Back;

  GLfloat h1Front, h2Front, h3Front, h4Front;
  GLfloat h1Back, h2Back, h3Back, h4Back;
	
  GLfloat frontWheelx; // The distance from the front of the
  // car to the front wheel axis
  GLfloat backWheelx; // The distance from the front of the
  // car to the back wheel axis
  GLfloat wheelRadius1; // The radius of the wheels
  GLfloat wheelRadius2; // The inner radius of the wheels (Felgen)
  GLfloat wheelWidth1; // The width of the wheels
  GLfloat wheelWidth2; // The inner width of the wheels (is a bit leaner)

 public:
  Car();
  ~Car();
  void writeList();
  void makeList();
  // Return the number of the GL list for a wheel
  GLuint inline getWheelList() {
    return gl_wheelList;
  }

 protected:
  void drawCoachwork(); // Karosserie zeichnen
  // Make a gl_list for a wheel, used by drawCoachwork().
  virtual void writeWheelList();
};

// A test of the GMovableObject class and the Car class
class TestCar: public Car {
 private:
  double oldTime;
  double deltaX, deltaAngle;

 public:
  TestCar();
  TestCar( double dx, double dr );
  ~TestCar();
  void draw();
};

#endif

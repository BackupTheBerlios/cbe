// joystick-driver.hh - source file for the CBE project
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
#include <linux/joystick.h> // so we need at least the Kernel-Headers
#include <fcntl.h>  // Needed by open
#include <unistd.h> // Needed by read
}

#include "JoystickDriver.hh"
#include <iostream>

// Constructor
JoystickDriver::JoystickDriver() {
  // Initialize Values
  xAxis=128;
  yAxis=128;
  buttons=0;
  joystickAvailable=false;

  joydev=open("/dev/js0", O_RDONLY); // open the first joystick-device
  if (joydev != -1) {  // ok, joystick available
    joystickAvailable=true;
    refreshJoystick();
  }
}

// Destructor
JoystickDriver::~JoystickDriver() {
}

// just get the jostic-Values into the object
// here I'm using the old and depreciated 0.x Linux-Joystick-Driver
// View /usr/src/linux/Documentation/joystick-api.txt on linux-systems for explanation
void JoystickDriver::refreshJoystick() {
  struct JS_DATA_TYPE js;
  if (joystickAvailable==true) {
    if (read(joydev, &js, JS_RETURN)!=JS_RETURN)
      cout << "Problem reading joystick device!" << endl;
    buttons=js.buttons;
    xAxis=js.x;
    yAxis=js.y;

  }
}

// Returns x-value
float JoystickDriver::getXaxis() {
  //return ((float)xAxis/(float)32767);
  return((float)(xAxis-128)/128);
}

// Returns y-value
float JoystickDriver::getYaxis() {
  //return ((float)yAxis/(float)32767);
  return((float)(yAxis-128)/128);
}

// Returns buttons (first button: button&1, second: button&2 ...)
int JoystickDriver::getButtons() {
  return buttons;
}

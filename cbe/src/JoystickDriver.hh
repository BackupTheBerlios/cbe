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


#include <string>
#include <iostream>
#include "Preferences.h"

using namespace std;

// very simple joystick-interface
class JoystickDriver {
private:
  pref::Preferences *prefs;      // Preferences object
  string joystick;               // As an alternative to the preferences object
  int joydev;
  int xAxis;                     // Position x-axis
  int yAxis;                     // Position y-axis
  int buttons;                   // Status of buttons
  bool joystickAvailable;        // true if joystick available

public:
  JoystickDriver(pref::Preferences*);  // Initialises first available joystick
  JoystickDriver(string); 
  ~JoystickDriver();

  void refreshJoystick();              // Reads the new joy-values into the object
  float getXaxis();                    // returns value for the x-axis (-1<=x<=1)
  float getYaxis();                    // returns value for the y-axis (-1<=y<=1)
  int getButtons();                    // returns status of buttons 
};

// SerialClient.hh - source file for the CBE project
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
#include <termios.h>
}

#define BAUDRATE B115200

// some constants
#define REQUEST_NORMAL 0
#define REQUEST_CHANGED 1

#define CHANGE_NOTHING 0
#define CHANGE_HIDE_CAR 49
#define CHANGE_TOGGLE_BREAKLIGHTS 50

class SerialClient {
public:
  SerialClient(const char * serial_device);
  ~SerialClient();

  void requestData();  // Requests and reads new eye-position
  void onlyRequestData(); // only in conjuction with onlyGetData()
  void onlyGetData(); // then the same semantics as requestData()
  bool isRun(); // return true for "run" and false for "reset"
  int getChange(); // returns status of what to change (0:nothing, 1:hide_car ...) 
  void setChange(); // used to confirm changes to eye-position-server
  int getX(); // returns x-value of eye
  int getY(); // returns y-value of eye
  // Serial-Device is available
  bool isAvailable;

private:
  int devptr;
  struct termios newtio;  // for the new serialport settings
  int change;
  int status;
  bool wasChange; // used to confirm changes back to the server
  bool isRandom; // possibility to create events randomly
  int x,y; // Position of eye (x and y values)
};

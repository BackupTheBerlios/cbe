// PPortDriver.cc - source file for the CBE project
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
#include <fcntl.h> // Needed by open
#include <unistd.h>  // Needed by read
#include <string.h> // Stingfunctions
}

#include <cstdlib>
#include <iostream>
#include <PPortDriver.hh>

// Constructor
PPortDriver::PPortDriver(const char * device) {
  data=0;
  oldBlink=false;
  randomData=false;
  if (strcmp(device, "random")==0) {
    randomData=true;
    cout << "Using random eye-events" << endl;
  }
  else {
    ppdev=open(device, O_RDONLY); // open device
    if (ppdev==-1) // Parallel port not opened
      cout << "Parallel port not opened expect no further data from Parallelport" << endl;
  }
  refreshData();
}

// Destructor
PPortDriver::~PPortDriver() {
}

// read new data from parallel port
void PPortDriver::refreshData() {
  if (ppdev!=-1) {
    if (randomData) 
      data=rand()%256;
    else
      if (read(ppdev,&data,1)!=1)
	cout << "Problem with reading from parallel port" << endl;
  }
}

// return true if blink is detected
bool PPortDriver::isBlink() {
  bool blink=false;
  if (data==1) { // Blink only reported if data=1 and oldBlink=false
    if (oldBlink==false)
      blink=true;
    oldBlink=true;
  }
  else
    oldBlink=false; // If eye opened reset oldBlink
  
  return(blink);
}

// Bitmap.h - source file for the CBE project
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


#ifndef __BITMAP_H_
#define __BITMAP_H_

#include <iostream>
#include <string>
extern "C" {
#include <stdio.h>
#include <tiffio.h>
}
#include "config.h"

using namespace std;

class Bitmap {
 private:
  string path;   // Path to image
  uint32* data;  // The image itself
  TIFF* pic;     // File handle to picture 
  uint32 sizeX;  // Image size
  uint32 sizeY;  // Image size
  
 public:
  Bitmap();
  Bitmap(string);
  ~Bitmap();
  bool load(void);
  void setPath(string);
  uint32* getData(void);
};

#endif

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

#include <string>
extern "C" {
#include <stdio.h>
}
#include "config.h"
#ifdef _WIN32
#include <iostream>
using namespace std;
#endif

class Bitmap {
 private:
  string path;          // Path to image
  char* data;           // The image itself
  unsigned long sizeX;  // Image size
  unsigned long sizeY;  // Image size
  
  unsigned int getint(FILE*);
  unsigned int getshort(FILE*);

 public:
  Bitmap();
  Bitmap(string);
  ~Bitmap();
  bool load(void);
  void setPath(string);
  char* getData(void);
};

#endif

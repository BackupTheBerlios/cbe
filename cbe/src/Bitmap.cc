// Bitmap.cc - source file for the CBE project
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


#include <iostream>
#include <string>
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
}
#include "Bitmap.h"


using namespace std;


Bitmap::Bitmap(string newPath) {
  path = newPath;
  data = NULL;
}


Bitmap::Bitmap() {
  path = "";
  data = NULL;
}


bool Bitmap::load(void) {
  pic = TIFFOpen(path.c_str(), "r");
  
  if (pic) {
    size_t npixels;

    TIFFGetField(pic, TIFFTAG_IMAGEWIDTH, &sizeX);
    TIFFGetField(pic, TIFFTAG_IMAGELENGTH, &sizeY);
    npixels = sizeX * sizeY;
    data = (uint32*) _TIFFmalloc(npixels * sizeof(uint32));

    if (data != NULL) {
      if (!TIFFReadRGBAImage(pic, sizeX, sizeY, data, 0)) {
	cerr << "Error opening TIF picture." << endl;
	TIFFClose(pic);
	return false;
      }
      else {
#ifdef DEBUG
	cout << "TIF picture successfully loaded." << endl;
#endif

#if macintosh
	uint8* p = (uint8*)data;
	
	for( long i = 0; i < sizeX * sizeY; i++ )
	{
		int r, g, b, a;
		r = p[ 0 ];
		g = p[ 1 ];
		b = p[ 2 ];
		a = p[ 3 ];
		p[ 0 ] = a;
		p[ 1 ] = b;
		p[ 2 ] = g;
		p[ 3 ] = r;
		p += 4;
	}
#endif

	TIFFClose(pic);
	return true;
      }
    }
  }
  
  return false;
}


Bitmap::~Bitmap() {
  if (data != NULL)
    _TIFFfree(data);

#ifdef DEBUG  
  cout << "Destructor of Bitmap object called." << endl;
#endif
}


void Bitmap::setPath(string newPath) {
  path = newPath;
}


uint32* Bitmap::getData(void) {
  return data;
}

uint32 Bitmap::getWidth() {
  return sizeX;
}

uint32 Bitmap::getHeight() {
  return sizeY;
}

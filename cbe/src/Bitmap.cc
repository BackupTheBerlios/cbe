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
}
#include "Bitmap.h"


Bitmap::Bitmap(string newPath) {
  path = newPath;
  data = NULL;
}


Bitmap::Bitmap() {
  path = "";
  data = NULL;
}


// Code to load 24bit BMP files, shamelessly copied from:
//
// http://nehe.gamedev.net/opengl.asp
//
// Modified and changed into C++ style by Andreas Bauer.
//
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more information.
bool Bitmap::load(void) {
  FILE* file;                         //  file handle for the image
  unsigned long size;                 //  size of the image in bytes.
  unsigned long i;                    //  standard counter.
  unsigned short int planes;          //  number of planes in image (must be 1)
  unsigned short int bpp;             //  number of bits per pixel (must be 24)
  char temp;                          //  used to convert bgr to rgb color.
  
  if (path.length() == 0)
    return false;                     // User has to specify valid path first
  else {
    if ((file = fopen(path.c_str(), "rb")) == NULL)
      return false;                   // File 'filename' not found
    else {
      //  seek through the bmp header, up to the width/height
      fseek(file, 18, SEEK_CUR);
      
      //  read the width
      sizeX = getint(file);
      
      //  read the height
      sizeY = getint(file);
      
      //  calculate the size (assuming 24 bits or 3 bytes per pixel).
      size = sizeX * sizeY * 3;
      
      //  read the planes
      planes = getshort(file);
      if (planes != 1)
	return false;           // Planes error

      //  read the bpp
      bpp = getshort(file);
      if (bpp != 24)
	return false;           // Image is not a 24bit BMP file
      
      //  seek past the rest of the bitmap header.
      fseek(file, 24, SEEK_CUR);
      
      //  read the data.
      data = (char*)malloc(size);
      if (data == NULL)
	return false;           // Memory allocation error
      
      if ((i = fread(data, size, 1, file)) != 1)
	return false;           // Read error
      
      for (i = 0; i < size; i += 3) { //  reverse all of the colors. (bgr -> rgb)
	temp = data[i];
	data[i] = data[i+2];
	data[i+2] = temp;
      }
      
      //  we're done.
      return true;
    }
  }
}


Bitmap::~Bitmap() {
  if (data != NULL)
    free(data);

  std::cout << "Destructor of Bitmap object called." << std::endl;
}


void Bitmap::setPath(string newPath) {
  path = newPath;
}


// getint and getshort arehelp functions to load the bitmap byte by byte on 
// SPARC platform.
// I've got them from xv bitmap load routinebecause the original bmp loader didn't work
// I've tried to change as less code as possible.
unsigned int Bitmap::getint(FILE* fp) {
  int c, c1, c2, c3;

  //  get 4 bytes
  c = getc(fp);  
  c1 = getc(fp);  
  c2 = getc(fp);  
  c3 = getc(fp);
  
  return((unsigned int) c) +   
    (((unsigned int) c1) << 8) + 
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}


unsigned int Bitmap::getshort(FILE* fp) {
  int c, c1;
  
  // get 2 bytes
  c = getc(fp);  
  c1 = getc(fp);

  return((unsigned int) c) + (((unsigned int) c1) << 8);
}

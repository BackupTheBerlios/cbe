// GObject.cc - source file for the CBE project
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

#include "GListObject.h"


// This class is the basic class for graphic objects
GListObject::GListObject() {
  gl_list = glGenLists( 1 );          // Create a new list for the object
}


GListObject::~GListObject() {
  glDeleteLists( gl_list, 1 );        // Delete the GL list corresponding to the object.
}


void GListObject::submit() {
  drawObjectLists();
}


void GListObject::drawObjectLists() {
  glCallList( gl_list );             // Draw the object
}


void GListObject::makeList() {
  glNewList(getList(), GL_COMPILE); // Initialize the list
  writeList();                      // Save the GL operations to the list
  glEndList();                      // Finish the list
}


// The procedure is empty in this class, but this empty 
// implementation may not be removed from here, because 
// it would produce a link error.
void GListObject::writeList() {}

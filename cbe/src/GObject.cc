// GObject.cc - source file for the CBE project
//
// Original code by George Stetten and Korin Crawford
// Copyright given to the public domain
// Please email comments to stetten@acpub.duke.edu
//
// Modifications are copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen
//                                       http://www.uni-muenchen.de/
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

#include "GObject.h"

// This class is the basic class for graphic objects
GObject::GObject() {
  hidden = false; // Not hidden at the beginning
  gl_list = glGenLists( 1 ); // Create a new list for the object
}

GObject::~GObject() {
  glDeleteLists( gl_list, 1 ); // Delete the GL list corresponding to the object.
}

void GObject::draw() {
  if ( !hidden ) {
    glCallList( gl_list ); // Draw the object
  }
}

// The procedure is empty in this class, but this empty 
// implementation may not be removed from here, because 
// it would produce a link error.
void GObject::makeList() {
}

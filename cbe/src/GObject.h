// GObject.h - source file for the CBE project
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

#ifndef __GOBJECT_H
#define __GOBJECT_H

extern "C" {
#include <GL/gl.h>
}

class GObject {
 private:
  bool hidden; // Shows, whether the object is hidden

 public:
  GObject();
  virtual ~GObject();
  
  // Sets the hidden flag to true. The object will not be
  // displayed by the next draw().
  virtual inline void hide() {
    hidden = true; 
  }
  // Sets the hidden flag to false. The object will be
  // displayed by the next draw().
  virtual inline void unhide() {
    hidden = false;
  }
  // Show the object if it is hidden and hide it, if it is visible.
  virtual inline void toggleVisibility() {
	hidden = !hidden;
  }
  // Returns the hidden-flag.
  inline bool isHidden() {
	  return hidden;
  }
  // Draws the object.
  virtual void draw();

  // Functions for changing something like color, shape etc.
  // In each function the parameter changeNum gives the number
  // of the change to be performed. These numbers are specific
  // to each class derived from GObject.
  //
  // In GObject class this procedures has no function. You just
  // have to overwrite them in a derived class, if You want
  // it to be able to perform changes.
  //
  // If You want to add a new change function like 
  // changeSomething( int changeNum ) for a new class, You should 
  // add the function already in the GObject class, so that it can 
  // be called for any instance of a subclass of GObject without
  // causing an error.
  virtual void change( int changeNum );
  virtual void changeColor( int changeNum );
  virtual void changeShape( int changeNum );
};

#endif

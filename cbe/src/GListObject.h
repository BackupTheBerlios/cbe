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

#ifndef __GLISTOBJECT_H
#define __GLISTOBJECT_H

#include <GMovableObject.h>

class GListObject : public GMovableObject {
 private:
  GLuint gl_list; // The GL number of the list of this object

 public:
  GListObject();
  virtual ~GListObject();
  // Returns the GL number of the list of this object
  virtual GLuint getList() {
    return gl_list;
  }

  // Creates the graphic representation of the object. 
  // It creates an new Open GL list an calls writeList() to
  // write the Open GL operations to the GL list with the
  // number gl_list created in the constructor.
  // This procedure does not have to, but it may be overwritten.
  // If it is overwritten, it must preserve the structure:
  //   ...
  //   glNewList(getList(), GL_COMPILE);
  //   writeList();
  //   glEndList();
  virtual void makeList();

  // Writes the Open GL operations to the list.
  // It is called by makeList() after initializing the Open GL 
  // list for the current graphic object.
  // This procedure must be overwritten in each classe derived
  // from GObject.
  // You may also enhance the graphical representation of an
  // object by deriving from its class and expanding its
  // writeList() procedure. The new procedure would look like:
  //   UpperClass::writeList(); // Write the graphical representation
  //                            // of the upper class
  //   ... // add new graphical operations
  // As the list is not closed by glEndList() in the writeList()
  // procedure, this expanding will work.
  virtual void writeList() = 0;
  
  virtual void submit();

protected:
  // Draws the Open GL list(s) for the graphic object.
  // If a derived class must draw several lists or, You just
  // have to overwrite this function without changing the draw()
  // function.
  virtual void drawObjectLists();
};

#endif // __GLISTOBJECT_H

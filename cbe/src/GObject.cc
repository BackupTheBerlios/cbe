// GObject.cpp: implementation of the GObject class.
//
//////////////////////////////////////////////////////////////////////

#include "GObject.h"
/* This class is the basic class for graphic objects */
GObject::GObject()
{
	hidden = false; // Not hidden at the beginning
	gl_list = glGenLists( 1 ); // Create a new list for the object
}

GObject::~GObject()
{
	glDeleteLists( gl_list, 1 ); /* Delete the GL list corresponding
		to the object. */
}

void GObject::draw() {
	if ( !hidden ) {
		glCallList( gl_list ); // Draw the object
	}
}

/* The procedure is empty in this class, but this empty 
   implementation may not be removed from here, because 
   it would produce a link error. */
void GObject::makeList() {}

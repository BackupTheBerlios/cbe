// GObject.h: interface for the GObject class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GOBJECT_H
#define __GOBJECT_H

#ifdef _WIN32
  #include <GL/glaux.h>
#endif

#include <GL/gl.h>

class GObject {
private:
	GLuint gl_list; // The GL number of the list of this object
	bool hidden; // Shows, whether the object is hidden
public:
	GObject();
	virtual ~GObject();
	// Returns the GL number of the list of this object
	virtual inline GLuint getList() { return gl_list; }
	/* Creates the graphic representation of the object. It
	   writes the graphic operations to the GL list with the
	   number gl_list created in the constructor.
	   This procedure must be overwritten in each class derived
	   from GObject. */
	virtual void makeList() = 0;
	/* Sets the hidden flag to true. The object will not be
	   displayed by the next draw(). */
	virtual inline void hide() { hidden = true; }
	/* Sets the hidden flag to false. The object will be
	   displayed by the next draw(). */
	virtual void unhide() { hidden = false; }
	// Draws the object.
	virtual void draw();
};

#endif

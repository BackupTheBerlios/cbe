// cbe.cc - source file for the CBE project
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

extern "C" {
#include <GL/gl.h>    // OpenGL
#include <GL/glut.h>  // GLUT
}
#include "common.h"
#include "config.h"
#include "cbe.hh"
#include "glutMaster.h"
#include "glutWindow.h"
#include "demoWindow.h"
#include "street.h"
#include "plane.h"

using namespace std;

GlutMaster* glutMaster;
DemoWindow* secondWindow = 0;



int main(int argc, char *argv[]) {
  cout << "Entering main in cbe.cc\n";
  glutMaster   = new GlutMaster(&argc, argv);  
  cout << "created GlutMaster in cbe.cc\n";
  secondWindow = new DemoWindow(glutMaster,
				500, 500,         // height, width
				200, 400,         // initPosition (x,y)
				(string)PACKAGE + (string)" " + (string)VERSION);   // title
  cout << "created DemoWindow secondWindow in cbe.cc\n";
  Street* s=new Street(-50,0,0,2);
  GLuint streetList=s->getStreet();
  secondWindow->setStreet(&streetList);
  Point a,b;
  a.x=-60; a.y=-0.1; a.z=-60;
  b.x=60; b.y=-0.1; b.z=60;
  Plane* p=new Plane(a,b);
  GLuint Planelist=p->getPlane();
  secondWindow->setPlane(&Planelist);

  secondWindow->StartSpinning(glutMaster);        // enable idle function
  cout << "Started Spinning in secondWindow in cbe.cc\n";
  glutMaster->CallGlutMainLoop();
  cout << "CallGlutMainLoop() success\n";

  return 0;
}

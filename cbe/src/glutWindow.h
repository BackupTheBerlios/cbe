// glutWindow.h - source file for the CBE project
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

#ifndef __GLUT_WINDOW_H__
#define __GLUT_WINDOW_H__

class GlutWindow{
protected:

   int          windowID;

public:

   GlutWindow(void);
   virtual ~GlutWindow();

   virtual void CallBackDisplayFunc();
   virtual void CallBackIdleFunc(void);
   virtual void CallBackKeyboardFunc(unsigned char key, int x, int y);
   virtual void CallBackMotionFunc(int x, int y);
   virtual void CallBackMouseFunc(int button, int state, int x, int y);
   virtual void CallBackPassiveMotionFunc(int x, int y);
   virtual void CallBackReshapeFunc(int w, int h);   
   virtual void CallBackSpecialKeyboardFunc(int key, int x, int y);   
   virtual void CallBackVisibilityFunc(int visible);

           void SetWindowID(int newWindowID);
           int  GetWindowID(void);

};

#endif








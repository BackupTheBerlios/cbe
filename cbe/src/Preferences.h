// Preferences.hh - source file for the cbe project
//
// Method was originally taken from Mailfilter, http://mailfilter.sourceforge.net/
//
// Changes are Copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen
//                                 http://www.uni-muenchen.de/
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

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <string>
#include <list>

using namespace std;

namespace pref {
  
  class Preferences {
  private:
    int lineCount;
    string curLine;
    bool blending;
    int framerate;
    string joystick, serial;
    list<string> getPrefValues(string);
    void showErrorParameter(string, string, string);

  public:
    Preferences(string);
    ~Preferences();
    bool useBlending(void);
    void setBlending(bool);
    void setFramerate(int);
    int getFramerate(void);
    void setJoystick(string);
    string getJoystick(void);
    void setSerial(string);
    string getSerial(void);
  };
  
  // Exceptions
  class MalformedPrefsFile { };
  class IOException { };
}

#endif

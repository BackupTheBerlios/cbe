// Preferences.cc - source file for the cbe project
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

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include "Preferences.h"
#include "config.h"

using namespace std;

namespace pref {

  Preferences::Preferences(string file) {
    ifstream prefsFile(file.c_str());
    setBlending(true);
    lineCount = 0;
    
    if (prefsFile) {
      try {
	while (getline(prefsFile, curLine)) {
	  lineCount++;

	  if (curLine[0] == '#')
	    continue;
	  
	  if (curLine.find("BLENDING=", 0) == 0) {
	    if (*(getPrefValues(curLine)).begin() == "no")
	      setBlending(false);
	    else if (*(getPrefValues(curLine)).begin() == "yes")
	      setBlending(true);
	    else {
	      showErrorParameter(file, "BLENDING", *(getPrefValues(curLine)).begin());
	      throw MalformedPrefsFile();	      
	    }
	  }
	  else if (curLine.length()) {
	    cerr << PACKAGE << ": Invalid keyword in '" << file << "' (" << lineCount << "): ";
	    cerr << "'" << curLine.substr(0, curLine.find("=")) << "'" << endl;
	    throw MalformedPrefsFile();
	  }
	}
      }
      catch (...) {
	prefsFile.close();
	throw;
      }
      
      // Close preferences file
      prefsFile.close();
    }
    else
      throw IOException();
  }
  
  
  Preferences::~Preferences() {
  }
  
  
  // Gets the values from an entry inside the preferences file
  // e.g. TEST=value1, value2
  // would result in returning 'value1' and 'value2'
  // (Method as is, taken from mailfilter)
  list<string> Preferences::getPrefValues(string prefLine) {
    list<string> parsedValues;
    int startSearch = prefLine.find('=') + 1;
    int newPos = 0;
    
    if (startSearch != -1) {
      for (int i = startSearch; i <= (int)prefLine.length(); i++) {
	newPos = prefLine.find(',', i);
	
	if (newPos == -1) {
	  parsedValues.push_back( prefLine.substr( i, prefLine.length() ) );
	  break;
	}
	else {
	  parsedValues.push_back( prefLine.substr( i, newPos - i ) );
	  i = newPos + 1;
	}
      }
      
      return parsedValues;
    }
    else {
      throw MalformedPrefsFile();
	  #ifdef _WIN32 // The VC compiler does not handle exceptions correctly
	  return 0;
      #endif
	}
  }


  // Shows an error if a keyword got wrong arguments
  // (Method taken from mailfilter)
  void Preferences::showErrorParameter(string file, string keyword, string parameter) {
    cerr << PACKAGE << ": A keyword in '" << file.c_str() << "' contains invalid parameters." << endl;
    cerr << PACKAGE << ": '" <<  keyword.c_str() << "=" << parameter.c_str() << "'" << endl;
  }


  bool Preferences::useBlending(void) {
    return blending;
  }

  
  void Preferences::setBlending(bool newBlending) {
    blending = newBlending;
  }

}

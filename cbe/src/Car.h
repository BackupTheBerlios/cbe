// Car.h - source file for the CBE project
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

#ifndef __CAR_H
#define __CAR_H

#include "GLinotteObject.h"

class Street;

class Car : public GLinotteObject {
 public:
 	Car();
 
	// Change constants
	enum { change_brakeLightOn = 0, change_brakeLightOff, change_toggleBrakeLight,
		   change_color0, change_color1, change_nextColor };
	// Change constants range
	enum { change_minColor = change_color0,
		   change_maxColor = change_color1 };
	enum { change_minChange = change_brakeLightOn,
		   change_maxChange = change_nextColor };
		   
	virtual void move( Street* street, double time );
	
protected:
	float mRotation;
};

#endif

# cbe
#
# Copyright (c) 2001  Ludwig-Maximilian-Universitaet Muenchen 
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
# USA.

AM_CXXFLAGS=-Wall -DDATADIR=\"@datadir@\" -DPREFIX=\"@prefix@\"

bin_PROGRAMS = cbe 

INCLUDES = @X_CFLAGS@ @GLUT_CFLAGS@ @GL_CFLAGS@

cbe_LDADD = @LIBTIFF@ @X_LDFLAGS@ @GLUT_LDFLAGS@ @GL_LDFLAGS@ @X_LIBS@ @GLUT_LIBS@ @GL_LIBS@

# Don't forget to update NEWS and ChangeLog
cbe_SOURCES = 	cbe.cc glutMaster.cc glutWindow.cc mainAppWindow.cc \
		cbe.hh glutMaster.h  glutWindow.h  mainAppWindow.h \
		GObject.cc Bitmap.cc JoystickDriver.cc SerialClient.cc\
		GObject.h  Bitmap.h  JoystickDriver.hh SerialClient.hh\
		street.cc plane.cc Point.cc Car.cc GMovableObject.cc \
		street.h  plane.h  Point.h  Car.h  GMovableObject.h

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

# `AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS=-Wall -g -DDATADIR=\"@datadir@\" -DPREFIX=\"@prefix@\"

noinst_LIBRARIES = liblinotte.a

INCLUDES = -I$(includedir) -I$(top_srcdir)/src/linotte -I$(top_srcdir)/src @X_CFLAGS@ @GLUT_CFLAGS@ @GL_CFLAGS@

LIBS = @LIBTIFF@ @X_LDFLAGS@ @GLUT_LDFLAGS@ @GL_LDFLAGS@ @X_LIBS@ @GLUT_LIBS@ @GL_LIBS@

# Don't forget to update NEWS and ChangeLog
liblinotte_a_SOURCES = 	array_t.cc model_t.cc \
			array_t.h material_manager_t.cc model_t.h \
			file_input_stream_t.cc material_manager_t.h simple_material_t.cc \
			file_input_stream_t.h material_t.cc simple_material_t.h \
			input_stream_t.cc material_t.h texture_material_t.cc \
			Bitmap.cc Bitmap.h \
			input_stream_t.h mesh_t.cc texture_material_t.h \
			linotte.h mesh_t.h

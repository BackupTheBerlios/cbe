// GLinotteObject.h - source file for the CBE project
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

#ifndef __GLINOTTEOBJECT_H
#define __GLINOTTEOBJECT_H

#include "GMovableObject.h"
#include "linotte/model_t.h"
#include "linotte/material_t.h"

class GLinotteObject : public GMovableObject {
 private:
  linotte::model_t* m_model;
  linotte::material_manager_t* m_matmgr;

 protected:
  void load( const char* path );
  void add_material( linotte::material_t* material );
  linotte::material_t* get_material( const char* name );

  linotte::bsphere_t m_bsphere;

 public:
  GLinotteObject();
  virtual ~GLinotteObject();
  
  virtual void submit();
  
  float radius() const;
};

#endif // __GLINOTTEOBJECT_H

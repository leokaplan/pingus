//  $Id: PinguMap.hh,v 1.5 2000/06/18 17:01:50 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 1999 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef PINGUMAP_HH
#define PINGUMAP_HH

#include <ClanLib/core.h>

#include "WorldObj.hh"
#include "ColMap.hh"

/** The type of the map, currently we have a random, a bitmap and a
    spot map, the only map, which is currently supported is the spot
    map. */
enum MapType { BMP, SPOT, RANDOM, UNDEF }///
;

///
class PinguMap
{
private:
public:
  ///
  PinguMap();
  ///
  virtual ~PinguMap();
  ///
  virtual void draw(int x1, int y1, int w, int h,
		    int x_of, int y_of, float s=1.0) = 0;
  ///
  virtual void mark_dirty(int,int,int,int);
  ///
  virtual ColMap *get_colmap();
  ///
  virtual int  get_width() = 0;
  ///
  virtual int  get_height() = 0;
  ///
  virtual void remove(int, int) {}///
;
  ///
  virtual void remove(CL_SurfaceProvider*, int, int);
  ///
  virtual void remove(CL_Surface*, int, int);
  ///
  virtual void put(CL_SurfaceProvider*, int, int);
}///
;

#endif

/* EOF */

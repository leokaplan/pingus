//  $Id: pingu_holder.cxx,v 1.8 2002/09/18 11:03:00 grumbel Exp $
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

#include <iostream>
#include "pingu_holder.hxx"
#include "pingu.hxx"
#include "pingu_action.hxx"

PinguHolder::PinguHolder()
{
  id_count = 0;
  total_size_count = 0;
  saved_pingus = 0;
}

PinguHolder::~PinguHolder()
{
  // Deleting all Pingu objects
  std::cout << "PinguHolder: Deleting pingus" << std::endl;
  for(std::vector<Pingu*>::iterator i = all_pingus.begin();
      i != all_pingus.end(); ++i)
    delete *i;
}

int
PinguHolder::total_size()
{
  return total_size_count;
}

void
PinguHolder::add (Pingu* pingu)
{
  total_size_count++;
  pingu->set_id(id_count++);
  pingus.push_back(pingu);
}

Pingu*
PinguHolder::create_pingu (const CL_Vector& pos, int owner_id)
{
  Pingu* pingu = new Pingu (pos, owner_id);
  // This list will get evaluated and deleted and destruction
  all_pingus.push_back (pingu);

  return pingu;
}

void
PinguHolder::draw (GraphicContext& gc)
{
  // Draw all walkers
  for(std::list<Pingu*>::iterator pingu = pingus.begin(); 
      pingu != pingus.end(); 
      ++pingu)
    {
      if ((*pingu)->get_action()->get_type() == Actions::Walker) 
	(*pingu)->draw (gc);
    }
  
  // Draw all non-walkers, so that they are easier spotable
  // FIXME: This might be usefull, but looks kind of ugly
  for(std::list<Pingu*>::iterator pingu = pingus.begin(); 
      pingu != pingus.end(); 
      ++pingu)
    {
      if ((*pingu)->get_action()->get_type() != Actions::Walker)
	(*pingu)->draw (gc);
    }
}

void
PinguHolder::update(float delta)
{
  PinguIter pingu = pingus.begin();
  
  while(pingu != pingus.end())
    {
      (*pingu)->update(delta);
      
      // FIXME: The draw-loop is not the place for things like this,
      // this belongs in the update loop
      if ((*pingu)->get_status() == PS_DEAD)
	{
	  // Removing the dead pingu and setting the iterator back to
	  // the correct possition, no memory hole since pingus will
	  // keep track of the allocated Pingus
	  pingu = pingus.erase(pingu);
	}
      else if ((*pingu)->get_status() == PS_EXITED) 
	{
	  saved_pingus++;
	  pingu = pingus.erase(pingu);
	}
      else 
	{
	  // move to the next Pingu
	  pingu++;
	}
    }
}

/* EOF */

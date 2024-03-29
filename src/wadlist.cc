/*
 *	wadlist.cc - Wad_list class
 *	AYM 2001-09-23
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-2003 Andr� Majorel and others.and others.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include "yadex.h"
#include "wadfile.h"
#include "wadlist.h"
#include <list>
#include <boost/smart_ptr.hpp>


Wad_list wad_list;			// One global instance


typedef std::list<boost::shared_ptr<Wad_file> > list_t;


class Wad_list_priv
{
  public :
    Wad_list_priv ();
    list_t list;
    mutable list_t::iterator iter;	// Current element
    mutable bool rewound;		// If true, next get() will return head
};


Wad_list_priv::Wad_list_priv ()
{
  rewound = true;
}


/*
 *	Wad_list::Wad_list - default ctor
 */
Wad_list::Wad_list ()
{
  priv = new Wad_list_priv;
}


/*
 *	Wad_list::~Wad_list - dtor
 */
Wad_list::~Wad_list ()
{
  delete priv;
  priv = 0;				// Catch bugs
}


/*
 *	Wad_list::rewind - arrange so that the next get() will return head
 */
void Wad_list::rewind () const
{
  priv->rewound = true;
}


/*
 *	Wad_list::get - retrieve the next wad from the list
 *
 *	If get() was not called since the last call to rewind(),
 *	the first wad is returned. Otherwise, the wad returned
 *	is the one that follows the one returned by the previous
 *	call to get().
 *
 *	Return false if already at the end of the list, true
 *	otherwise.
 */
bool Wad_list::get (Wad_file *& wf)
{
  if (priv->rewound)
  {
    priv->iter = priv->list.begin ();
    priv->rewound = false;
  }
  else
    ++priv->iter;

  if (priv->iter == priv->list.end ())
  {
    wf = 0;
    return false;
  }

  wf = priv->iter->get ();
  return true;
}


bool Wad_list::get (const Wad_file *& wf) const
{
  if (priv->rewound)
  {
    priv->iter = priv->list.begin ();
    priv->rewound = false;
  }
  else
    ++priv->iter;

  if (priv->iter == priv->list.end ())
    return false;

  wf = priv->iter->get ();
  return true;
}


/*
 *	Wad_list::insert - insert a new wad at the pointer
 */
void Wad_list::insert (Wad_file *wf)
{
  priv->list.insert (priv->iter, boost::shared_ptr<Wad_file> (wf));
}


/*
 *	Wad_list::del - delete from the list the wad under the pointer
 *
 *	If get() was not called since the last call to rewind(),
 *	the first wad is deleted. Otherwise, the wad deleted is
 *	the one returned by the last call to get().
 *
 *	After a call to del(), a call to get will return the wad
 *	the followed the one that was deleted.
 *
 *	If already at the end of the list, nf_bug() is called.
 */
void Wad_list::del ()
{
  list_t::iterator i;

  if (priv->rewound)
  {
    i = priv->list.begin ();
  }
  else
  {
    i = priv->iter;
  }
  if (i == priv->list.end ())
  {
    nf_bug ("Wad_list::del: attempt to delete last item");
    return;
  }
  priv->iter = priv->list.erase (i);
  if (priv->iter == priv->list.begin ())
  {
    priv->iter = 0;			// Catch bugs
    priv->rewound = true;
  }
}


/* This file is part of sutil, a random collection of utilities.

sutil is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Alternatively, you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

sutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License and a copy of the GNU General Public License along with
sutil. If not, see <http://www.gnu.org/licenses/>.
 */
/* \file  CObjectHistory.hpp
 *
 *  Created on: Nov 18, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef COBJECTHISTORY_HPP_
#define COBJECTHISTORY_HPP_

#include <sutil/CMappedList.hpp>
#include <sutil/CSystemClock.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  /** Time stamps an object, copies it and saves it.
   * The object may be restored at any future point of time.
   *
   * NOTE: Since this requires a memory copy, it will be
   *       slow in general.
   *
   * Templpate parameters:
   * 1. Idx :
   * The class will create a map and will index all stored
   * objects by an Idx type key. Each object's key must be
   * unique.
   *
   * 2. StoreObjectsAs :
   * The class requires a you to convert all objects
   * into a given StoreObjectsAs type.
   * The StoreObjectsAs type must have a public constructor,
   * a copy constructor and a destructor that correctly
   * deallocates its memory.
   * Ie. An Idx will work, but a simple typecast to
   * char* won't.
   *
   * Of course, you must use another class in tandem to convert
   * your objects into something that can be stored here.
   * */
  template<typename Idx, typename StoreObjectsAs /* = Idx //works best */>
  class CObjectHistory
  {
  protected:
    /** Index all stored objects by an index and timestamp in a two
     * level mapped list. */
    CMappedList<
      /** The specified index type */
      Idx,
      /** For each logged object, store a mapped list indexed
       * by the timestamp at which the object was logged. */
      CMappedList<double, StoreObjectsAs>
    > data_;

  public:
    /** Saves the current state of an object (can be retrieved later) */
    bool saveObject(const Idx& arg_idx, const StoreObjectsAs& arg_obj)
    {
      bool flag;
      CMappedList<double, StoreObjectsAs>* objl = data_.at(arg_idx);
      if(NULL == objl)
      {
        flag = data_.create(arg_idx);
        if(false == flag)
        {
#ifdef DEBUG
          std::cout<<"\nCObjectHistory::addObject() : Could not create an entry for this object";
#endif
          return false;
        }
        objl = data_.at(arg_idx);
        if(NULL == objl)
        {
#ifdef DEBUG
          std::cout<<"\nCObjectHistory::addObject() : Created an entry for this object but can't access it. Invalid state.";
#endif
          return false;
        }
      }

      flag = objl->create(CSystemClock::getSysTime(), arg_obj);//Stores objects at the start
      if(false == flag)
      {
#ifdef DEBUG
        std::cout<<"\nCObjectHistory::addObject() : Could not timestamp and store this object";
#endif
        return false;
      }

      return true;
    }

    /** Retrieves an object's entire stored history */
    const CMappedList<double, StoreObjectsAs>*
    getObjectTimeSeries(const Idx& arg_idx)
    { return data_.at(arg_idx); }

    /** Retrieves an object from a point in its stored history */
    const StoreObjectsAs* getObject(const Idx& arg_idx,
        /** Optional timestamp argument. Ignore this to get the last saved state. */
        double arg_time = -1.0)
    {
      CMappedList<double, StoreObjectsAs>* objl = data_.at(arg_idx);
      if(NULL == objl)
      {
#ifdef DEBUG
        std::cout<<"\nCObjectHistory::getObject() : Can't find object in history.";
#endif
        return NULL;
      }
      //Now check for the time
      if(arg_time > 0.0)
      { return objl->at(arg_time); }
      else
      { return objl->at( std::size_t(0) );  }
    }

    /** Pointer deletion requires knowing the type. The Idx
     * and StoreObjectsAsdestructors should work. */
    bool removeObjectTimeSeries(const Idx& arg_idx)
    { return data_.erase(arg_idx); }

    /** Pointer deletion requires knowing the type. */
    bool removeObject(const Idx& arg_idx,
        /** Optional timestamp argument. Ignore this to remove the last saved state. */
        double arg_time = -1)
    {
      CMappedList<double, StoreObjectsAs>* objl = data_.at(arg_idx);
      if(NULL == objl)
      {
#ifdef DEBUG
        std::cout<<"\nCObjectHistory::removeObject() : Can't find object in history.";
#endif
        return NULL;
      }
      //Now check for the time
      if(arg_time > 0.0)
      { return objl->erase(arg_time); }
      else
      { return objl->erase( objl->at(std::size_t(0)) );  }
    }

    /** Constructor : Does nothing. */
    CObjectHistory(){}

    /** Destructor : Does nothing. */
    ~CObjectHistory(){}
  };

} /* namespace sutil */
#endif /* COBJECTHISTORY_HPP_ */

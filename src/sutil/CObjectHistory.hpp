/* This file is part of scl, a control and simulation library
for robots and biomechanical models.

scl is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Alternatively, you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

scl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License and a copy of the GNU General Public License along with
scl. If not, see <http://www.gnu.org/licenses/>.
 */
/* \file  CObjectHistory.hpp
 *
 *  Created on: Nov 18, 2011
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#ifndef COBJECTHISTORY_HPP_
#define COBJECTHISTORY_HPP_

#include <sutil/CMappedList.hpp>
#include <sutil/CSystemClock.hpp>

namespace sutil
{
  namespace cobjecthistory
  { class CObjectHistoryBase;  }

  /** This class ensures that multiple types can be inserted into the object
   * store.
   *
   * NOTE : You are responsible to make sure that this doesn't segfault!
   */
  template<typename StoredObjectType>
  class CObjectHistory : public cobjecthistory::CObjectHistoryBase
  {
  public:
    /** Saves the current state of an object (can be retrieved later) */
    bool saveObject(const std::string& arg_name, const StoredObjectType& arg_obj)
    {
      StoredObjectType* tobj = new StoredObjectType(arg_obj);
      bool flag = saveObject(arg_name, reinterpret_cast<const void*>( tobj ));
      if(false == flag)
      {
        delete tobj;
#ifdef DEBUG
            std::cout<<"\nCObjectHistoryBase::saveObject() : Could not create an entry for this object";
#endif
        return false;
      }
      return true;
    }

    /** Retrieves an object's entire stored history.
     *
     * NOTE : You are responsible to make sure that this doesn't segfault! */
    const CMappedPointerList<double, StoredObjectType, false>*
    getObjectTimeSeries(const std::string& arg_name)
    { return reinterpret_cast<CMappedPointerList<double, StoredObjectType, false>*>(
        CObjectHistoryBase::getObjectTimeSeries(arg_name)); }

    /** Retrieves an object from a point in its stored history
     *
     * NOTE : You are responsible to make sure that this doesn't segfault! */
    const void* getObject(const std::string& arg_name,
        /** Optional timestamp argument. Ignore this to get the last saved state. */
        double arg_time = -1.0)
    { return reinterpret_cast<StoredObjectType*>(
        STimeStampedObjects::getData()->data_.at(arg_name)); }

    /** Pointer deletion requires knowing the type.
     *
     * NOTE : You are responsible to make sure that this doesn't segfault! */
    virtual const bool removeObjectTimeSeries(const std::string& arg_name)=0;

    /** Pointer deletion requires knowing the type.
     *
     * NOTE : You are responsible to make sure that this doesn't segfault! */
    virtual const bool removeObject(const std::string& arg_name,
        /** Optional timestamp argument. Ignore this to remove the last saved state. */
        double arg_time = -1)=0;

  private:
    /** Private constructors : Use the static methods. */
    CObjectHistoryBase(){}
    virtual ~CObjectHistoryBase(){}
    CObjectHistoryBase(const CObjectHistoryBase&){}
    CObjectHistoryBase& operator =(const CObjectHistoryBase&){}
  };

  namespace cobjecthistory
  {
    /** Time stamps an object, copies it and saves it.
     * The object may be restored at any future point of time. */
    class CObjectHistoryBase
    {
    protected:
      /** Index all stored objects by name and timestamp in a two
       * level mapped list. */
      class SObjectStore
      {
      public:
        /** Each stored object can be accessed by name */
        CMappedList<std::string,
          /** For each logged object, store a mapped list indexed
           * by the timestamp at which the object was logged. */
          CMappedPointerList<double, void, false> > data_;
      };

      /** A data store of all stored and timestamped objects */
      typedef CSingleton<SObjectStore> STimeStampedObjects;

      /** Saves the current state of an object (can be retrieved later) */
      bool saveObject(const std::string& arg_name, const void * arg_obj)
      {
        bool flag
        CMappedPointerList<double, void>* pl;
        pl = STimeStampedObjects::getData()->data_.at(arg_name);
        if(NULL == pl)
        {
          flag = STimeStampedObjects::getData()->data_.create(arg_name);
          if(false == flag)
          {
#ifdef DEBUG
            std::cout<<"\nCObjectHistoryBase::addObject() : Could not create an entry for this object";
#endif
            return false;
          }
          pl = STimeStampedObjects::getData()->data_.at(arg_name);
          if(NULL == pl)
          {
#ifdef DEBUG
            std::cout<<"\nCObjectHistoryBase::addObject() : Created an entry for this object but can't access it. Invalid state.";
#endif
            return false;
          }
        }

        flag = pl->create(CSystemClock::getSysTime(), arg_obj);
        if(false == flag)
        {
#ifdef DEBUG
          std::cout<<"\nCObjectHistoryBase::addObject() : Could not timestamp and store this object";
#endif
          return false;
        }

        return true;
      }

      /** Retrieves an object's entire stored history */
      const CMappedPointerList<double, void, false>*
      getObjectTimeSeries(const std::string& arg_name)
      { return STimeStampedObjects::getData()->data_.at(arg_name); }

      /** Retrieves an object from a point in its stored history */
      const void* getObject(const std::string& arg_name,
          /** Optional timestamp argument. Ignore this to get the last saved state. */
          double arg_time = -1.0)
      { return STimeStampedObjects::getData()->data_.at(arg_name); }

      /** Pointer deletion requires knowing the type. Must be */
      virtual const bool removeObjectTimeSeries(const std::string& arg_name)=0;

      /** Pointer deletion requires knowing the type. */
      virtual const bool removeObject(const std::string& arg_name,
          /** Optional timestamp argument. Ignore this to remove the last saved state. */
          double arg_time = -1)=0;

      /** Protected constructor : To ensure only subclasses may call it. */
      CObjectHistoryBase(){}

    private:
      /** Private constructor : To ensure uniqueness */
      CObjectHistoryBase(const CObjectHistoryBase&){}

      /** Private operator : To ensure uniqueness */
      CObjectHistoryBase& operator =(const CObjectHistoryBase&){}
    };
  }

} /* namespace sutil */
#endif /* COBJECTHISTORY_HPP_ */

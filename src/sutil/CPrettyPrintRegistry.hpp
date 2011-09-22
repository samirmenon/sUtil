/* This file is part of sUtil, a random collection of utilities.

sUtil is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Alternatively, you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

sUtil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License and a copy of the GNU General Public License along with
sUtil. If not, see <http://www.gnu.org/licenses/>.
 */
/* \file CPrettyPrintRegistry.hpp
 *
 *  Created on: Sep 20, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CPRETTYPRINTREGISTRY_HPP_
#define CPRETTYPRINTREGISTRY_HPP_

#include <sutil/CMappedList.hpp>
#include <sutil/CSingleton.hpp>

#include <string>

namespace sutil
{
  /** To print any arbitrary type's contents, you have to do two things:
   * 1. Define a function that parses the object's data into
   *    an ostream
   * 2. Add the printable subclass of SPrintable to the print
   *    object map
   *
   * To make your life easy, we have a template that does most
   * of the work for you.
   *
   * 1. Simply code this function somewhere in your callbacks:
   *
   *     template <>
   *     void printToStream<TYPE>(
   *        std::ostream& ostr, const TYPE& arg_data)
   *      { ostr<<arg_data.whatever_you_want_; }
   *
   * 2. Add these lines of code in your callbacks:
   *
   *     CPrettyPrintRegistry::add(rob.name_, SPrintable<TYPE>(your_object));
   */
  template<typename T> void printToStream(std::ostream&, const T&);

  /** A generic printable object. All database objects should
   * inherit from this. */
  class SPrintableBase
  {
  public:
    /** Default destructor. Does nothing. */
    virtual ~SPrintableBase(){}

    virtual void printDataToStream(std::ostream& arg_stream) const =0;

    virtual SPrintableBase* createObject() const =0;

    //The only way you can overload << (it is a global function)
    friend std::ostream& operator<<(std::ostream& outstr, const SPrintableBase& me);
  };

  std::ostream& operator<<(std::ostream& out, const SPrintableBase& me)
  { me.printDataToStream(out); return out; }

  template<typename T>
  class SPrintable : public SPrintableBase
  {
  public:
    SPrintable(const T& arg_data) : data_(arg_data) {}

    virtual void printDataToStream(std::ostream& ostr) const
    { printToStream<T>(ostr, data_); }

    virtual SPrintableBase* createObject() const
    { return new SPrintable<T>(data_); }

    const T& data_;

  private: //To enforce implementers to pass an object (which is then registered)
    SPrintable();
    SPrintable(const SPrintable&);
    SPrintable& operator=(const SPrintable&);
  };


  /** This maintains a map of all printable objects. Anything returned
   * by this map can be passed to an ostream and will print its contents
   * into the stream
   *
   * Eg. One such call could be:
   * if(0!=CPrettyPrintRegistry::get(std::string("YourObject")))
   *   std::cout<<*CPrettyPrintRegistry::get(std::string("YourObject"));
   */
  class  CPrettyPrintRegistry : private
  sutil::CSingleton<CMappedPointerList<std::string,SPrintableBase,true> >
  {typedef sutil::CSingleton<CMappedPointerList<std::string,SPrintableBase,true> > singleton;
  public:
  /** Gets a pointer to the database data structure */
  static const SPrintableBase* get(const std::string& arg_name)
  {
    SPrintableBase** ret = singleton::getData()->at(arg_name);
    if(NULL == ret) { return NULL;  }
    return static_cast<const SPrintableBase*>(*ret);
  }

  /** Adds an object to the lookup */
  static bool add(const std::string& arg_name,
      const SPrintableBase& arg_obj)
  {
    SPrintableBase** obj = singleton::getData()->create(arg_name);
    if(NULL == obj){  return false; }
    *obj = arg_obj.createObject();
    return true;
  }

  /** Deletes the singleton object and creates a new one
   * in its stead */
  static bool reset()
  { return singleton::resetData(); }

  private:
  /** Private constructor : for the singleton */
  CPrettyPrintRegistry();
  /** Private constructor : for the singleton */
  CPrettyPrintRegistry(const CPrettyPrintRegistry&);
  /** Private operator : for the singleton */
  CPrettyPrintRegistry& operator= (const CPrettyPrintRegistry&);
  };
}

#endif /* CPRETTYPRINTREGISTRY_HPP_ */

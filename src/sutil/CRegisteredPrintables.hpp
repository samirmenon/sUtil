/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CRegisteredPrintables.hpp
 *
 *  Created on: Sep 20, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CREGISTEREDPRINTABLES_HPP_
#define CREGISTEREDPRINTABLES_HPP_

#include <sutil/CMappedList.hpp>
#include <sutil/CSingleton.hpp>

#include <string>

namespace sutil
{
  /** You must define this function somewhere for each type that
   *  you want to support for printing */
  template<typename ObjectType>
  void printToStream(std::ostream&, const ObjectType&);

  // Forward declarations. Anything that subclasses this is printable on
  // stl streams. Ie. Supports the 'stream<<object' operator style.
  class SPrintableBase;

  // This helps the map to work properly through typical object orientation.
  // And saves you from writing redundant code for every type.
  template<typename ObjectType> class SPrintable;

  /** This maintains a map of all printable objects.
   * Although you can directly access it, it is best to
   * ignore it and use the helper functions in the "printable"
   * namespace instead. */
  typedef sutil::CSingleton<CMappedPointerList<std::string,SPrintableBase,true> > CRegisteredPrintables;

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
   *     namespace sutil //Remember to do this.
   *     {
   *       template <>
   *       void printToStream<TYPE>(
   *          std::ostream& ostr, const TYPE& arg_data)
   *        { ostr<<arg_data.whatever_you_want_; }
   *     }
   *
   * 2. Add these lines of code in your callbacks:
   *
   *     printables::add<TYPE>("object_name", object);
   */
  namespace printables
  {
    /** Enables access to the map of printable objects. An object in
     * the map can be passed to an ostream, and will print its contents
     * into the stream
     *
     * Eg. One such call could be:
     * if(0!=printables::get(std::string("YourObject")))
     *   std::cout<<*printables::get(std::string("YourObject"));
     */
    inline const SPrintableBase* get(const std::string& arg_name)
    {
      SPrintableBase** ret = CRegisteredPrintables::getData()->at(arg_name);
      if(NULL == ret) { return NULL;  }
      return static_cast<const SPrintableBase*>(*ret);
    }

    /** Enables adding an object to the map of printable objects. An object in
     * the map can be passed to an ostream, and will print its contents
     * into the stream */
    template<typename ObjectType>
    bool add(const std::string& arg_name,
        const ObjectType& arg_obj)
    {
      SPrintable<ObjectType> tmp_printable_object(arg_obj);
      SPrintableBase** obj = CRegisteredPrintables::getData()->create(arg_name);
      if(NULL == obj){  return false; }
      *obj = tmp_printable_object.createObject();
      return true;
    }

    /** Deletes the all registered printable object definitions.
     * Resets the singleton map that stores them. */
    inline bool reset() { return CRegisteredPrintables::resetData(); }
  }

  /** A generic printable object. All database objects should
   * inherit from this.
   *
   * Anything that subclasses this is printable on stl streams.
   * Ie. This class supports the 'stream<<object' operator style.*/
  class SPrintableBase
  {
  public:
    /** Default destructor. Does nothing. */
    virtual ~SPrintableBase(){}

    virtual void printDataToStream(std::ostream& arg_stream) const =0;

    virtual SPrintableBase* createObject() const =0;

    //The only way you can overload << (it is a global function)
    friend std::ostream& operator<<(std::ostream& outstr, const SPrintableBase& me);

  protected:
    SPrintableBase(){}
  private:
    SPrintableBase(const SPrintableBase&);
    SPrintableBase& operator =(const SPrintableBase&);
  };

  /** Enables printing SPrintableBase objects */
  inline std::ostream& operator<<(std::ostream& out, const SPrintableBase& me)
  { me.printDataToStream(out); return out; }

  /** This helps the map to work properly (through typical object
   * orientation).
   * It also saves you from writing redundant code for every type.
   *
   * This class will be automatically generated for a type when you
   * implement the:
   *      printToStream<Type>(std::outstream&, Type&)
   * function. Do so for every printable type you want to support. */
  template<typename ObjectType>
  class SPrintable : public SPrintableBase
  {
  public:
    SPrintable(const ObjectType& arg_data) :
      SPrintableBase(), data_(arg_data) {}

    virtual void printDataToStream(std::ostream& ostr) const
    { printToStream<ObjectType>(ostr, data_); }

    virtual SPrintableBase* createObject() const
    { return new SPrintable<ObjectType>(data_); }

    const ObjectType& data_;

  private: //To enforce implementers to pass an object (which is then registered)
    SPrintable();
    SPrintable(const SPrintable&);
    SPrintable& operator=(const SPrintable&);
  };
}

#endif /* CREGISTEREDPRINTABLES_HPP_ */

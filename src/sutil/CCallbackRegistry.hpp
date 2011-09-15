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
/* \file CCallbackRegistry.hpp
 *
 *  Created on: Sep 13, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CCALLBACKREGISTRY_HPP_
#define CCALLBACKREGISTRY_HPP_

#include <sutil/CSingleton.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  /*
   *
   * Usually this will be:
   * std::tuple<argument_type1, argument_type2, ... argument_typeN>
   *
   * Er. std::tuple<int, double, std::string>
   *
   * Allows automatic code generation for callbacks with
   * a variety of arguments.
      typename ArgumentTuple,
      typename Data,
      typename ReturnType
   */

  // These are forward declarations. Read the class comments@definition.
  template <typename Idx> class CCallbackSuperBase;
  template <typename Idx, typename ArgumentTuple> class CCallbackBase;

  /** This class implements a callback factory singleton. In plain English,
   * it is a one-of-a-kind class that can give you an function based on
   * its name.
   *
   * Example usage:
   *  CCallbackRegistry<string>::call<tuple<int, double, string> >(string s,
   *        tuple<int, double, string> t);
   *
   * Or more concretely:
   *
   * class CCallbackBoboFunc : public CCallbackBase<string, tuple<int, double, string> >
   * {
   * public:
   *  CCallbackBoboFunc(const Idx &arg_callback_name) :
   *    CCallbackBase<string, tuple<int,double,string> >::CCallbackBase(arg_callback_name)
   *    { }
   *
   *  virtual void call(ArgumentTuple& args)
   *  { cout<<"\nThe args are: "<<get<0>(args)<<", "<<get<1>(args)<<", "<<get<2>(args); }
   *
   *  virtual CCallbackBase<Idx, ArgumentTuple>* createObject()
   *  { return dynamic_cast<CCallbackBase<Idx, ArgumentTuple>*>(new CCallbackBoboFunc()); }
   * };
   *
   * string s("BoboFunc);
   * CCallbackBoboFunc f("BoboFunc");
   *
   * CCallbackRegistry<string>::register<tuple<int, double, string> >(string s,
   *        tuple<int, double, string> t);
   * */
  template <typename Idx>
  class CCallbackRegistry : private CSingleton<CMappedPointerList<Idx,CCallbackSuperBase<Idx> > >
  {
    //A typedef for easy use;
    typedef CSingleton<CMappedPointerList<Idx,CCallbackSuperBase<Idx> > > singleton;

    //So that the base class can call the register function
    friend class CCallbackSuperBase<Idx>;

  public:
    /** Checks whether this callback has been registered with the factory */
    static bool callbackRegistered(const Idx& arg_callback_name)
    {
      CCallbackSuperBase<Idx>** mapped_callback = singleton::getData()->at(arg_callback_name);
      if(0 == mapped_callback) { return false; }
      return true;
    }

    /** Deletes the singleton object and creates a new one
     * in its stead */
    static bool resetCallbacks()
    { return singleton::resetData(); }

    static CMappedPointerList<Idx,CCallbackSuperBase<Idx> >*getCallbacks()
    { return singleton::getData();  }

  private:
    /** This function registers new dynamic callbacks with the factory.
     * You can get objects of this callback by calling the
     * createObjectForCallback function */
    static bool registerCallback(const Idx& arg_callback_name,
        CCallbackSuperBase<Idx>* arg_callback_object)
    {
      if(callbackRegistered(arg_callback_name))
      {
#ifdef DEBUG
        std::cerr<<"\nCCallbackRegistry::registerCallback() Warning :"
            <<" The passed callback is already registered.";
#endif
        return false;
      }

      // Creates an object that points to the passed callback object
      CCallbackSuperBase<Idx>** t = singleton::getData()->
          create(arg_callback_name,arg_callback_object);
      if(0 == t)
      {
#ifdef DEBUG
        std::cerr<<"\nCCallbackRegistry::registerCallback() Error :"
            <<" Failed to create callback in the callback mapped list.";
#endif
        return false;
      }

      return true;
    }

    /** Private for the singleton */
    CCallbackRegistry();

    /** Private for the singleton */
    CCallbackRegistry(const CCallbackRegistry&);

    /** Private for the singleton */
    CCallbackRegistry& operator= (const CCallbackRegistry&);
  };

  template<typename Idx, typename ArgumentTuple>
  class CCallbacks
  {
  public:
    /** This function returns an indexed callback (if the
     * callback has already been registered with the singleton) */
    static void call(const Idx& arg_callback_name, ArgumentTuple& args)
    {
      CCallbackSuperBase<Idx>** mapped_callback =
          CCallbackRegistry<Idx>::getCallbacks()->at(arg_callback_name);

      CCallbackBase<Idx, ArgumentTuple>* callback =
          dynamic_cast<CCallbackBase<Idx, ArgumentTuple>*>(*mapped_callback);

      callback->call(args);
    }
  };

  /** *********************************************************
   * To support indexed querying for dynamic object allocation:
   * Subclass CCallbackSuperBase and implement the createObject()
   * function to return an object of the wanted callback.
   * ********************************************************* */
  template <typename Idx>
  class CCallbackSuperBase
  {
  public:
    /** Must name a callback while creating an object */
    CCallbackSuperBase(const Idx &arg_callback_name) : callback_name_(arg_callback_name){}

    /** Default Destructor : Does nothing */
    virtual ~CCallbackSuperBase(){}

  protected:
    virtual bool registerMyCallback(CCallbackSuperBase* arg_obj)
    { return CCallbackRegistry<Idx>::registerCallback(callback_name_,arg_obj); }

    /** The callback of the object */
    Idx callback_name_;

  private:
    /** Must name a callback while creating an object */
    CCallbackSuperBase();
    /** Must name a callback while creating an object */
    CCallbackSuperBase(const CCallbackSuperBase&);
    /** Must name a callback while creating an object */
    CCallbackSuperBase& operator= (const CCallbackSuperBase&);
  };

  /** *********************************************************
   * This enables supporting dynamic typing for arbitrary
   * index and object callbacks.
   * ********************************************************* */
  template <typename Idx, typename ArgumentTuple>
  class CCallbackBase : public CCallbackSuperBase<Idx>
  {
  public:
    /** Must name a callback while creating an object */
    CCallbackBase(const Idx &arg_callback_name) :
      CCallbackSuperBase<Idx>::CCallbackSuperBase(arg_callback_name)
      { }

    /** Default Destructor : Does nothing */
    virtual ~CCallbackBase(){}

    /** A subclass must implement this function.
     * You can choose to add a "return type" into
     * the ArgumentTuple and get data from the function. */
    virtual void call(ArgumentTuple& args) = 0;

    /** A subclass must implement this function */
    virtual CCallbackBase<Idx, ArgumentTuple>* createObject()=0;

    virtual bool registerCallback()
    {
      bool flag;
      flag = CCallbackRegistry<Idx>::callbackRegistered(
          CCallbackSuperBase<Idx>::callback_name_);

      if(flag)//Callback already registered. Do nothing and return false.
      { return false; }
      else
      {
        //Duplicate this object and register the new one with the singleton
        CCallbackBase<Idx,ArgumentTuple>* obj = createObject();

        flag = CCallbackSuperBase<Idx>::registerMyCallback(
            dynamic_cast<CCallbackSuperBase<Idx>*>(obj) );
        if(!flag)
        { delete obj; return false; }
      }
      return true;
    }

  private:
    /** Must name a callback while creating an object */
    CCallbackBase();
    /** Must name a callback while creating an object */
    CCallbackBase(const CCallbackBase&);
    /** Must name a callback while creating an object */
    CCallbackBase& operator= (const CCallbackBase&);
  };
}

#endif /* CCALLBACKREGISTRY_HPP_ */

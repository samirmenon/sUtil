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
/* \file CRegisteredCallbacks.hpp
 *
 *  Created on: Sep 13, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CREGISTEREDCALLBACKS_HPP_
#define CREGISTEREDCALLBACKS_HPP_

#include <sutil/CSingleton.hpp>
#include <sutil/CMappedList.hpp>

#include <vector>
#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  // These are forward declarations. Read the class comments for details.
  template <typename Idx> class CCallbackSuperBase;
  template <typename Idx, typename ArgumentTuple, typename Data> class CCallbackBase;
  template <typename Idx> class CRegisteredCallbacks;

  /** Use this namespace to create and call callback
   * functions
   *
   * Example usage:
   *
   *  // 1. Defining a callback function class
   *
   *  // NOTE: You can also use typename Data to
   *  // customize different objects of this class and then
   *  // register them.
   *  class CCallbackFunc : public
   *  sutil::CCallbackBase<std::string, std::vector<std::string> >
   *  {
   *    typedef sutil::CCallbackBase<std::string, std::vector<std::string> > base;
   *  public:
   *    virtual void call(std::vector<std::string>& args)
   *    {//Do something with the argument
   *      std::vector<std::string>::iterator it,ite;
   *      for(it = args.begin(), ite = args.end(); it!=ite; ++it)
   *      {  std::cout<<"\nPassed arg : "<<*it;  }
   *
   *      //And/or copy the result into it
   *      std::string x;
   *      std::cin>>x;
   *      args.push_back(x);
   *    }
   *
   *    virtual base* createObject()
   *    { return dynamic_cast<base*>(new CCallbackFunc()); }
   *  };
   *
   *  //2. Register the function
   *  std::string name("BoboFunc");
   *  std::vector<std::string> args;
   *
   *  flag = sutil::callbacks::add<CCallbackFunc,
   *  std::string, std::vector<std::string> >(name);
   *
   *  //3. Call the function
   *  sutil::callbacks::call<std::string,std::vector<std::string> >(callback_name,args);
   */
  namespace callbacks
  {
    /** This function returns an indexed callback (if the
     * callback has already been registered with the singleton) */
    template<typename Idx, typename ArgumentTuple, typename Data=bool >
    bool call(const Idx& arg_callback_name, ArgumentTuple& args)
    {
      CCallbackSuperBase<Idx>** mapped_callback =
          CRegisteredCallbacks<Idx>::getCallbacks()->at(arg_callback_name);

      if(0 == mapped_callback) {  return false; } //Function not found.

      CCallbackBase<Idx, ArgumentTuple, Data>* callback =
          dynamic_cast<CCallbackBase<Idx, ArgumentTuple, Data>*>(*mapped_callback);

      if(NULL == callback)
      { return false; }

      callback->call(args); //The actual function call

      return true; //Everything worked out
    }

    /** Why do we allow specifying Data?
     * Ans: Well a class might automatically and uniquely
     * initialize the Data in the constructor */
    template< typename CallbackClass, typename Idx,
    typename ArgumentTuple, typename Data=bool >
    bool add(const Idx& arg_callback_name)
    {
      CallbackClass f;
      return f.sutil::CCallbackBase<Idx, ArgumentTuple, Data>::
      registerCallback(arg_callback_name);
    }

    template< typename CallbackClass, typename Idx,
    typename ArgumentTuple, typename Data>
    bool add(const Idx& arg_callback_name, Data* arg_data)
    {
      CallbackClass f;
      return f.sutil::CCallbackBase<Idx, ArgumentTuple, Data>::
          registerCallback(arg_callback_name, arg_data);
    }

    /** Return a list of the registered callbacks (a vector of indices) */
    template<typename Idx>
    bool list(std::vector<Idx>& idxlist)
    { typedef CMappedPointerList<Idx,CCallbackSuperBase<Idx>,true> map;
      map* m = CRegisteredCallbacks<Idx>::getCallbacks();
      if(NULL == m) { return false; }
      typename map::iterator it,ite;
      idxlist.clear();
      for(it = m->begin(), ite = m->end();it!=ite;++it)
      { idxlist.push_back(!it); }//Add the index to the vector
      return true;
    }
  }

  /** This class implements a callback factory singleton. In plain English,
   * it is a one-of-a-kind class that can give you an function based on
   * its name.
   *
   * NOTE: The singleton manages its memory and deletes all the pointers. */
  template <typename Idx>
  class CRegisteredCallbacks : private CSingleton<CMappedPointerList<Idx,CCallbackSuperBase<Idx>,true> >
  {
    //A typedef for easy use;
    typedef CSingleton<CMappedPointerList<Idx,CCallbackSuperBase<Idx>,true> > singleton;

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

    /** Used to access the callback list */
    static CMappedPointerList<Idx,CCallbackSuperBase<Idx>,true>* getCallbacks()
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
        std::cerr<<"\nCRegisteredCallbacks::registerCallback() Warning :"
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
        std::cerr<<"\nCRegisteredCallbacks::registerCallback() Error :"
            <<" Failed to create callback in the callback mapped list.";
#endif
        return false;
      }

      return true;
    }

    /** Private for the singleton */
    CRegisteredCallbacks();

    /** Private for the singleton */
    CRegisteredCallbacks(const CRegisteredCallbacks&);

    /** Private for the singleton */
    CRegisteredCallbacks& operator= (const CRegisteredCallbacks&);
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
    /** Default Destructor : Does nothing */
    virtual ~CCallbackSuperBase(){}

  protected:
    /** The constructor may only be called by a subclass */
    CCallbackSuperBase(){}

    /** To allow the callback registry to create objects for itself */
    virtual bool registerCallbackSuper(
        const Idx &arg_callback_name,
        CCallbackSuperBase* arg_obj)
    {
      return CRegisteredCallbacks<Idx>::
          registerCallback(arg_callback_name,arg_obj);
    }

  private:
    /** Must name a callback while creating an object */
    CCallbackSuperBase(const CCallbackSuperBase&);
    /** Must name a callback while creating an object */
    CCallbackSuperBase& operator= (const CCallbackSuperBase&);
  };

  /** *********************************************************
   * This enables supporting dynamic typing for arbitrary
   * index and object callbacks.
   * ********************************************************* */
  template <typename Idx, typename ArgumentTuple, typename Data=bool>
  class CCallbackBase : public CCallbackSuperBase<Idx>
  {
  public:
    /** A subclass must implement this function.
     * You can choose to add a "return type" into
     * the ArgumentTuple and get data from the function. */
    virtual void call(ArgumentTuple& args) = 0;

    /** A subclass must implement this function */
    virtual CCallbackBase<Idx, ArgumentTuple, Data>* createObject()=0;

    virtual bool registerCallback(
        const Idx& arg_callback_name,
        Data* arg_data = 0)
    {
      bool flag;
      flag = CRegisteredCallbacks<Idx>::callbackRegistered(arg_callback_name);

      if(flag)//Callback already registered. Do nothing and return false.
      { return false; }
      else
      {
        //Duplicate this object and register the new one with the singleton
        CCallbackBase<Idx,ArgumentTuple,Data>* obj = createObject();

        //Set up data for this function object
        if(0 != arg_data)
        { obj->data_ = arg_data; }

        flag = CCallbackSuperBase<Idx>::registerCallbackSuper(
            arg_callback_name,
            dynamic_cast<CCallbackSuperBase<Idx>*>(obj) );
        if(!flag)
        { delete obj; return false; }
      }
      return true;
    }

    /** Default Destructor : Does nothing */
    virtual ~CCallbackBase(){}

  protected:
    /** Only a subclass may create an object of this type */
    CCallbackBase() : data_(NULL){}

    Data* data_;

  private:
    CCallbackBase(const CCallbackBase&);
    CCallbackBase& operator= (const CCallbackBase&);
  };
}

#endif /* CREGISTEREDCALLBACKS_HPP_ */

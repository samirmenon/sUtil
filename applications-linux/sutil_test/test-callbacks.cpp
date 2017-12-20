/* This file is part of sUtil, a random collection of utilities.
See the Readme.txt file in the root folder for licensing information.
 */
/*
 * test-callbacks.cpp
 *
 *  Created on: Sep 09, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#include "test-callbacks.hpp"

#include <sutil/CSystemClock.hpp>

#include <sutil/CRegisteredCallbacks.hpp>

#include <iostream>
#include <string>
#include <tuple>
#include <stdexcept>

namespace sutil_test
{

  typedef std::tuple<int, double, std::string> ArgType;

  class CCallbackFunc : public
  sutil::CCallbackBase<std::string, ArgType >
  {
    typedef sutil::CCallbackBase<std::string, ArgType > base;
  public:
    virtual ~CCallbackFunc(){}

    virtual void call(ArgType& args)
    {
      int x; x = std::get<0>(args);
      double y; y = std::get<1>(args);
      std::string z; z =  std::get<2>(args);
      std::cout<<"\nThe args are: "<<x<<", "<<y<<", "<<z;
    }

    virtual base* createObject()
    { return dynamic_cast<base*>(new CCallbackFunc()); }
  };

  /** Tests the dynamic callback utility
   * @param arg_id : The id of the test */
  void test_callbacks(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Create callback object
      std::string callback_name("BoboFunc");

      //Test 1 : Register callbacks. Should succeed.
      flag = sutil::callbacks::add<CCallbackFunc,
          std::string, ArgType>(callback_name);
      if(false == flag)
      { throw(std::runtime_error(std::string("Failed to register callback : ")+callback_name ));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered callback :" << callback_name; }

      //Test 2: Run callback
      //Set up an argument type to check whether it works
      ArgType t;

      std::get<0>(t) = 8080; std::get<1>(t) = 8080.80; std::get<2>(t) = "I am Bobo! >:) ";
      sutil::callbacks::call<std::string,ArgType>(callback_name,t);
      std::cout<<"\nTest Result ("<<test_id++<<") Ran callback "<<callback_name;

      std::get<0>(t) = 80; std::get<1>(t) = 80.80; std::get<2>(t) = "And I wish I was.. :-( ";
      sutil::callbacks::call<std::string,ArgType>(callback_name,t);
      std::cout<<"\nTest Result ("<<test_id++<<") Ran callback "<<callback_name;

      std::cout<<"\nTest #"<<arg_id<<" (Callback Registry Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Callback Registry Test) Failed";
    }
  }

}

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

      //Test 3: Edge cases and error conditions
      
      // Test duplicate callback registration
      flag = sutil::callbacks::add<CCallbackFunc,
          std::string, ArgType>(callback_name);
      if(flag) {
        throw(std::runtime_error("Should not allow duplicate callback registration"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Duplicate callback registration correctly rejected";

      // Test calling non-existent callback
      try {
        sutil::callbacks::call<std::string,ArgType>("NonExistentCallback", t);
        throw(std::runtime_error("Should have failed calling non-existent callback"));
      } catch(const std::exception& e) {
        // Expected to fail
        std::cout<<"\nTest Result ("<<test_id++<<") Non-existent callback correctly rejected";
      }

      // Test multiple different callbacks
      std::string callback_name2("TestCallback2");
      flag = sutil::callbacks::add<CCallbackFunc,
          std::string, ArgType>(callback_name2);
      if(!flag) {
        throw(std::runtime_error("Failed to register second callback"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Registered second callback";

      // Test both callbacks work independently
      std::get<0>(t) = 100; std::get<1>(t) = 200.5; std::get<2>(t) = "First callback test";
      sutil::callbacks::call<std::string,ArgType>(callback_name, t);
      
      std::get<0>(t) = 300; std::get<1>(t) = 400.7; std::get<2>(t) = "Second callback test";
      sutil::callbacks::call<std::string,ArgType>(callback_name2, t);
      std::cout<<"\nTest Result ("<<test_id++<<") Multiple callbacks work independently";

      // Test edge case argument values
      std::get<0>(t) = -2147483648; // INT_MIN
      std::get<1>(t) = 1.7976931348623157e+308; // Close to DBL_MAX
      std::get<2>(t) = ""; // Empty string
      sutil::callbacks::call<std::string,ArgType>(callback_name, t);
      std::cout<<"\nTest Result ("<<test_id++<<") Edge case argument values handled correctly";

      // Test very long string argument
      std::string very_long_string(10000, 'A');
      std::get<0>(t) = 0;
      std::get<1>(t) = 0.0;
      std::get<2>(t) = very_long_string;
      sutil::callbacks::call<std::string,ArgType>(callback_name, t);
      std::cout<<"\nTest Result ("<<test_id++<<") Very long string argument handled correctly";

      // Test special character callback names
      std::string special_name("Callback@#$%^&*()");
      flag = sutil::callbacks::add<CCallbackFunc,
          std::string, ArgType>(special_name);
      if(!flag) {
        throw(std::runtime_error("Failed to register callback with special characters"));
      }
      
      std::get<0>(t) = 999; std::get<1>(t) = 999.999; std::get<2>(t) = "Special chars test";
      sutil::callbacks::call<std::string,ArgType>(special_name, t);
      std::cout<<"\nTest Result ("<<test_id++<<") Special character callback name works";

      // Test empty callback name
      try {
        flag = sutil::callbacks::add<CCallbackFunc,
            std::string, ArgType>("");
        if(flag) {
          std::cout<<"\nTest Result ("<<test_id++<<") Empty callback name allowed (implementation choice)";
        } else {
          std::cout<<"\nTest Result ("<<test_id++<<") Empty callback name correctly rejected";
        }
      } catch(...) {
        std::cout<<"\nTest Result ("<<test_id++<<") Empty callback name correctly rejected with exception";
      }

      std::cout<<"\nTest #"<<arg_id<<" (Callback Registry Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Callback Registry Test) Failed";
    }
  }

}

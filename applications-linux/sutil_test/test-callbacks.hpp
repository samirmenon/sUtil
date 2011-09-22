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
/*
 * test-callbacks.hpp
 *
 *  Created on: Sep 09, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_CALLBACKS_HPP_
#define TEST_CALLBACKS_HPP_

#include <sutil/CSystemClock.hpp>

#include <sutil/CRegisteredCallbacks.hpp>

#include <iostream>
#include <string>
#include <tuple>

namespace sutil_test
{

  typedef std::tuple<int, double, std::string> ArgType;

  class CCallbackFunc : public
  sutil::CCallbackBase<std::string, ArgType >
  {
    typedef sutil::CCallbackBase<std::string, ArgType > base;
  public:
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
#endif /* TEST_CALLBACKS_HPP_ */

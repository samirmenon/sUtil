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

#include <sutil/CCallbackRegistry.hpp>

#include <iostream>
#include <string>
#include <tuple>

namespace sutil_test
{

  typedef std::tuple<int, double, std::string> ArgType;

  class CCallbackBoboFunc : public
  sutil::CCallbackBase<std::string, ArgType >
  {
    typedef sutil::CCallbackBase<std::string, ArgType > base;
  public:
    CCallbackBoboFunc(const std::string &arg_callback_name) :
      base::CCallbackBase(arg_callback_name)
      { }

    virtual
    void call(ArgType& args)
    {
      int x; x = std::get<0>(args);
      double y; y = std::get<1>(args);
      std::string z; z =  std::get<2>(args);
      std::cout<<"\nThe args are: "<<x<<", "<<y<<", "<<z;
    }

    virtual
    base* createObject()
    {
      std::string s("BoboFunc");
      return dynamic_cast<base*>(new CCallbackBoboFunc(s));
    }
  };

  /** Tests the dynamic callback utility
   * @param arg_id : The id of the test */
  void test_callbacks(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Initialize the buffers and the mem copier
      CCallbackBoboFunc f("BoboFunc");

      //sutil::CCallbackFactory<std::string>::
//      <tuple<int, double, string> >(string s,
//          tuple<int, double, string> t);

      //Test 1 : Register callbacks. Should succeed.
//      flag = type1.registerType();
      if(false == flag)
      { throw(std::runtime_error("Failed to register type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered type SType1"; }


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

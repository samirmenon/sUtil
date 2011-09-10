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
 * test-dyntypes.hpp
 *
 *  Created on: Sep 09, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_DYNTYPES_HPP_
#define TEST_DYNTYPES_HPP_

#include <sutil/CSystemClock.hpp>

#include <sutil/CDynamicTypeFactory.hpp>

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

namespace sutil_test
{
  struct SType1
  { int x; };

  struct SType2
  { int x, y; };

  struct SType3
  { int x, y, z; };

  /** Tests the dynamic typing utility
   * @param arg_id : The id of the test */
  void test_dyntypes(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Timers
      //double time1,time2;

      //Initialize the buffers and the mem copier
      sutil::CDynamicType<std::string,SType1> type1(std::string("SType1"));
      sutil::CDynamicType<std::string,SType2> type2(std::string("SType2"));
      sutil::CDynamicType<std::string,SType3> type3(std::string("SType3"));

      //Test 1 : Register types. Should succeed.
      flag = type1.registerType();
      if(false == flag)
      { throw(std::runtime_error("Failed to register type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered type SType1"; }

      flag = type2.registerType();
      if(false == flag)
      { throw(std::runtime_error("Failed to register type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered type SType2"; }

      flag = type3.registerType();
      if(false == flag)
      { throw(std::runtime_error("Failed to register type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered type SType3"; }

      //Test 2 : Register types again. Should fail.
      flag = type1.registerType();
      if(true == flag)
      { throw(std::runtime_error("Re-registered type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Didn't re-register type SType1"; }

      flag = type2.registerType();
      if(true == flag)
      { throw(std::runtime_error("Re-registered type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Didn't re-register type SType2"; }

      flag = type3.registerType();
      if(true == flag)
      { throw(std::runtime_error("Re-registered type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Didn't re-register type SType3"; }

      std::cout<<"\nTest #"<<arg_id<<" (Dynamic Typing Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Dynamic Typing Test) Failed";
    }
  }

}
#endif /* TEST_DYNTYPES_HPP_ */

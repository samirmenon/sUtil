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
/* \file test_main.cpp
 *
 *  Created on: Jul 7, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#include "test-singleton.hpp"
#include "test-pilemap.hpp"

#include <sutil/CSingleton.hpp>
#include <sutil/CSystemClock.hpp>

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace sutil;

/** This is a test application to test sutil's features.
 * It also serves as a tutorial to show you how to use the
 * features. */
int main(int argc, char** argv)
{
  bool flag;
  if(argc != 2)
  {
    int tid = 0;
    cout<<"\nThe command line input is: ./<executable> <test_number>";
    cout<<"\n"<<tid++<<" : Run all tests";
    cout<<"\n"<<tid++<<" : Run singleton tests";
    cout<<"\n"<<tid++<<" : Run pilemap tests";
    cout<<"\n";
  }
  else
  {
    unsigned int tid, id = 1;
    tid = static_cast<unsigned int>(atoi(argv[1]));
    cout<<"\nRunning sutil tests for case: "<<tid;

    flag = sutil::CSystemClock::start(); //NOTE: Clock MUST be created and destroyed in main
    if(false == flag)
    { throw(std::runtime_error("Could not start system clock"));  }

    cout<<"\nStarting tests. Time:"<<sutil::CSystemClock::getSysTime();

    if((tid==0)||(tid==id))
    {//Test clock
      std::cout<<"\n\nTest #"<<id<<". System Clock [Sys time, Sim time :"
          <<sutil::CSystemClock::getSysTime()
          <<" "
          <<sutil::CSystemClock::getSimTime()
          <<"]";
      sutil_test::test_singleton(id);
    }
    ++id;

    if((tid==0)||(tid==id))
    {//Test clock
      std::cout<<"\n\nTest #"<<id<<". System Clock [Sys time, Sim time :"
          <<sutil::CSystemClock::getSysTime()
      <<" "
      <<sutil::CSystemClock::getSimTime()
      <<"]";
      sutil_test::test_pilemap(id);
    }
    ++id;

    if((tid==0)||(tid==id))
    {//Test clock
      std::cout<<"\n\nTest #"<<id<<". System Clock [Sys time, Sim time :"
          <<sutil::CSystemClock::getSysTime()
      <<" "
      <<sutil::CSystemClock::getSimTime()
      <<"]";
      sutil_test::test_multi_pilemap(id);
    }
    ++id;

    cout<<"\n\nEnding tests. Time:"<<sutil::CSystemClock::getSysTime()<<"\n";
  }
  return 0;
}

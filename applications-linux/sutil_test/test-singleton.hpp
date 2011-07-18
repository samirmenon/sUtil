/* This file is part of SNeuron, a control and simulation library
for neural networks.

SNeuron is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Alternatively, you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

SNeuron is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License and a copy of the GNU General Public License along with
SNeuron. If not, see <http://www.gnu.org/licenses/>.
 */
/* \file singleton_tests.hpp
 *
 *  Created on: Jul 2, 2010
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_DATABASE_HPP_
#define TEST_DATABASE_HPP_

#include <sutil/CSingleton.hpp>
//#include <sneuron/util/CSystemClock.hpp>

#include <iostream>
#include <stdexcept>

namespace sutil_test
{
  /** Temporary struct to test the singleton */
  struct SSingletonTester
  {
    int x[10];
    double bobo;
    std::string lobo;

    SSingletonTester() : bobo(1.0), lobo("lobo"){}
  };

  /** Tests the Singleton implementation
   * @param arg_id : The id of the test */
  void test_singleton(const unsigned int arg_id)
  {
    //sneuron::snBool flag = true;
    unsigned int test_id=0;

    try
    {
      //Temporary typedef to refrain from typing this over and over
      typedef sutil::CSingleton<SSingletonTester> CDb;
      SSingletonTester *db, *db2; //To store pointers from the singleton

      db = CDb::getData();
      if(NULL == db) { throw(std::runtime_error("Singleton returned NULL after creation")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Tested that singleton doesn't return NULL"; }

      //Double check singleton mechanism
      db2 = CDb::getData();
      if(db2 != db) { throw(std::runtime_error("Singleton failed: Singleton pointer changed after creation")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Singleton pointer remains the same for multiple calls"; }

      std::cout<<"\nTest #"<<arg_id<<" (Singleton Test) Successful";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Singleton Test) Failed";
    }
  }

}


#endif /* TEST_DATABASE_HPP_ */

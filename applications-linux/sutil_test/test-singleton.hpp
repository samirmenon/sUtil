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

  /** Temporary struct to test the singleton */
  struct SSingletonTester2
  {
    int x[11];
    std::string bobo, lobo;

    SSingletonTester2() : bobo("bobo"), lobo("lobo"){}
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
      SSingletonTester *db_a, *db_b; //To store pointers from the singleton

      db_a = CDb::getData();
      if(NULL == db_a) { throw(std::runtime_error("Singleton returned NULL after creation")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Tested that singleton doesn't return NULL on data structure 1"; }

      //Double check singleton mechanism
      db_b = CDb::getData();
      if(db_b != db_a) { throw(std::runtime_error("Singleton failed: Singleton pointer changed after creation")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Singleton pointer remains the same for multiple calls on data structure 1"; }

      //Stress check the singleton with a different data structure
      typedef sutil::CSingleton<SSingletonTester2> CDb2;
      SSingletonTester2 *db2_a, *db2_b; //To store pointers from the singleton
      const unsigned int test_iters(1000);
      for(unsigned int i=0; i<test_iters; ++i)
      {
        db2_a = CDb2::getData();
        if(NULL == db2_a) { throw(std::runtime_error("Singleton returned NULL after creation on data structure 2")); }

        //Double check singleton mechanism
        db2_b = CDb2::getData();
        if(db2_b != db2_a) { throw(std::runtime_error("Singleton failed: Singleton pointer changed after creation on data structure 2")); }
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Singleton works for "<<test_iters<<" calls on data structure 2";

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

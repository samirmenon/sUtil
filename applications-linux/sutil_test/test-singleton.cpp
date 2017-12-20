/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file test-singleton.cpp
 *
 *  Created on: Jul 17, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#include "test-singleton.hpp"

#include <sutil/CSingleton.hpp>

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
    unsigned int test_id=0;
    bool flag;

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

      //Set some data, which will be wiped out by the reset
      db_a->lobo = "WipeThisStringOnReset";
      db_a->bobo = 8080.00;

      flag = CDb::resetData();
      if(false == flag) { throw(std::runtime_error("Singleton reset failed")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Reset the singleton"; }

      db_a = CDb::getData();
      if(static_cast<int>(db_a->bobo) == 8080)
      { throw(std::runtime_error("Singleton failed: Singleton's double data didn't reset")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Singleton's double data was reset successfully "; }

      if(db_a->lobo == std::string("WipeThisStringOnReset") )
      { throw(std::runtime_error("Singleton failed: Singleton's string data didn't reset")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Singleton's string data was reset successfully "; }

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

      std::cout<<"\nTest #"<<arg_id<<" (Singleton Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Singleton Test) Failed";
    }
  }

}

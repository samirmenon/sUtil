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

      //Test edge cases and potential problems
      
      //Test 6: Large object singleton
      typedef sutil::CSingleton<SSingletonTester> CDb;
      SSingletonTester* large_obj = CDb::getData();
      
      // Fill the object with test data
      for(int i = 0; i < 10; ++i) {
        large_obj->x[i] = i * 100;
      }
      large_obj->bobo = 123.456;
      large_obj->lobo = "Large object test data";
      
      // Verify data persistence
      SSingletonTester* large_obj2 = CDb::getData();
      if(large_obj != large_obj2) {
        throw(std::runtime_error("Large object singleton pointer changed"));
      }
      
      bool data_intact = true;
      for(int i = 0; i < 10; ++i) {
        if(large_obj2->x[i] != i * 100) {
          data_intact = false;
          break;
        }
      }
      if(!data_intact || large_obj2->bobo != 123.456 || large_obj2->lobo != "Large object test data") {
        throw(std::runtime_error("Large object singleton data corrupted"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Large object singleton maintains data integrity";

      //Test 7: Multiple reset operations
      for(int i = 0; i < 5; ++i) {
        flag = CDb::resetData();
        if(!flag) {
          throw(std::runtime_error("Multiple reset operations failed"));
        }
        SSingletonTester* reset_obj = CDb::getData();
        if(reset_obj->bobo != 1.0 || reset_obj->lobo != "lobo") {
          throw(std::runtime_error("Multiple reset operations corrupted default values"));
        }
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Multiple reset operations work correctly";

      //Test 8: Exception safety with normal objects
      struct NormalTester {
        int value;
        NormalTester() : value(42) {}
      };
      
      typedef sutil::CSingleton<NormalTester> CNormalDb;
      
      try {
        NormalTester* normal_obj = CNormalDb::getData();
        if(normal_obj == nullptr) {
          throw(std::runtime_error("Normal test object should not be null"));
        }
        if(normal_obj->value != 42) {
          throw(std::runtime_error("Normal test object not initialized correctly"));
        }
        std::cout<<"\nTest Result ("<<test_id++<<") Normal object singleton initialized correctly";
      } catch(...) {
        throw(std::runtime_error("Unexpected exception during normal construction"));
      }

      //Test 9: Test different template instantiations don't interfere
      typedef sutil::CSingleton<int> CIntSingleton;
      typedef sutil::CSingleton<double> CDoubleSingleton;
      typedef sutil::CSingleton<std::string> CStringSingleton;
      
      int* int_obj = CIntSingleton::getData();
      double* double_obj = CDoubleSingleton::getData();
      std::string* string_obj = CStringSingleton::getData();
      
      *int_obj = 42;
      *double_obj = 3.14159;
      *string_obj = "Template test";
      
      // Verify they don't interfere with each other
      if(*int_obj != 42 || *double_obj != 3.14159 || *string_obj != "Template test") {
        throw(std::runtime_error("Different template instantiations interfere with each other"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Different template instantiations work independently";

      //Test 10: Stress test with rapid creation/reset cycles
      const int stress_cycles = 100;
      for(int i = 0; i < stress_cycles; ++i) {
        SSingletonTester* stress_obj = CDb::getData();
        stress_obj->bobo = i * 1.5;
        stress_obj->lobo = "Stress test " + std::to_string(i);
        
        if(i % 10 == 0) {
          flag = CDb::resetData();
          if(!flag) {
            throw(std::runtime_error("Stress test reset failed at iteration " + std::to_string(i)));
          }
        }
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Stress test with "<<stress_cycles<<" cycles completed";

      std::cout<<"\nTest #"<<arg_id<<" (Singleton Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Singleton Test) Failed";
    }
  }

}

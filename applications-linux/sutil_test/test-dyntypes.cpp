/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/*
 * test-dyntypes.cpp
 *
 *  Created on: Sep 09, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */
#include "test-dyntypes.hpp"

#include <sutil/CSystemClock.hpp>

#include <sutil/CRegisteredDynamicTypes.hpp>

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace sutil_test
{
  class SType
  {
    /** This is so that down-casting is enabled in the subclasses.
     *
     * A virtual function is required to make a class "polymorphic" */
    virtual void doesNothing(){}
  public:
    /** To save some warnings about virtual destructors */
    virtual ~SType(){}
  };

  class SType1 : public SType
  {
  public:
    virtual ~SType1(){}
    int x;
  };

  class SType2 : public SType1
  {
  public:
    virtual ~SType2(){}
    int y;
  };

  class SType3 : public SType2
  {
  public:
    virtual ~SType3(){}
    int z;
  };

  /** Tests the dynamic typing utility
   * @param arg_id : The id of the test */
  void test_dyntypes(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
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


      //Test 2 : Test type functionality.
      typedef sutil::CRegisteredDynamicTypes<std::string> RegDynTypes;
      void* obj = NULL;

      SType1* t1; SType2* t2; SType3* t3;

      flag = RegDynTypes::getObjectForType(std::string("SType1"), obj);
      if(false == flag)
      { throw(std::runtime_error("Could not create dynamic object for type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created dynamic object for type SType1"; }

      t1 = reinterpret_cast<SType1*>(obj);
      t1->x = 10;
      std::cout<<"\nTest Result ("<<test_id++<<") Set data in type SType1 : "<<t1->x;

      delete t1;
      obj = NULL;

      flag = RegDynTypes::getObjectForType(std::string("SType2"), obj);
      if(false == flag)
      { throw(std::runtime_error("Could not create dynamic object for type SType2"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created dynamic object for type SType2"; }

      t1 = reinterpret_cast<SType1*>(obj);
      t2 = dynamic_cast<SType2*>(t1);
      std::cout<<"\nTest Result ("<<test_id++<<") Tested dynamic cast for SType1 to SType2";

      t2->x = 10;
      t2->y = 20;
      std::cout<<"\nTest Result ("<<test_id++<<") Set data in type SType2 : "<<t2->x<<" "<<t2->y;

      delete t2;
      obj = NULL;

      flag = RegDynTypes::getObjectForType(std::string("SType3"), obj);
      if(false == flag)
      { throw(std::runtime_error("Could not create dynamic object for type SType3"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created dynamic object for type SType3"; }

      t1 = reinterpret_cast<SType1*>(obj);
      t2 = dynamic_cast<SType2*>(t1);
      t3 = dynamic_cast<SType3*>(t1);
      t3 = dynamic_cast<SType3*>(t2);
      std::cout<<"\nTest Result ("<<test_id++<<") Tested dynamic cast for SType1 to SType2 to SType3";

      t3->x = 10;
      t3->y = 20;
      t3->z = 30;
      std::cout<<"\nTest Result ("<<test_id++<<") Set data in type SType3 : "<<t3->x<<" "<<t3->y<<" "<<t3->z;

      delete t3;
      obj = NULL;

      //Test 3 : Test new type-safe methods with smart pointers
      auto smart_t1 = RegDynTypes::createObject<SType1>(std::string("SType1"));
      if(!smart_t1)
      { throw(std::runtime_error("Could not create smart pointer object for type SType1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created smart pointer object for type SType1"; }

      smart_t1->x = 100;
      std::cout<<"\nTest Result ("<<test_id++<<") Set data in smart pointer SType1 : "<<smart_t1->x;

      auto smart_t2 = RegDynTypes::createObject<SType2>(std::string("SType2"));
      if(!smart_t2)
      { throw(std::runtime_error("Could not create smart pointer object for type SType2"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created smart pointer object for type SType2"; }

      smart_t2->x = 200;
      smart_t2->y = 300;
      std::cout<<"\nTest Result ("<<test_id++<<") Set data in smart pointer SType2 : "<<smart_t2->x<<" "<<smart_t2->y;

      auto smart_t3 = RegDynTypes::createObject<SType3>(std::string("SType3"));
      if(!smart_t3)
      { throw(std::runtime_error("Could not create smart pointer object for type SType3"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created smart pointer object for type SType3"; }

      smart_t3->x = 400;
      smart_t3->y = 500;
      smart_t3->z = 600;
      std::cout<<"\nTest Result ("<<test_id++<<") Set data in smart pointer SType3 : "<<smart_t3->x<<" "<<smart_t3->y<<" "<<smart_t3->z;

      // Test invalid type name
      auto invalid_obj = RegDynTypes::createObject<SType1>(std::string("InvalidType"));
      if(invalid_obj)
      { throw(std::runtime_error("Created object for invalid type"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Correctly failed to create object for invalid type"; }

      // Smart pointers automatically clean up when they go out of scope
      std::cout<<"\nTest Result ("<<test_id++<<") Smart pointers automatically manage memory";

      //Test 4: Edge cases and error conditions
      
      // Test null pointer handling in legacy API
      void* null_test = nullptr;
      flag = RegDynTypes::getObjectForType(std::string("SType1"), null_test);
      if(!flag || null_test == nullptr) {
        throw(std::runtime_error("Failed to create object with null pointer"));
      }
      delete static_cast<SType1*>(null_test);
      std::cout<<"\nTest Result ("<<test_id++<<") Null pointer handling works correctly";

      // Test invalid object creation (non-null pointer passed)
      void* non_null_test = reinterpret_cast<void*>(0x1); // Non-null invalid pointer
      flag = RegDynTypes::getObjectForType(std::string("SType1"), non_null_test);
      if(flag) {
        throw(std::runtime_error("Should have failed with non-null pointer"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Correctly rejected non-null pointer";

      // Test type registration boundary conditions
      flag = RegDynTypes::typeRegistered(std::string("SType1"));
      if(!flag) {
        throw(std::runtime_error("Type SType1 should be registered"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Type registration check works";

      flag = RegDynTypes::typeRegistered(std::string("NonExistentType"));
      if(flag) {
        throw(std::runtime_error("NonExistentType should not be registered"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Non-existent type correctly not found";

      // Test large object creation stress
      const int stress_objects = 100;
      std::vector<void*> stress_ptrs;
      for(int i = 0; i < stress_objects; ++i) {
        void* stress_obj = nullptr;
        flag = RegDynTypes::getObjectForType(std::string("SType3"), stress_obj);
        if(!flag || stress_obj == nullptr) {
          throw(std::runtime_error("Stress test object creation failed at iteration " + std::to_string(i)));
        }
        stress_ptrs.push_back(stress_obj);
      }
      
      // Clean up stress test objects
      for(void* ptr : stress_ptrs) {
        delete static_cast<SType3*>(ptr);
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Stress test created and cleaned up "<<stress_objects<<" objects";

      // Test mixed API usage (legacy and modern)
      void* legacy_obj = nullptr;
      flag = RegDynTypes::getObjectForType(std::string("SType2"), legacy_obj);
      if(!flag || legacy_obj == nullptr) {
        throw(std::runtime_error("Legacy API object creation failed"));
      }
      
      auto modern_obj = RegDynTypes::createObject<SType2>(std::string("SType2"));
      if(!modern_obj) {
        delete static_cast<SType2*>(legacy_obj);
        throw(std::runtime_error("Modern API object creation failed"));
      }
      
      // Both should work independently
      static_cast<SType2*>(legacy_obj)->x = 999;
      modern_obj->x = 888;
      
      if(static_cast<SType2*>(legacy_obj)->x != 999 || modern_obj->x != 888) {
        delete static_cast<SType2*>(legacy_obj);
        throw(std::runtime_error("Mixed API usage failed"));
      }
      
      delete static_cast<SType2*>(legacy_obj);
      std::cout<<"\nTest Result ("<<test_id++<<") Mixed legacy and modern API usage works";

      // Test empty string type name
      auto empty_obj = RegDynTypes::createObject<SType1>(std::string(""));
      if(empty_obj) {
        throw(std::runtime_error("Empty string type name should fail"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Empty string type name correctly rejected";

      // Test very long type name
      std::string long_name(10000, 'A');
      auto long_name_obj = RegDynTypes::createObject<SType1>(long_name);
      if(long_name_obj) {
        throw(std::runtime_error("Very long type name should fail"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Very long type name correctly rejected";

      // Test reset functionality
      flag = RegDynTypes::resetDynamicTypes();
      if(!flag) {
        throw(std::runtime_error("Reset dynamic types failed"));
      }
      
      // After reset, types should not be registered
      flag = RegDynTypes::typeRegistered(std::string("SType1"));
      if(flag) {
        throw(std::runtime_error("Type should not be registered after reset"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Reset functionality works correctly";

      // Re-register types after reset
      flag = type1.registerType();
      if(!flag) {
        throw(std::runtime_error("Failed to re-register type after reset"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Type re-registration after reset works";

      std::cout<<"\nTest #"<<arg_id<<" (Dynamic Typing Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Dynamic Typing Test) Failed";
    }
  }

}

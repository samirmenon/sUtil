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

      std::cout<<"\nTest #"<<arg_id<<" (Dynamic Typing Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Dynamic Typing Test) Failed";
    }
  }

}

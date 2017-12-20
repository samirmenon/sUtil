/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/*
 * test-printables.cpp
 *
 *  Created on: Sep 22, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */
#include "test-printables.hpp"

#include <sutil/CRegisteredPrintables.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

namespace sutil_test
{
  /** A random type, whose contents we want to print to an outstream.
   *
   * But we don't want this type to inherit from something or define
   * some (inefficient and ugly) "print" function.
   *
   * Moreover, some users want to print everything in this class,
   * while others only want to print its name. The print function
   * should allow users to print what they want.
   *
   * Eg. You bundle this class in a library, but want to give users
   * an easy way to print its contents on to the screen without forcing
   * them to use some in-built print function. You also want your
   * library's code to be slim and trim (extra code means extra
   * maintenance means extra cost).
   *
   * This test will demonstrate a nice way to support the above */
  class CUserType
  {
  public:
    CUserType(const std::string& arg_name) :
      x_(0), y_(10), z_(100), name_(arg_name)
    {}

    int x_, y_, z_;
    std::string name_;

  private:
    //I can't do without a name
    CUserType();
    CUserType(const CUserType&);
    CUserType& operator ==(const CUserType&);
  };
}

/** This is all we need to specify to support printing our
 * useless type. (And every user can specify it differently).
 *
 * Ie. This function will not be in the library (except for some
 * very commonly used types). And even if it is, the user doesn't
 * have to use the library's version. */
namespace sutil
{
  template<>
  void printToStream<sutil_test::CUserType>(
      std::ostream& str, const sutil_test::CUserType& obj)
  { str<<obj.name_<<" [ "<<obj.x_<<" , "<<obj.y_<<" , "<<obj.z_<<" ]"; }
}

namespace sutil_test
{
  /** Tests the printable utilities
   * @param arg_id : The id of the test */
  void test_printables(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Create printable object
      const std::string o1n("o1"), o1c("o1clone"), o2n("o2"), test("I_Dont_Exist");
      CUserType o1(o1n), o1clone(o1n), o2(o2n);

      o1clone.x_ = 2* o1.x_;
      o1clone.y_ = 2* o1.y_;
      o1clone.z_ = 2* o1.z_;

      o2.x_ = 3* o1.x_;
      o2.y_ = 3* o1.y_;
      o2.z_ = 3* o1.z_;

      //Test 1 : Register printable. Should succeed.
      flag = sutil::printables::add<CUserType>(o1n, o1);
      if(false == flag)
      { throw(std::runtime_error("Failed to register printable : o1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered printable : o1"; }

      flag = sutil::printables::add<CUserType>(o1c, o1clone);
      if(false == flag)
      { throw(std::runtime_error("Failed to register printable : o1clone"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered printable : o1clone"; }

      flag = sutil::printables::add<CUserType>(o2n, o2);
      if(false == flag)
      { throw(std::runtime_error("Failed to register printable : o2"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Registered printable : o2"; }

      //Test 2: Test whether the printables work
      if(NULL != sutil::printables::get(test))
      { throw(std::runtime_error("Get returned a non-existent printable"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Couldn't find non-existent printable"; }

      if(NULL == sutil::printables::get(o1n))
      { throw(std::runtime_error("Could not get printable o1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") get returned printable o1"; }

      if(NULL == sutil::printables::get(o1c))
      { throw(std::runtime_error("Could not get printable o1clone"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") get returned printable o1clone"; }

      if(NULL == sutil::printables::get(o2n))
      { throw(std::runtime_error("Could not get printable o2"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") get returned printable o2"; }

      std::cout<<"\nTest Result ("<<test_id++<<") Printing printables: "
          <<*sutil::printables::get(o1n)<<", "<<*sutil::printables::get(o1c)
          <<", "<<*sutil::printables::get(o2n);

      std::stringstream s1,s2,s3;
      s1<<*sutil::printables::get(o1n);
      s2<<*sutil::printables::get(o1c);
      s3<<*sutil::printables::get(o2n);

      std::string ss1,ss2,ss3;
      s1>>ss1;
      s2>>ss2;
      s3>>ss3;

      if(ss1 != ss2)
      { throw(std::runtime_error("o1 and its clone don't match"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") o1 matches its clone"; }

      if(ss1 == ss3)
      { throw(std::runtime_error("o1 and o2 match"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") o1 doesn't match o2"; }

      //Give the clone a new identity and see if the printable accesses it.
      o1clone.name_ = "A clone no more";
      s1.str("");s1<<*sutil::printables::get(o1n);
      s2.str("");s2<<*sutil::printables::get(o1c);
      s1>>ss1;
      s2>>ss2;

      if(ss1 == ss2)
      {
        throw(std::runtime_error(
            std::string("o1 and its clone match after the clone's identity changed: o1=")+
            ss1+std::string(", o1clone=")+ss2));
      }
      else
      {
        std::cout<<"\nTest Result ("<<test_id++<<
        ") o1 doesn't match its clone after the clone's identity changed";
      }

      std::cout<<"\nTest #"<<arg_id<<" (Printable Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Printable Test) Failed";
    }
  }

}

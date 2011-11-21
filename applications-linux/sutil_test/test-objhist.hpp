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
 * test-objhist.hpp
 *
 *  Created on: Sep 09, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_OBJHIST_HPP_
#define TEST_OBJHIST_HPP_

#include <sutil/CObjectHistory.hpp>
#include <sutil/CSystemClock.hpp>

#include <iostream>
#include <string>
#include <sstream>

namespace sutil_test
{
  /** A random data structure to test the object history store */
  struct SObjectToStore
  {
    double x_, y_, z_;
    std::vector<double> vec_;
  };

  /** Tests the object history storage utility
   * @param arg_id : The id of the test */
  void test_objhist(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    const int instances_to_store = 5;
    try
    {
      std::cout<<std::flush;
      sutil::CObjectHistory<std::string,SObjectToStore> oh;

      SObjectToStore o1;

      //Store an object's history instances_to_store times
      for(int i=0; i<instances_to_store; ++i)
      {
        //Initialize the object (to different values)
        o1.x_ = i;
        o1.y_ = i*i;
        o1.z_ = i*i*i;
        o1.vec_.push_back(sutil::CSystemClock::getSysTime());

        //Store the object
        flag = oh.saveObject("o1",o1);
        if(false == flag)
        { throw(std::runtime_error( "Failed to save object to history" ));  }
        else
        { std::cout<<"\nObject History ("<<i<<") Stored object to history"; }

        //Print stored object to screen
        std::cout<<"\n Obj: "<<o1.x_<<", "<<o1.y_<<", "<<o1.z_<<". [ ";
        std::vector<double>::const_iterator it2,it2e;
        for(it2 = o1.vec_.begin(), it2e = o1.vec_.end();
            it2!=it2e; ++it2)
        { std::cout<<*it2<<" ";  }
        std::cout<<"]";
      }

      std::cout<<"\nTest Result ("<<test_id++<<") Stored object's history time series.";

      //Now print all the stored instances
      const sutil::CMappedList<double,SObjectToStore>* olist = oh.getObjectTimeSeries("o1");
      if(NULL == olist)
      { throw(std::runtime_error( "Failed to retrieve object's history time series" ));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Retrieved object's history time series."; }

      int j=0;
      sutil::CMappedList<double,SObjectToStore>::const_iterator it, ite;
      for(it = olist->begin(), ite = olist->end();
          it!=ite; ++it, ++j/*To count iterations*/)
      {
        //Get the object
        const SObjectToStore& o2 = *it;

        //Now print the object's contents
        std::cout<<"\nObject History ("<<j<<") Time ["<<(!it)<<"]";
        std::cout<<"\n Obj: "<<o2.x_<<", "<<o2.y_<<", "<<o2.z_<<". [ ";
        std::vector<double>::const_iterator it2,it2e;
        for(it2 = o2.vec_.begin(), it2e = o2.vec_.end();
            it2!=it2e; ++it2)
        { std::cout<<*it2<<" ";  }
        std::cout<<"]";
      }

      if(instances_to_store != j)
      { throw(std::runtime_error( "Failed to retrieve the correct number of entries in the object's history time series" ));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Retrieved the correct number of entries in the object's history time series."; }

      if(false == flag)
      { throw(std::runtime_error("Client: Failed to read Object History"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") "; }

      std::cout<<"\nTest #"<<arg_id<<" (Object History Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Object History Test) Failed";
    }
  }

}
#endif /* TEST_OBJHIST_HPP_ */

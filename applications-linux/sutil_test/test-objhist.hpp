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
#include <vector>

namespace sutil_test
{
  /** A random data structure to test the object history store */
  struct SObjectToStore
  {
    double x_, y_, z_;
    std::vector<double> vec_;

    void print() const
    {
      std::cout<<"Obj: "<<x_<<", "<<y_<<", "<<z_<<". [ ";
      std::vector<double>::const_iterator it2,it2e;
      for(it2 = vec_.begin(), it2e = vec_.end();
          it2!=it2e; ++it2)
      { std::cout<<*it2<<" ";  }
      std::cout<<"]";
    }
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
        std::cout<<"\n "; o1.print();
      }

      std::cout<<"\nTest Result ("<<test_id++<<") Stored object's history time series.";

      //Now print all the stored instances
      const sutil::CMappedList<double,SObjectToStore>* olist = oh.getObjectTimeSeries("o1");
      if(NULL == olist)
      { throw(std::runtime_error( "Failed to retrieve object's history time series" ));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Retrieved object's history time series."; }

      int j=0;
      double time_stored[instances_to_store]; //For since object retrieval.

      sutil::CMappedList<double,SObjectToStore>::const_iterator it, ite;
      for(it = olist->begin(), ite = olist->end();
          it!=ite; ++it, ++j/*To count iterations*/)
      {
        //Get the object
        const SObjectToStore& o2 = *it;

        //Now print the object's contents
        std::cout<<"\nObject History ("<<j<<") Time ["<<(!it)<<"]";
        std::cout<<"\n "; o2.print();

        time_stored[j] = !it; //Save the storage time
      }

      if(instances_to_store != j)
      { throw(std::runtime_error( "Failed to retrieve the correct number of entries in the object's history time series" ));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Retrieved the correct number of entries in the object's history time series."; }

      //Test the dual map
      const SObjectToStore* tobj;

      for(int i=0; i<instances_to_store; ++i)
      {
        tobj = oh.getObject("o1", time_stored[i]);
        if(NULL == tobj)
        { throw(std::runtime_error( "Failed to retrieve single object based on stored time" ));  }
        else
        { std::cout<<"\nObject retrieve ("<<i<<") Retrieved object stored at time: "<<time_stored[i]; }
      }

      std::cout<<"\nTest Result ("<<test_id++<<") Retrieved the objects by time index.";

      //Test the default (last object stored) retrieve call
      tobj = oh.getObject("o1");
      if(NULL == tobj)
      { throw(std::runtime_error( "Failed to retrieve last single object" ));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Retrieved the last single object."; }

      //Test object removal : Remove all but last three objects
      for(int i=0; i<instances_to_store-3; ++i)
      {
        flag = oh.removeObject("o1", time_stored[i]);
        if(false == flag)
        { throw(std::runtime_error( "Failed to remove single object based on stored time" ));  }
        else
        { std::cout<<"\nObject delete ("<<i<<") Removed object stored at time: "<<time_stored[i]; }

        tobj = oh.getObject("o1", time_stored[i]);
        if(NULL != tobj)
        { throw(std::runtime_error( "Delete unsuccessful. Retrieved deleted object" ));  }
      }

      //Remove a single object
      flag = oh.removeObject("o1");
      if(false == flag)
      { throw(std::runtime_error( "Failed to remove the last single object" ));  }
      else
      { std::cout<<"\nObject delete : Removed the last single object"; }

      //Last one stored should be the next one in the list
      tobj = oh.getObject("o1", time_stored[instances_to_store-3]);
      if(NULL != tobj)
      {
        std::cout<<"\nRetrieved deleted last single object: ";
        tobj->print();
        throw(std::runtime_error( "Delete unsuccessful" ));
      }

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

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
 * test-pilemap.hpp
 *
 *  Created on: Jul 17, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef UTIL_DS_TESTS_HPP_
#define UTIL_DS_TESTS_HPP_

#include <sutil/CSystemClock.hpp>

#include <sutil/CMemCopier.hpp>
#include <sutil/CPileMap.hpp>
#include <sutil/CMultiLevelPileMap.hpp>

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

namespace sutil_test
{
  /** Tests the pilemap utility
   * @param arg_id : The id of the test */
  void test_pilemap(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Timers
      double time1,time2;

      //Initialize the buffers and the mem copier
      double *t1, *t2, *t3, *t4;
      sutil::CPileMap<std::string,double> pilemap;

      std::string tstr;

      // Test 1 : Create nodes
      tstr = "t1";
      t1 = pilemap.create(tstr);
      if((NULL == t1) || (NULL==pilemap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 1"; }

      tstr = "t2";
      t2 = pilemap.create(tstr);
      if((NULL == t2) || (NULL==pilemap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 2"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 2"; }

      tstr = "t3";
      t3 = pilemap.create(tstr);
      if((NULL == t3) || (NULL==pilemap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 3"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 3"; }

      tstr = "t4";
      t4 = pilemap.create(tstr);
      if((NULL == t4) || (NULL==pilemap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 4"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 4"; }

      //Test 2 : Set values to the 4 nodes.
      *t1 = 1; *t2 = 2; *t3 = 3; *t4 = 4;
      std::cout<<"\nTest Result ("<<test_id++<<") Nodes created: "<<*t1<<" "<<*t2<<" "<<*t3<<" "<<*t4;

      if(4 != pilemap.size())
      { throw(std::runtime_error("Pilemap size is not 4 after creating 4 nodes")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Pile size is 4 with 4 nodes"; }

      if( (4==*(pilemap.at(0))) && (3==*(pilemap.at(1))) &&
          (2==*(pilemap.at(2))) && (1==*(pilemap.at(3))) )
      { std::cout<<"\nTest Result ("<<test_id++<<") Pile values correctly set"; }
      else
      { throw(std::runtime_error("Failed to set pilemap values correctly")); }

      if((1==*(pilemap.at("t1"))) &&
         (2==*(pilemap.at("t2"))) &&
         (3==*(pilemap.at("t3"))) &&
         (4==*(pilemap.at("t4")))
        )
      { std::cout<<"\nTest Result ("<<test_id++<<") Pile map access correctly initialized"; }
      else
      {
        std::cout<<"\nValues:"<<*(pilemap.at("t1"))<<" "
            <<*(pilemap.at("t2"))<<" "
            <<*(pilemap.at("t3"))<<" "
            <<*(pilemap.at("t4"));
        throw(std::runtime_error("Failed to initialize pile map access"));
      }

      //Test 4 : Erase one node.
      flag = pilemap.erase(t2);
      if( (false==flag) || (NULL != pilemap.at("t2")) )
      { throw(std::runtime_error("Failed to delete node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Deleted node successfully"; }

      if(3 != pilemap.size())
      { throw(std::runtime_error("Unexpected pilemap size after deleting node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Pile size is 3 after deleting one node"; }

      if( (4==*(pilemap.at(0))) && (3==*(pilemap.at(1))) && (1==*(pilemap.at(2))) )
      { std::cout<<"\nTest Result ("<<test_id++<<") Pile values correctly set"; }
      else
      { throw(std::runtime_error("Pile values incorrect after deleting node")); }

      //Test 5 : Test copy constructor.
      time1 = 5.12;
      tstr="t2";
      t2 = pilemap.create(tstr,time1);
      if( (NULL==t2) || (5.12!=*(pilemap.at(0))) || (4!=pilemap.size()))
      { throw(std::runtime_error("Failed to create pilemap node with copy-constructor")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created pilemap node with copy constructor"; }

      //Test 6 : Test clear
      flag = pilemap.clear();

      if( (!flag) || ( NULL != pilemap.at(0)))
      { throw(std::runtime_error("Failed to clear pilemap"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Cleared pilemap"; }

      //Test 7 : Pile stress
      std::stringstream ss;
      std::cout<<"\nTest Result ("<<test_id++<<") Initializing large pilemap: 0%";
      const unsigned int pilemap_sz=1000;
      for(unsigned int i=0;i<pilemap_sz;i++)
      {
        ss<<"t2_"<<i;
        t2 = pilemap.create(ss.str());
        if(NULL == t2)
        { throw(std::runtime_error("Failed to initialize 1,000 pilemap")); }
        if(i%((int)(pilemap_sz/10)) == 0)
        { std::cout<<"+10%"; std::flush(std::cout);  }
      }

      if( pilemap_sz != pilemap.size())
      { throw(std::runtime_error("Pilemap size should be 1,000, but isn't")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Correctly initialized "<<pilemap_sz<<" pilemap"; }

      //Test the access speed
      time1 = sutil::CSystemClock::getSysTime();
      for(long long i=0;i<10000;i++)
      {
        t2 = pilemap.at(pilemap_sz-1);
      }
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Pile Retrieve Stress : "<<pilemap_sz
          <<" <double> entry ("<<*t2<<") 10,000 times"
          <<" in "<<time2-time1<<" seconds";

      std::cout<<"\nTest #"<<arg_id<<" (Pilemap Test) Successful";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Pilemap Test) Failed";
    }
  }


  /** A data structure to test the multi-level pilemap, which requires a value and priority */
  struct SMapTester{ double val; unsigned int pri; };

  /** Tests the multi-level pilemap utility
   * @param arg_id : The id of the test */
  void test_multi_pilemap(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Initialize the buffers and the mem copier
      SMapTester *t1, *t2, *t3, *t4;
      sutil::CMultiLevelPileMap<std::string,SMapTester> mlpmap;

      std::string tstr;

      // Test 1 : Create nodes
      tstr = "t1";
      t1 = mlpmap.create(tstr,0); t1->val = 1; t1->pri = 0;
      if((NULL == t1) || (NULL==mlpmap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 1")); }

      tstr = "t2";
      t2 = mlpmap.create(tstr,2); t2->val = 2; t2->pri = 2;
      if((NULL == t2) || (NULL==mlpmap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 2")); }

      tstr = "t3";
      t3 = mlpmap.create(tstr,2); t3->val = 3; t3->pri = 2;
      if((NULL == t3) || (NULL==mlpmap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 3")); }

      tstr = "t4";
      t4 = mlpmap.create(tstr,8); t4->val = 4; t4->pri = 8;
      if((NULL == t4) || (NULL==mlpmap.at(tstr)))
      { throw(std::runtime_error("Failed to create node 4")); }

      if(9!=mlpmap.getNumPriorityLevels())
      { throw(std::runtime_error("Incorrectly added task levels")); }

      if((t1->pri != (unsigned int) mlpmap.getPriorityLevel(t1)) ||
          (t1->pri != (unsigned int) mlpmap.getPriorityLevel("t1")) ||
          (t2->pri != (unsigned int) mlpmap.getPriorityLevel(t2)) ||
          (t2->pri != (unsigned int) mlpmap.getPriorityLevel("t2")) ||
          (t3->pri != (unsigned int) mlpmap.getPriorityLevel(t3)) ||
          (t3->pri != (unsigned int) mlpmap.getPriorityLevel("t3")) ||
          (t4->pri != (unsigned int) mlpmap.getPriorityLevel(t4)) ||
          (t4->pri != (unsigned int) mlpmap.getPriorityLevel("t4"))
      )
      { throw(std::runtime_error("Incorrect priority level recall")); }

      std::cout<<"\nTest Result ("<<test_id++<<") Created 4 nodes in the mlpmap : ";

      //Test 2 : Set values to the 4 nodes.
      std::cout<<t1->val<<","<<t1->pri<<" "
          <<t2->val<<","<<t2->pri<<" "
          <<t3->val<<","<<t3->pri<<" "
          <<t4->val<<","<<t4->pri;

      if(4 != mlpmap.size())
      { throw(std::runtime_error("Failed. Unexpected mlpmap size")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") MultiLevelPmap size is 4 with 4 nodes"; }

      if( (4==(mlpmap.at(0))->val) && (3==(mlpmap.at(1))->val) &&
          (2==(mlpmap.at(2))->val) && (1==(mlpmap.at(3))->val) )
      { std::cout<<"\nTest Result ("<<test_id++<<") MultiLevelPmap values correctly set"; }
      else
      { throw(std::runtime_error("Failed to set mlpmap values correctly")); }

      if((0==(mlpmap.at("t1"))->pri) &&
          (2==(mlpmap.at("t2"))->pri) &&
          (2==(mlpmap.at("t3"))->pri) &&
          (8==(mlpmap.at("t4"))->pri)
      )
      { std::cout<<"\nTest Result ("<<test_id++<<") MultiLevelPmap map access correctly initialized"; }
      else
      { throw(std::runtime_error("Failed to initialize pile map access"));  }

      //Test 4 : Erase one node.
      flag = mlpmap.erase(t4);
      if( (false==flag) || (NULL != mlpmap.at("t4")) )
      { throw(std::runtime_error("Failed to delete node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Deleted node successfully"; }

      if(3 != mlpmap.getNumPriorityLevels())
      { throw(std::runtime_error("Failed. Unexpected mlpmap priority levels after deleting node."));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") MultiLevelPmap has 3 levels after deleting level 8 node"; }

      flag = mlpmap.erase("t1");
      if( (false==flag) || (NULL != mlpmap.at("t1")) )
      { throw(std::runtime_error("Failed to delete node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Deleted node successfully"; }

      if(3 != mlpmap.getNumPriorityLevels())
      { throw(std::runtime_error("Failed. Unexpected mlpmap priority levels after deleting node."));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") MultiLevelPmap has 3 levels after deleting level 0 node"; }

      //Test 5 : Test copy constructor.
      SMapTester tmp_var; tmp_var.pri = 9; tmp_var.val = 6;
      tstr="t1";
      t2 = mlpmap.create(tstr,tmp_var,tmp_var.pri);
      if( (NULL==t2) || (tmp_var.pri!=(mlpmap.at("t1")->pri)) ||
          ((tmp_var.pri+1)!=mlpmap.getNumPriorityLevels()))
      { throw(std::runtime_error("Failed to create mlpmap node with copy-constructor"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Created mlpmap node with copy constructor"; }

      if(tmp_var.val!=mlpmap.getSinglePriorityLevel(tmp_var.pri)->at(0)->val)
      { throw(std::runtime_error("Failed to access single priority level"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Correctly accessed single priority level"; }

      //Test 6 : Test clear
      flag = mlpmap.clear();

      if( (!flag) || ( NULL != mlpmap.at(0)))
      { throw(std::runtime_error("Failed to clear mlpmap"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Cleared mlpmap"; }

      std::cout<<"\nTest #"<<arg_id<<" (MultiLevelPmap Test) Successful";
    }
    catch(std::exception &e)
    {
      std::cout<<"\nTest Result ("<<test_id++<<") "<<e.what();
      std::cout<<"\nTest #"<<arg_id<<" (MultiLevelPmap Test) Failed";
    }
  }

}
#endif /* UTIL_DS_TESTS_HPP_ */

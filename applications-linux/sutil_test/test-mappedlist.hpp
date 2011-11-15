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
 * test-mappedlist.hpp
 *
 *  Created on: Jul 17, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_MAPPEDLIST_HPP_
#define TEST_MAPPEDLIST_HPP_

#include <sutil/CSystemClock.hpp>

#include <sutil/CMemCopier.hpp>
#include <sutil/CMappedList.hpp>
#include <sutil/CMappedMultiLevelList.hpp>

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

namespace sutil_test
{
  /** Tests the mapped list utility
   * @param arg_id : The id of the test */
  void test_mappedlist(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Timers
      double time1,time2;

      //Initialize the buffers and the mem copier
      double *t1, *t2, *t3, *t4;
      sutil::CMappedList<std::string,double> mappedlist;

      std::string tstr;

      // Test 1 : Create nodes
      tstr = "t1";
      t1 = mappedlist.create(tstr);
      if((NULL == t1) || (NULL==mappedlist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 1"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 1"; }

      tstr = "t2";
      t2 = mappedlist.create(tstr);
      if((NULL == t2) || (NULL==mappedlist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 2"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 2"; }

      tstr = "t3";
      t3 = mappedlist.create(tstr);
      if((NULL == t3) || (NULL==mappedlist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 3"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 3"; }

      tstr = "t4";
      t4 = mappedlist.create(tstr);
      if((NULL == t4) || (NULL==mappedlist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 4"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created node 4"; }

      //Test 2 : Set values to the 4 nodes.
      *t1 = 1; *t2 = 2; *t3 = 3; *t4 = 4;
      std::cout<<"\nTest Result ("<<test_id++<<") Nodes created: "<<*t1<<" "<<*t2<<" "<<*t3<<" "<<*t4;

      if(4 != mappedlist.size())
      { throw(std::runtime_error("Mapped list size is not 4 after creating 4 nodes")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Mapped list size is 4 with 4 nodes"; }

      if( (4==*(mappedlist.at(0))) && (3==*(mappedlist.at(1))) &&
          (2==*(mappedlist.at(2))) && (1==*(mappedlist.at(3))) )
      { std::cout<<"\nTest Result ("<<test_id++<<") Mapped list values correctly set"; }
      else
      { throw(std::runtime_error("Failed to set mapped list values correctly")); }

      if((1==*(mappedlist.at("t1"))) &&
         (2==*(mappedlist.at("t2"))) &&
         (3==*(mappedlist.at("t3"))) &&
         (4==*(mappedlist.at("t4")))
        )
      { std::cout<<"\nTest Result ("<<test_id++<<") Mapped list map access correctly initialized"; }
      else
      {
        std::cout<<"\nValues:"<<*(mappedlist.at("t1"))<<" "
            <<*(mappedlist.at("t2"))<<" "
            <<*(mappedlist.at("t3"))<<" "
            <<*(mappedlist.at("t4"));
        throw(std::runtime_error("Failed to initialize mapped list access"));
      }

      //Test 4 : Erase one node.
      flag = mappedlist.erase(t2);
      if( (false==flag) || (NULL != mappedlist.at("t2")) )
      { throw(std::runtime_error("Failed to delete node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Deleted node successfully"; }

      if(3 != mappedlist.size())
      { throw(std::runtime_error("Unexpected mapped list size after deleting node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Mapped list size is 3 after deleting one node"; }

      if( (4==*(mappedlist.at(0))) && (3==*(mappedlist.at(1))) && (1==*(mappedlist.at(2))) )
      { std::cout<<"\nTest Result ("<<test_id++<<") Mapped list values correctly set"; }
      else
      { throw(std::runtime_error("Mapped list values incorrect after deleting node")); }

      //Test 5 : Test copy constructor.
      time1 = 5.12;
      tstr="t2";
      t2 = mappedlist.create(tstr,time1);
      if( (NULL==t2) || (5.12!=*(mappedlist.at(0))) || (4!=mappedlist.size()))
      { throw(std::runtime_error("Failed to create mapped list node with copy-constructor")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Created mapped list node with copy constructor"; }

      //Test 6 : Test clear
      flag = mappedlist.clear();

      if( (!flag) || ( NULL != mappedlist.at(0)))
      { throw(std::runtime_error("Failed to clear mapped list"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Cleared mapped list"; }

      //Test 7 : Mapped list stress
      std::stringstream ss;
      std::cout<<"\nTest Result ("<<test_id++<<") Initializing large mapped list: 0%";
      const unsigned int maplist_sz=1000;
      for(unsigned int i=0;i<maplist_sz;i++)
      {
        ss<<"t2_"<<i;
        t2 = mappedlist.create(ss.str());
        *t2 = i; //Initialize the node's (double) value to the index of insertion
        if(NULL == t2)
        { throw(std::runtime_error("Failed to initialize 1,000 mapped list")); }
        if(i%((int)(maplist_sz/10)) == 0)
        { std::cout<<"+10%"; std::flush(std::cout);  }
      }

      if( maplist_sz != mappedlist.size())
      { throw(std::runtime_error("Mapped list size should be 1,000, but isn't")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Correctly initialized "<<maplist_sz<<" mapped list"; }

      //Test the access speed
      time1 = sutil::CSystemClock::getSysTime();
      for(long long i=0;i<10000;i++)
      { t2 = mappedlist.at(maplist_sz-1); }
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Retrieve Last Element Stress : "<<maplist_sz
          <<" <double> entry ("<<*t2<<") 10,000 times"
          <<" in "<<time2-time1<<" seconds";

      std::cout<<std::flush;//Time for the next round

      /**  **********************
       * Test operators
       * *********************** */
      sutil::CMappedList<std::string, int> l1,l2;
      l1.create("1",1);
      l1.create("2",2);
      l1.create("3",3);
      if(l1 == l1) { std::cout<<"\nTest Result ("<<test_id++<<") operator == works correctly with identical lists"; }
      else { throw(std::runtime_error("operator == failed with identical lists")); }

      if(l1 != l1) { throw(std::runtime_error("operator != failed with identical lists")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") operator != works correctly with identical lists"; }

      std::cout<<std::flush;//Time for the next round

      l2 = l1;

      if(l1 == l1) { std::cout<<"\nTest Result ("<<test_id++<<") operator == works after using operator = "; }
      else { throw(std::runtime_error("operator = failed after using operator = ")); }

      if(l1 == l2) { std::cout<<"\nTest Result ("<<test_id++<<") operator = works correctly"; }
      else { throw(std::runtime_error("operator = failed")); }

      if(l2 == l1) { std::cout<<"\nTest Result ("<<test_id++<<") operator = works correctly, opposite order"; }
      else { throw(std::runtime_error("operator = failed, opposite order")); }

      l2.create("22",22);
      if(l1 != l2) { std::cout<<"\nTest Result ("<<test_id++<<") operator != works for unequal lists at start"; }
      else { throw(std::runtime_error("operator == failed for unequal lists at start")); }

      if(l2 != l1) { std::cout<<"\nTest Result ("<<test_id++<<") operator != works for unequal lists at start, opposite order"; }
      else { throw(std::runtime_error("operator == failed for unequal lists at start, opposite order")); }

      flag = l2.erase("22");
      if(false == flag) { throw(std::runtime_error("Could not erase element from temp list"));  }

      if(l1 == l2) { std::cout<<"\nTest Result ("<<test_id++<<") operator = works after insert and erase"; }
      else { throw(std::runtime_error("operator = failed after insert and erase")); }

      if(l2 == l1) { std::cout<<"\nTest Result ("<<test_id++<<") operator = works after insert and erase, opposite order"; }
      else { throw(std::runtime_error("operator = failed after insert and erase, opposite order")); }

      l2.create("22",22,false);
      if(l1 != l2) { std::cout<<"\nTest Result ("<<test_id++<<") operator != works for unequal lists at end"; }
      else { throw(std::runtime_error("operator == failed for unequal lists at start")); }

      if(l2 != l1) { std::cout<<"\nTest Result ("<<test_id++<<") operator != works for unequal lists at end, opposite order"; }
      else { throw(std::runtime_error("operator == failed for unequal lists at start, opposite order")); }

      std::cout<<std::flush;//Time for the next round

      /**  **********************
       * Test deep copy
       * *********************** */
      //Copy constructor test
      sutil::CMappedList<std::string,double> mappedlist_c1(mappedlist);
      if(mappedlist == mappedlist_c1)
      { std::cout<<"\nTest Result ("<<test_id++<<") Copy constructor works correctly"; }
      else
      { throw(std::runtime_error("Mapped list copy constructor failed")); }

      sutil::CMappedList<std::string,double> mappedlist_c2;
      mappedlist_c2 = mappedlist;
      if(mappedlist_c2 == mappedlist_c1)
      { std::cout<<"\nTest Result ("<<test_id++<<") Deep copy works correctly"; }
      else
      { throw(std::runtime_error("Deep copy failed to produce identical results with copy constructor and operator =")); }


      /** **********************
       * Iterator tests
       * *********************** */
      sutil::CMappedList<std::string,double> mappedlist2;
      mappedlist2.create("0",0.0);
      mappedlist2.create("1",1.0);
      mappedlist2.create("2",2.0);

      sutil::CMappedList<std::string,double>::iterator it, ite;

      //Test the iterator's * operator
      it = mappedlist2.begin();
      if(2 != *it) { throw(std::runtime_error("Iterator * operator failed")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Iterator * operator works"; }

      //Test the iterator's ! operator
      it = mappedlist2.begin();
      if(std::string("2") != !it) { throw(std::runtime_error("Iterator ! operator failed")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Iterator ! operator works"; }

      //Test the iterator's ++ operators
      it = mappedlist2.begin();
      ++it;
      if(1 != *it) { throw(std::runtime_error("Iterator prefix ++ operator failed")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Iterator prefix ++ operator works"; }

      it = mappedlist2.begin();
      it++;
      if(1 != *it) { throw(std::runtime_error("Iterator postfix ++ operator failed")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Iterator postfix ++ operator works"; }

      //Test the iterator's + operator
      it = mappedlist2.begin();
      ite = it+1;
      it++;
      if(it != ite) { throw(std::runtime_error("Iterator + operator failed")); }

      it = mappedlist2.begin();
      ite = it+2;
      it++; it++;
      if(it != ite) { throw(std::runtime_error("Iterator + operator failed at second iteration")); }

      std::cout<<"\nTest Result ("<<test_id++<<") Iterator + operator works";

      it = mappedlist2.begin();
      ite = it + mappedlist2.size() + 2;
      if(mappedlist2.end() != ite) { throw(std::runtime_error("Iterator + operator failed at overflow")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Iterator + operator works at overflow"; }

      //Test the iterator's access speed
      time1 = sutil::CSystemClock::getSysTime();
      for(long long i=0;i<10000;i++)
      {
        for(it = mappedlist.begin(), ite = mappedlist.end();
            it!=ite;++it);
        *t2 = i;
      } //Assign the element to make sure it takes some time
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Iterator Retrieve Last Element Stress : "<<maplist_sz
          <<" <double> entries 10,000 times in "<<time2-time1<<" seconds";

      //Test the iterator's iteration speed
      time1 = sutil::CSystemClock::getSysTime();
      for(it = mappedlist.begin(), ite = mappedlist.end();
          it!=ite;++it)
      { *t2 = *it; } //Assign the element to make sure it takes some time
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Iterator Copy Each Element : "<<maplist_sz
          <<" <double> entries in "<<time2-time1<<" seconds";

      //Test the iterator's access
      time1 = sutil::CSystemClock::getSysTime();
      int i=0;
      for(it = mappedlist.begin(), ite = mappedlist.end();
          it!=ite;++it)
      {
        *it = i;
        i++;
      }
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Iterator Change Each Element : "<<maplist_sz
                <<" <double> entries in "<<time2-time1<<" seconds";

      i=0;
      for(it = mappedlist.begin(), ite = mappedlist.end();
          it!=ite;++it)
      {
        if(*it != i)
        { throw(std::runtime_error("Mapped list Iterator Change Each Element failed")); }
        i++;
      }
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Iterator Change Each Element : Verified";

      /** **********************
       * Const Iterator tests
       * *********************** */

      sutil::CMappedList<std::string,double>::const_iterator cit, cite;

      //Test the citerator's access speed
      time1 = sutil::CSystemClock::getSysTime();
      for(long long i=0;i<10000;i++)
      {
        for(cit = mappedlist.begin(), cite = mappedlist.end();
            cit!=cite;++cit);
        *t2 = i;
      } //Assign the element to make sure it takes some time
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Const Iterator Retrieve Last Element Stress : "<<maplist_sz
          <<" <double> entries 10,000 times in "<<time2-time1<<" seconds";

      //Test the citerator's iteration speed
      time1 = sutil::CSystemClock::getSysTime();
      for(cit = mappedlist.begin(), cite = mappedlist.end();
          cit!=cite;++cit)
      { *t2 = *cit; } //Assign the element to make sure it takes some time
      time2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Mapped list Const Iterator Copy Each Element : "<<maplist_sz
          <<" <double> entries in "<<time2-time1<<" seconds";

      std::cout<<"\nTest #"<<arg_id<<" (Mapped list Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Mapped list Test) Failed";
    }
  }


  /** A data structure to test the multi-level mapped list, which requires a value and priority */
  struct SMapTester{ double val; unsigned int pri; };

  /** Tests the multi-level mapped list utility
   * @param arg_id : The id of the test */
  void test_mapped_multi_level_list(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    try
    {
      //Initialize the buffers and the mem copier
      SMapTester *t1, *t2, *t3, *t4;
      sutil::CMappedMultiLevelList<std::string,SMapTester> mapmllist;

      std::string tstr;

      // Test 1 : Create nodes
      tstr = "t1";
      t1 = mapmllist.create(tstr,0); t1->val = 1; t1->pri = 0;
      if((NULL == t1) || (NULL==mapmllist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 1")); }

      tstr = "t2";
      t2 = mapmllist.create(tstr,2); t2->val = 2; t2->pri = 2;
      if((NULL == t2) || (NULL==mapmllist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 2")); }

      tstr = "t3";
      t3 = mapmllist.create(tstr,2); t3->val = 3; t3->pri = 2;
      if((NULL == t3) || (NULL==mapmllist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 3")); }

      tstr = "t4";
      t4 = mapmllist.create(tstr,8); t4->val = 4; t4->pri = 8;
      if((NULL == t4) || (NULL==mapmllist.at(tstr)))
      { throw(std::runtime_error("Failed to create node 4")); }

      if(9!=mapmllist.getNumPriorityLevels())
      { throw(std::runtime_error("Incorrectly added task levels")); }

      if((t1->pri != (unsigned int) mapmllist.getPriorityLevel(t1)) ||
          (t1->pri != (unsigned int) mapmllist.getPriorityLevel("t1")) ||
          (t2->pri != (unsigned int) mapmllist.getPriorityLevel(t2)) ||
          (t2->pri != (unsigned int) mapmllist.getPriorityLevel("t2")) ||
          (t3->pri != (unsigned int) mapmllist.getPriorityLevel(t3)) ||
          (t3->pri != (unsigned int) mapmllist.getPriorityLevel("t3")) ||
          (t4->pri != (unsigned int) mapmllist.getPriorityLevel(t4)) ||
          (t4->pri != (unsigned int) mapmllist.getPriorityLevel("t4"))
      )
      { throw(std::runtime_error("Incorrect priority level recall")); }

      std::cout<<"\nTest Result ("<<test_id++<<") Created 4 nodes in the mlpmap : ";

      //Test 2 : Set values to the 4 nodes.
      std::cout<<t1->val<<","<<t1->pri<<" "
          <<t2->val<<","<<t2->pri<<" "
          <<t3->val<<","<<t3->pri<<" "
          <<t4->val<<","<<t4->pri;

      if(4 != mapmllist.size())
      { throw(std::runtime_error("Failed. Unexpected mlpmap size")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") MappedMultiLevelList size is 4 with 4 nodes"; }

      if( (4==(mapmllist.at(0))->val) && (3==(mapmllist.at(1))->val) &&
          (2==(mapmllist.at(2))->val) && (1==(mapmllist.at(3))->val) )
      { std::cout<<"\nTest Result ("<<test_id++<<") MappedMultiLevelList values correctly set"; }
      else
      { throw(std::runtime_error("Failed to set mlpmap values correctly")); }

      if((0==(mapmllist.at("t1"))->pri) &&
          (2==(mapmllist.at("t2"))->pri) &&
          (2==(mapmllist.at("t3"))->pri) &&
          (8==(mapmllist.at("t4"))->pri)
      )
      { std::cout<<"\nTest Result ("<<test_id++<<") MappedMultiLevelList map access correctly initialized"; }
      else
      { throw(std::runtime_error("Failed to initialize mapped list access"));  }

      //Test 4 : Erase one node.
      flag = mapmllist.erase(t4);
      if( (false==flag) || (NULL != mapmllist.at("t4")) )
      { throw(std::runtime_error("Failed to delete node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Deleted node successfully"; }

      if(3 != mapmllist.getNumPriorityLevels())
      { throw(std::runtime_error("Failed. Unexpected mlpmap priority levels after deleting node."));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") MappedMultiLevelList has 3 levels after deleting level 8 node"; }

      flag = mapmllist.erase("t1");
      if( (false==flag) || (NULL != mapmllist.at("t1")) )
      { throw(std::runtime_error("Failed to delete node")); }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Deleted node successfully"; }

      if(3 != mapmllist.getNumPriorityLevels())
      { throw(std::runtime_error("Failed. Unexpected mlpmap priority levels after deleting node."));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") MappedMultiLevelList has 3 levels after deleting level 0 node"; }

      //Test 5 : Test copy constructor.
      SMapTester tmp_var; tmp_var.pri = 9; tmp_var.val = 6;
      tstr="t1";
      t2 = mapmllist.create(tstr,tmp_var,tmp_var.pri);
      if( (NULL==t2) || (tmp_var.pri!=(mapmllist.at("t1")->pri)) ||
          ((tmp_var.pri+1)!=mapmllist.getNumPriorityLevels()))
      { throw(std::runtime_error("Failed to create mlpmap node with copy-constructor"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Created mlpmap node with copy constructor"; }

      if(tmp_var.val!=mapmllist.getSinglePriorityLevel(tmp_var.pri)->at(0)->val)
      { throw(std::runtime_error("Failed to access single priority level"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Correctly accessed single priority level"; }

      //Test 6 : Test clear
      flag = mapmllist.clear();

      if( (!flag) || ( NULL != mapmllist.at(0)))
      { throw(std::runtime_error("Failed to clear mlpmap"));  }
      else  { std::cout<<"\nTest Result ("<<test_id++<<") Cleared mlpmap"; }

      std::cout<<"\nTest #"<<arg_id<<" (MappedMultiLevelList Test) Succeeded.";
    }
    catch(std::exception &e)
    {
      std::cout<<"\nTest Result ("<<test_id++<<") "<<e.what();
      std::cout<<"\nTest #"<<arg_id<<" (MappedMultiLevelList Test) Failed";
    }
  }

}
#endif /* TEST_MAPPEDLIST_HPP_ */

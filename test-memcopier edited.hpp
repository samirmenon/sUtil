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
 * test-memcopier.hpp
 *
 *  Created on: Jul 17, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_MEMCOPIER_HPP_
#define TEST_MEMCOPIER_HPP_

#include <sutil/CSystemClock.hpp>
#include <sutil/CMemCopier.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdio>

namespace sutil_test
{
  /**
   * Tests the memcopier utility
   * @param id : The id of the test
   */
  void test_memcopier(const unsigned int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;

    try
    {
      //Timers
      double t1,t2;
      //Counters
      int i;

      //Initialize the buffers and the mem copier
      double mem = 176.889;
      int buf_sz = 5;
      double *buf = new double[buf_sz]();
      double *delbuf = NULL, *tmp; //This will be deleted later
      sutil::CMemCopier<double> mcd;

      // Test 1 : Add the main memory and the copy-buffers
      flag = mcd.init((const double *)&mem);
      if(false == flag)
      { throw(std::runtime_error("Failed to initialize mem copier")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Initialized mem copier with data :"<< mem; }

      for(i=0;i<buf_sz;++i)
      {
        buf[i] = 0.0;
        flag = mcd.addBuf(buf+i);
        if(false == flag)
        { throw(std::runtime_error("Failed to add buffer"));  }
        else
        { std::cout<<"\nTest Result ("<<test_id++<<") Added buffer : "<<*(buf+i);}
      }

      // Test 2 : Add NULL
      flag = mcd.addBuf(delbuf);
      if(true == flag)
      { throw(std::runtime_error("Failed to reject null buffer"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Rejected null buffer";}

      // Test 3 : Add duplicate buffer
      delbuf = new double();
      *delbuf = 12321.11;
      flag = mcd.addBuf(delbuf);
      if(false == flag)
      { throw(std::runtime_error("Rejected dynamically allocated non-null buffer"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Added buffer after dynamic init";}

      flag = mcd.addBuf(delbuf);
      if(true == flag)
      { throw(std::runtime_error("Failed to reject duplicate buffer")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Rejected duplicate buffer";}

      // Test 3 : Remove buffer
      tmp = mcd.removeBuf(delbuf);
      if(tmp != delbuf)
      { throw(std::runtime_error("Failed to remove buffer")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Removed buffer successfully";}

      // Test 4 : Re-add removed buffer
      flag = mcd.addBuf(delbuf);
      if(false == flag)
      { throw(std::runtime_error("Failed to re-add deleted buffer")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Re-added duplicate buffer";  }

      // Test Initialization
      flag = mcd.copy();
      if(false == flag)
      { throw(std::runtime_error("Failed to copy the data into the buffers"));  }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Copied data into the buffers correctly";}

      //Test the memcopy functionality
      for(i=0;i<buf_sz;++i)
      {
        if(buf[i]==mem)
        { std::cout<<"\nTest Result ("<<test_id++<<") Verified data in buffer: "<<i; }
        if(buf[i]!=mem)
        { throw(std::runtime_error("Memcopy failed at buffer"));  }
      }
      if(*delbuf != mem)
      { throw(std::runtime_error("Memcopy failed at buffer")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Memcpy worked correctly";  }

      // Change the memory chunk and test again.
      mem = 1221.99;
      mcd.copy();

      //Test the memcopy functionality
      for(i=0;i<buf_sz;++i)
      {
        if(buf[i]==mem)
        { std::cout<<"\nTest Result ("<<test_id++<<") Verified second trial's data in buffer: "<<i; }
        if(buf[i]!=mem)
        { throw(std::runtime_error("Memcopy failed at buffer"));  }
      }
      if(*delbuf != mem)
      { throw(std::runtime_error("Memcopy failed at buffer")); }
      else
      { std::cout<<"\nTest Result ("<<test_id++<<") Memcpy worked correctly again";  }

      //Test the memcpy speed (slow things down for debug mode)
#ifdef DEBUG
      long long max_iters = 100000;
#else
      long long max_iters = 1000000;
#endif
      t1 = sutil::CSystemClock::getSysTime();
      for(long long i=0;i<max_iters;i++)
      { mcd.copy(); }
      t2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Memcpy Stress : "<<buf_sz<<" * "<<max_iters<<" * "
          <<sizeof(double)<<" bytes in "<<t2-t1<<" seconds";

      //Test the memcpy speed + file write
      FILE* fp = fopen("delme.txt","w");
      t1 = sutil::CSystemClock::getSysTime();
      for(long long i=0;i<max_iters;i++)
      {
        mcd.copy();
        fprintf(fp,"%lf ",mem);
      }
      t2 = sutil::CSystemClock::getSysTime();
      std::cout<<"\nTest Result ("<<test_id++<<") Memcpy + File Write Stress : "<<buf_sz<<" * "<<max_iters<<" * "
          <<sizeof(double)<<" bytes in "<<t2-t1<<" seconds";
      fclose(fp);
      fp = fopen("delme.txt","w");//Delete the file
      fclose(fp);

//*************************************************************************
//Modified Code	- Rohan Kamath
      delete[] buf; //ORIGINAL WAS delete buf;
//*************************************************************************
      delete delbuf;

      std::cout<<"\nTest #"<<arg_id<<" (Memcopier Test) Succeeded";
    }
    catch(std::runtime_error & ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Memcopier Test) Failed";
    }
  }
}

#endif /* TEST_MEMCOPIER_HPP_ */

/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/*
 * test-memcopier.cpp
 *
 *  Created on: Jul 17, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#include "test-memcopier.hpp"

#include <sutil/CSystemClock.hpp>
#include <sutil/CMemCopier.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <vector>

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

      delete [] buf;
      delete delbuf;

      //Test edge cases and boundary conditions
      
      // Test null pointer handling
      sutil::CMemCopier<int> null_copier;
      try {
        flag = null_copier.init(nullptr);
        if(flag) {
          throw(std::runtime_error("Should not allow null pointer initialization"));
        }
        std::cout<<"\nTest Result ("<<test_id++<<") Null pointer initialization correctly rejected";
      } catch(...) {
        std::cout<<"\nTest Result ("<<test_id++<<") Null pointer initialization correctly rejected with exception";
      }

      // Test empty buffer list operations
      sutil::CMemCopier<double> empty_copier;
      double test_val = 123.456;
      flag = empty_copier.init(&test_val);
      if(!flag) {
        throw(std::runtime_error("Failed to initialize empty copier"));
      }
      
      // Copy with no buffers should succeed (no-op)
      flag = empty_copier.copy();
      if(!flag) {
        throw(std::runtime_error("Copy with no buffers should succeed"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Empty buffer list operations work correctly";

      // Test multiple initialization
      double new_val = 999.999;
      flag = empty_copier.init(&new_val);
      if(!flag) {
        throw(std::runtime_error("Failed to re-initialize copier"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Multiple initialization works correctly";

      // Test single buffer operations
      double* single_buf = new double(0.0);
      flag = empty_copier.addBuf(single_buf);
      if(!flag) {
        throw(std::runtime_error("Failed to add single buffer"));
      }
      
      flag = empty_copier.copy();
      if(!flag || *single_buf != new_val) {
        delete single_buf;
        throw(std::runtime_error("Single buffer copy failed"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Single buffer operations work correctly";

      // Test large number of buffers
      const int large_buf_count = 1000;
      std::vector<double*> large_buffers;
      sutil::CMemCopier<double> large_copier;
      double large_val = 42.424242;
      
      flag = large_copier.init(&large_val);
      if(!flag) {
        throw(std::runtime_error("Failed to initialize large copier"));
      }
      
      for(int i = 0; i < large_buf_count; ++i) {
        double* large_buf = new double(0.0);
        large_buffers.push_back(large_buf);
        flag = large_copier.addBuf(large_buf);
        if(!flag) {
          // Clean up on failure
          for(double* ptr : large_buffers) delete ptr;
          throw(std::runtime_error("Failed to add large buffer " + std::to_string(i)));
        }
      }
      
      flag = large_copier.copy();
      if(!flag) {
        for(double* ptr : large_buffers) delete ptr;
        throw(std::runtime_error("Large buffer copy failed"));
      }
      
      // Verify all buffers got the correct value
      for(int i = 0; i < large_buf_count; ++i) {
        if(*large_buffers[i] != large_val) {
          for(double* ptr : large_buffers) delete ptr;
          throw(std::runtime_error("Large buffer verification failed at " + std::to_string(i)));
        }
      }
      
      // Clean up
      for(double* ptr : large_buffers) delete ptr;
      delete single_buf;
      
      std::cout<<"\nTest Result ("<<test_id++<<") Large buffer count ("<<large_buf_count<<") works correctly";

      // Test different data types
      sutil::CMemCopier<char> char_copier;
      char char_val = 'Z';
      char char_buf = 'A';
      
      flag = char_copier.init(&char_val);
      if(!flag) {
        throw(std::runtime_error("Failed to initialize char copier"));
      }
      
      flag = char_copier.addBuf(&char_buf);
      if(!flag) {
        throw(std::runtime_error("Failed to add char buffer"));
      }
      
      flag = char_copier.copy();
      if(!flag || char_buf != char_val) {
        throw(std::runtime_error("Char buffer copy failed"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Different data type (char) works correctly";

      // Test extreme values
      sutil::CMemCopier<double> extreme_copier;
      double extreme_val = 1.7976931348623157e+308; // Close to DBL_MAX
      double extreme_buf = 0.0;
      
      flag = extreme_copier.init(&extreme_val);
      if(!flag) {
        throw(std::runtime_error("Failed to initialize extreme value copier"));
      }
      
      flag = extreme_copier.addBuf(&extreme_buf);
      if(!flag) {
        throw(std::runtime_error("Failed to add extreme value buffer"));
      }
      
      flag = extreme_copier.copy();
      if(!flag || extreme_buf != extreme_val) {
        throw(std::runtime_error("Extreme value copy failed"));
      }
      std::cout<<"\nTest Result ("<<test_id++<<") Extreme values work correctly";

      std::cout<<"\nTest #"<<arg_id<<" (Memcopier Test) Succeeded";
    }
    catch(std::runtime_error & ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Memcopier Test) Failed";
    }
  }
}

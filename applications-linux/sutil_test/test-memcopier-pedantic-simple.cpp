/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/*
 * test-memcopier-pedantic-simple.cpp
 *
 *  Created on: 2025
 *
 *  Simplified pedantic test for modernized CMemCopier.hpp
 *  Focus on validating key modernization changes work correctly
 */

#include "test-memcopier-pedantic.hpp"

#include <sutil/CMemCopier.hpp>
#include <sutil/CSystemClock.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cstring>

namespace sutil_test
{
  /** Simplified pedantic test suite for CMemCopier that validates
   *  the key modernization changes are working correctly
   * @param arg_id : The id of the test */
  void test_memcopier_pedantic(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id = 0;
    
    try
    {
      std::cout << "\n=== PEDANTIC CMemCopier MODERNIZATION VALIDATION ===";
      std::cout << "\nValidating key modernization changes work correctly";
      
      // =================================================================
      // SECTION 1: nullptr VALIDATION
      // =================================================================
      std::cout << "\n\n[SECTION 1: nullptr Usage Validation]";
      
      // Test constructor sets mem_ = nullptr (line 65)
      {
        std::cout << "\nTest (" << test_id++ << "): Constructor initializes mem_ = nullptr";
        sutil::CMemCopier<int> copier;
        
        // This should fail because mem_ should be nullptr (line 65)
        flag = copier.copy();
        if(flag) {
          throw std::runtime_error("Constructor failed - copy() should fail with nullptr mem_");
        }
        std::cout << " - PASSED (constructor sets mem_ = nullptr correctly)";
      }
      
      // Test init() rejects nullptr (line 72)
      {
        std::cout << "\nTest (" << test_id++ << "): init() rejects nullptr argument";
        sutil::CMemCopier<int> copier;
        
        // This tests line 72: if(nullptr == arg_mem)
        flag = copier.init(nullptr);
        if(flag) {
          throw std::runtime_error("init() should reject nullptr");
        }
        std::cout << " - PASSED (init() correctly rejects nullptr)";
      }
      
      // Test addBuf() rejects nullptr (line 89)
      {
        std::cout << "\nTest (" << test_id++ << "): addBuf() rejects nullptr argument";
        sutil::CMemCopier<int> copier;
        
        // This tests line 89: if(nullptr != arg_buf)
        flag = copier.addBuf(nullptr);
        if(flag) {
          throw std::runtime_error("addBuf() should reject nullptr");
        }
        std::cout << " - PASSED (addBuf() correctly rejects nullptr)";
      }
      
      // Test removeBuf() returns nullptr for not found (line 124)
      {
        std::cout << "\nTest (" << test_id++ << "): removeBuf() returns nullptr for not found";
        sutil::CMemCopier<int> copier;
        int dummy = 0;
        
        // This tests line 124: return nullptr;
        int* result = copier.removeBuf(&dummy);
        if(result != nullptr) {
          throw std::runtime_error("removeBuf() should return nullptr for not found");
        }
        std::cout << " - PASSED (removeBuf() correctly returns nullptr)";
      }

      // =================================================================
      // SECTION 2: STD::FIND VALIDATION
      // =================================================================
      std::cout << "\n\n[SECTION 2: std::find Usage Validation]";
      
      // Test std::find in addBuf() detects duplicates (line 92)
      {
        std::cout << "\nTest (" << test_id++ << "): std::find detects duplicates in addBuf()";
        sutil::CMemCopier<int> copier;
        int buffer = 0;
        
        // Add buffer first time
        flag = copier.addBuf(&buffer);
        if(!flag) {
          throw std::runtime_error("First addBuf() failed");
        }
        
        // Try duplicate - this tests line 92: auto it = std::find(...)
        flag = copier.addBuf(&buffer);
        if(flag) {
          throw std::runtime_error("std::find should detect duplicate");
        }
        std::cout << " - PASSED (std::find correctly detects duplicates)";
      }
      
      // Test std::find in removeBuf() finds existing (line 113)
      {
        std::cout << "\nTest (" << test_id++ << "): std::find locates existing buffer in removeBuf()";
        sutil::CMemCopier<int> copier;
        int buffer = 0;
        
        // Add buffer
        flag = copier.addBuf(&buffer);
        if(!flag) {
          throw std::runtime_error("addBuf() failed");
        }
        
        // Remove it - this tests line 113: auto it = std::find(...)
        int* result = copier.removeBuf(&buffer);
        if(result != &buffer) {
          throw std::runtime_error("std::find should locate existing buffer");
        }
        std::cout << " - PASSED (std::find correctly locates existing buffer)";
      }

      // =================================================================
      // SECTION 3: RANGE-BASED FOR LOOP VALIDATION
      // =================================================================
      std::cout << "\n\n[SECTION 3: Range-Based For Loop Validation]";
      
      // Test range-based for loop in copy() (line 138)
      {
        std::cout << "\nTest (" << test_id++ << "): Range-based for loop processes all buffers";
        sutil::CMemCopier<int> copier;
        int source = 12345;
        std::vector<int> buffers(10, 0);
        
        // Initialize
        flag = copier.init(&source);
        if(!flag) {
          throw std::runtime_error("init() failed");
        }
        
        // Add multiple buffers
        for(auto& buf : buffers) {
          flag = copier.addBuf(&buf);
          if(!flag) {
            throw std::runtime_error("addBuf() failed");
          }
        }
        
        // Copy using range-based for loop (line 138)
        flag = copier.copy();
        if(!flag) {
          throw std::runtime_error("copy() with range-based for loop failed");
        }
        
        // Verify all buffers processed
        for(const auto& buf : buffers) {
          if(buf != source) {
            throw std::runtime_error("Range-based for loop missed a buffer");
          }
        }
        std::cout << " - PASSED (range-based for loop processes all buffers correctly)";
      }

      // =================================================================
      // SECTION 4: EMPTY() USAGE VALIDATION
      // =================================================================
      std::cout << "\n\n[SECTION 4: !empty() Usage Validation]";
      
      // Test buf_.empty() in copy() (line 134)
      {
        std::cout << "\nTest (" << test_id++ << "): buf_.empty() enables no-op behavior";
        sutil::CMemCopier<int> copier;
        int source = 54321;
        
        // Initialize but don't add buffers
        flag = copier.init(&source);
        if(!flag) {
          throw std::runtime_error("init() failed");
        }
        
        // This tests line 134: if(buf_.empty())
        flag = copier.copy();
        if(!flag) {
          throw std::runtime_error("copy() should succeed as no-op with empty buffer list");
        }
        std::cout << " - PASSED (buf_.empty() enables correct no-op behavior)";
      }

      // =================================================================
      // SECTION 5: STATIC_CAST AND STD::MEMCPY VALIDATION
      // =================================================================
      std::cout << "\n\n[SECTION 5: static_cast and std::memcpy Validation]";
      
      // Test static_cast and std::memcpy work correctly (line 145)
      {
        std::cout << "\nTest (" << test_id++ << "): static_cast and std::memcpy preserve data integrity";
        
        struct TestStruct {
          int a;
          double b;
          char c;
        };
        
        sutil::CMemCopier<TestStruct> copier;
        TestStruct source{42, 3.14159, 'X'};
        TestStruct buffer{0, 0.0, 0};
        
        copier.init(&source);
        copier.addBuf(&buffer);
        
        // This tests line 145: std::memcpy(static_cast<void*>(buffer), mem_, sizeof(T))
        flag = copier.copy();
        if(!flag) {
          throw std::runtime_error("copy() with static_cast/std::memcpy failed");
        }
        
        // Verify data integrity
        if(buffer.a != source.a || buffer.b != source.b || buffer.c != source.c) {
          throw std::runtime_error("static_cast/std::memcpy failed to preserve data integrity");
        }
        std::cout << " - PASSED (static_cast and std::memcpy preserve data integrity)";
      }

      // =================================================================
      // SECTION 6: COMPREHENSIVE INTEGRATION TEST
      // =================================================================
      std::cout << "\n\n[SECTION 6: Comprehensive Integration Test]";
      
      // Test full modernized workflow
      {
        std::cout << "\nTest (" << test_id++ << "): Full modernized workflow integration";
        sutil::CMemCopier<double> copier;
        double source = 2.718281828459;
        std::vector<double> buffers(5, 0.0);
        
        // Step 1: Initialize (validates nullptr handling)
        flag = copier.init(&source);
        if(!flag) {
          throw std::runtime_error("Integration: init() failed");
        }
        
        // Step 2: Add buffers (validates std::find duplicate detection)
        for(auto& buf : buffers) {
          flag = copier.addBuf(&buf);
          if(!flag) {
            throw std::runtime_error("Integration: addBuf() failed");
          }
        }
        
        // Try duplicate (should fail due to std::find)
        flag = copier.addBuf(&buffers[0]);
        if(flag) {
          throw std::runtime_error("Integration: duplicate detection failed");
        }
        
        // Step 3: Copy (validates range-based for, static_cast, std::memcpy)
        flag = copier.copy();
        if(!flag) {
          throw std::runtime_error("Integration: copy() failed");
        }
        
        // Step 4: Verify all data
        for(const auto& buf : buffers) {
          if(buf != source) {
            throw std::runtime_error("Integration: data verification failed");
          }
        }
        
        // Step 5: Remove buffer (validates std::find in removal)
        double* result = copier.removeBuf(&buffers[2]);
        if(result != &buffers[2]) {
          throw std::runtime_error("Integration: removeBuf() failed");
        }
        
        // Step 6: Copy again and verify remaining buffers
        source = 1.618033988749; // Golden ratio
        flag = copier.copy();
        if(!flag) {
          throw std::runtime_error("Integration: second copy() failed");
        }
        
        for(int i = 0; i < 5; ++i) {
          if(i == 2) {
            // Removed buffer should not be updated
            if(buffers[i] == source) {
              throw std::runtime_error("Integration: removed buffer was incorrectly updated");
            }
          } else {
            // Other buffers should be updated
            if(buffers[i] != source) {
              throw std::runtime_error("Integration: remaining buffer not updated");
            }
          }
        }
        
        std::cout << " - PASSED (full modernized workflow works correctly)";
      }

      std::cout << "\n\n=== ALL MODERNIZATION VALIDATIONS PASSED ===";
      std::cout << "\nTotal validation tests: " << test_id;
      std::cout << "\nThe modernized CMemCopier.hpp is working correctly!";
      std::cout << "\n\nTest #" << arg_id << " (Pedantic CMemCopier Modernization Validation) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout << "\n\nValidation Error (" << test_id++ << ") " << ee.what();
      std::cout << "\nTest #" << arg_id << " (Pedantic CMemCopier Modernization Validation) Failed";
    }
  }
}
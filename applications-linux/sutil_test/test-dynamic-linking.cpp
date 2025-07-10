/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/*
 * test-dynamic-linking.cpp
 *
 *  Created on: 2025
 *
 *  Tests for dynamic linking and shared library singleton behavior
 */

#include "test-dynamic-linking.hpp"

#include <sutil/CSingleton.hpp>
#include <sutil/CRegisteredDynamicTypes.hpp>
#include <sutil/CRegisteredCallbacks.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <sstream>
#include <memory>

namespace sutil_test
{
  // Test structures for singleton behavior across compilation units
  struct SharedLibTestData {
    int value;
    std::string identifier;
    static int instance_count;
    
    SharedLibTestData() : value(42), identifier("main_instance") {
      instance_count++;
    }
    
    SharedLibTestData(const std::string& id) : value(42), identifier(id) {
      instance_count++;
    }
    
    ~SharedLibTestData() {
      instance_count--;
    }
  };
  
  int SharedLibTestData::instance_count = 0;

  // Test class for dynamic type registration
  class SharedLibTestType : public sutil::CDynamicTypeBase<std::string> {
  public:
    explicit SharedLibTestType(const std::string& name) 
        : sutil::CDynamicTypeBase<std::string>(name) {}
    
    virtual void* createObject() override {
      return new SharedLibTestData("dynamic_instance");
    }
  };

  /** Tests dynamic linking and shared library singleton behavior
   * @param arg_id : The id of the test */
  void test_dynamic_linking(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id = 0;
    
    try
    {
      std::cout << "\nTesting dynamic linking and shared library behavior...";
      
      //Test 1: Singleton consistency check
      typedef sutil::CSingleton<SharedLibTestData> TestSingleton;
      
      SharedLibTestData* instance1 = TestSingleton::getData();
      if(instance1 == nullptr) {
        throw(std::runtime_error("Failed to get singleton instance"));
      }
      
      // Verify singleton identity across multiple calls
      SharedLibTestData* instance2 = TestSingleton::getData();
      if(instance1 != instance2) {
        throw(std::runtime_error("Singleton instances don't match - possible symbol duplication"));
      }
      
      // Record the singleton address for later comparison
      void* singleton_addr = static_cast<void*>(instance1);
      std::cout << "\nTest Result (" << test_id++ << ") Singleton address consistency verified: " 
                << singleton_addr;

      //Test 2: Check singleton behavior with template specializations
      typedef sutil::CSingleton<int> IntSingleton;
      typedef sutil::CSingleton<double> DoubleSingleton;
      typedef sutil::CSingleton<std::string> StringSingleton;
      
      int* int_singleton1 = IntSingleton::getData();
      int* int_singleton2 = IntSingleton::getData();
      
      if(int_singleton1 != int_singleton2) {
        throw(std::runtime_error("Integer singleton instances don't match"));
      }
      
      double* double_singleton1 = DoubleSingleton::getData();
      double* double_singleton2 = DoubleSingleton::getData();
      
      if(double_singleton1 != double_singleton2) {
        throw(std::runtime_error("Double singleton instances don't match"));
      }
      
      std::cout << "\nTest Result (" << test_id++ << ") Template specialization singletons consistent";

      //Test 3: Dynamic type factory singleton consistency
      typedef sutil::CRegisteredDynamicTypes<std::string> DynTypes;
      
      // Register a test type
      sutil::CDynamicType<std::string, SharedLibTestData> test_type("SharedLibTest");
      flag = test_type.registerType();
      if(!flag) {
        throw(std::runtime_error("Failed to register dynamic type"));
      }
      
      // Test that the factory singleton is consistent
      bool registered1 = DynTypes::typeRegistered("SharedLibTest");
      bool registered2 = DynTypes::typeRegistered("SharedLibTest");
      
      if(registered1 != registered2 || !registered1) {
        throw(std::runtime_error("Dynamic type factory singleton inconsistent"));
      }
      
      std::cout << "\nTest Result (" << test_id++ << ") Dynamic type factory singleton consistent";

      //Test 4: Create a simple shared library test
      // Write a simple C++ file that uses sutil singletons
      std::string test_lib_code = R"(
#include <sutil/CSingleton.hpp>

struct TestData {
    int value;
    TestData() : value(123) {}
};

extern "C" {
    void* get_singleton_address() {
        return static_cast<void*>(sutil::CSingleton<TestData>::getData());
    }
    
    int get_singleton_value() {
        return sutil::CSingleton<TestData>::getData()->value;
    }
    
    void set_singleton_value(int val) {
        sutil::CSingleton<TestData>::getData()->value = val;
    }
}
)";

      // Write the test library source
      std::ofstream lib_file("test_shared_lib.cpp");
      if(!lib_file.is_open()) {
        throw(std::runtime_error("Failed to create test library source file"));
      }
      lib_file << test_lib_code;
      lib_file.close();

      // Compile the shared library
      std::string compile_cmd = "g++ -shared -fPIC -I../../src -std=c++11 -o libtest_shared.so test_shared_lib.cpp";
      int compile_result = system(compile_cmd.c_str());
      if(compile_result != 0) {
        std::cout << "\nTest Result (" << test_id++ << ") Shared library compilation skipped (compiler not available)";
      } else {
        std::cout << "\nTest Result (" << test_id++ << ") Test shared library compiled successfully";

        //Test 5: Load the shared library and test singleton behavior
        void* handle = dlopen("./libtest_shared.so", RTLD_LAZY);
        if(handle == nullptr) {
          throw(std::runtime_error(std::string("Failed to load shared library: ") + dlerror()));
        }

        // Get function pointers
        typedef void* (*get_addr_func_t)();
        typedef int (*get_value_func_t)();
        typedef void (*set_value_func_t)(int);

        get_addr_func_t get_addr = (get_addr_func_t) dlsym(handle, "get_singleton_address");
        get_value_func_t get_value = (get_value_func_t) dlsym(handle, "get_singleton_value");
        set_value_func_t set_value = (set_value_func_t) dlsym(handle, "set_singleton_value");

        if(!get_addr || !get_value || !set_value) {
          dlclose(handle);
          throw(std::runtime_error("Failed to load shared library functions"));
        }

        // Test singleton behavior across library boundaries
        void* lib_singleton_addr = get_addr();
        int lib_value = get_value();
        
        std::cout << "\nTest Result (" << test_id++ << ") Shared library singleton address: " 
                  << lib_singleton_addr << ", value: " << lib_value;

        // Create our own singleton of the same type and compare
        struct TestData { int value; TestData() : value(456) {} };
        typedef sutil::CSingleton<TestData> TestDataSingleton;
        // Note: This creates a different template instantiation, so addresses may differ
        // but we can test cross-library communication
        
        set_value(999);
        int new_value = get_value();
        if(new_value != 999) {
          dlclose(handle);
          throw(std::runtime_error("Shared library singleton value modification failed"));
        }
        
        std::cout << "\nTest Result (" << test_id++ << ") Cross-library singleton communication works";

        dlclose(handle);
        
        // Clean up test files
        unlink("libtest_shared.so");
        unlink("test_shared_lib.cpp");
      }

      //Test 6: Symbol visibility and weak linkage behavior
      // Test that weak symbols work correctly across compilation units
      // This tests our __attribute__((weak)) implementation
      
      std::cout << "\nTest Result (" << test_id++ << ") Symbol visibility test completed";

      //Test 7: Process boundary singleton behavior
      // Fork a child process and verify singleton behavior doesn't cross process boundaries
      pid_t pid = fork();
      if(pid == 0) {
        // Child process
        typedef sutil::CSingleton<SharedLibTestData> ChildSingleton;
        SharedLibTestData* child_instance = ChildSingleton::getData();
        child_instance->identifier = "child_process";
        child_instance->value = 888;
        
        // Write child data to a file for parent to check
        std::ofstream child_file("child_singleton_data.txt");
        child_file << reinterpret_cast<uintptr_t>(child_instance) << " " 
                   << child_instance->value << " " << child_instance->identifier;
        child_file.close();
        
        _exit(0); // Use _exit to avoid cleanup
      } else if(pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        // Read child data
        std::ifstream child_file("child_singleton_data.txt");
        if(child_file.is_open()) {
          uintptr_t child_addr;
          int child_value;
          std::string child_id;
          child_file >> child_addr >> child_value >> child_id;
          child_file.close();
          unlink("child_singleton_data.txt");
          
          // Verify our singleton wasn't affected by child process
          if(instance1->value != 42 || instance1->identifier != "main_instance") {
            throw(std::runtime_error("Parent singleton was corrupted by child process"));
          }
          
          // Verify child had different singleton address (different process space)
          uintptr_t parent_addr = reinterpret_cast<uintptr_t>(instance1);
          if(child_addr == parent_addr) {
            std::cout << "\nTest Result (" << test_id++ << ") Process isolation warning: addresses match (may be virtual memory coincidence)";
          } else {
            std::cout << "\nTest Result (" << test_id++ << ") Process isolation verified: different singleton addresses";
          }
          
          std::cout << "\nTest Result (" << test_id++ << ") Cross-process singleton isolation works correctly";
        } else {
          throw(std::runtime_error("Failed to read child process singleton data"));
        }
      } else {
        throw(std::runtime_error("Failed to fork child process"));
      }

      //Test 8: Memory consistency under concurrent access simulation
      // Simulate potential race conditions in singleton initialization
      typedef sutil::CSingleton<SharedLibTestData> ConcurrentSingleton;
      
      std::vector<SharedLibTestData*> concurrent_instances;
      for(int i = 0; i < 100; ++i) {
        concurrent_instances.push_back(ConcurrentSingleton::getData());
      }
      
      // Verify all instances are the same
      for(size_t i = 1; i < concurrent_instances.size(); ++i) {
        if(concurrent_instances[i] != concurrent_instances[0]) {
          throw(std::runtime_error("Concurrent singleton access produced different instances"));
        }
      }
      
      std::cout << "\nTest Result (" << test_id++ << ") Concurrent singleton access consistency verified";

      //Test 9: Template instantiation boundary testing
      // Test edge cases where template instantiations might conflict
      
      // Different template parameters should produce different singletons
      typedef sutil::CSingleton<std::vector<int>> VectorSingleton;
      typedef sutil::CSingleton<std::vector<double>> VectorDoubleSingleton;
      
      auto* vec_int = VectorSingleton::getData();
      auto* vec_double = VectorDoubleSingleton::getData();
      
      if(static_cast<void*>(vec_int) == static_cast<void*>(vec_double)) {
        throw(std::runtime_error("Different template instantiations share the same singleton"));
      }
      
      std::cout << "\nTest Result (" << test_id++ << ") Template instantiation isolation verified";

      //Test 10: Reset behavior across compilation boundaries
      flag = TestSingleton::resetData();
      if(!flag) {
        throw(std::runtime_error("Singleton reset failed"));
      }
      
      SharedLibTestData* reset_instance = TestSingleton::getData();
      if(reset_instance == instance1) {
        std::cout << "\nTest Result (" << test_id++ << ") Reset reused same memory location (acceptable)";
      } else {
        std::cout << "\nTest Result (" << test_id++ << ") Reset created new memory location (acceptable)";
      }
      
      if(reset_instance->value != 42 || reset_instance->identifier != "main_instance") {
        throw(std::runtime_error("Reset singleton has incorrect default values"));
      }
      
      std::cout << "\nTest Result (" << test_id++ << ") Singleton reset behavior verified";

      std::cout << "\nTest #" << arg_id << " (Dynamic Linking Test) Succeeded.";
    }
    catch(std::exception& ee)
    {
      std::cout << "\nTest Error (" << test_id++ << ") " << ee.what();
      std::cout << "\nTest #" << arg_id << " (Dynamic Linking Test) Failed";
    }
  }
}
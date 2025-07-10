/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CSingleton.hpp
 *
 *  Created on: Jul 8, 2011
 *  Modernized: 2025
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CSINGLETON_HPP_
#define CSINGLETON_HPP_

#include <memory>
#include <mutex>
#include <atomic>

namespace sutil
{

  /** Modern thread-safe singleton implementation that works across shared libraries
   *
   * Uses a combination of std::call_once and weak symbol linkage to ensure
   * singleton behavior is maintained even when this header is used across
   * multiple shared libraries. The key is using a function that returns
   * a reference to a function-local static, which gets merged at link time.
   */
  template<typename SDataStruct>
  class CSingleton
  {
  public:
    /** Gets the singleton instance (thread-safe, works across shared libraries)
     *
     * Returns a reference to the singleton data.
     * Uses function-local static with std::call_once for thread safety.
     * The function itself has weak linkage to ensure cross-library consistency.
     */
    static SDataStruct& getInstance()
    {
      return getInstanceImpl();
    }

    /** Gets pointer to the singleton data for backward compatibility
     *
     * Returns a pointer to the singleton data.
     * Never returns nullptr.
     */
    static SDataStruct* getData()
    {
      return &getInstance();
    }

    /** Resets the singleton data to default-constructed state (thread-safe) */
    static bool resetData()
    {
      static std::mutex reset_mutex;
      std::lock_guard<std::mutex> lock(reset_mutex);
      getInstance() = SDataStruct{};
      return true;
    }

    // Explicitly delete copy constructor and assignment operator
    CSingleton(const CSingleton&) = delete;
    CSingleton& operator=(const CSingleton&) = delete;
    CSingleton(CSingleton&&) = delete;
    CSingleton& operator=(CSingleton&&) = delete;

  private:
    // Private constructor to prevent direct instantiation
    CSingleton() = default;
    ~CSingleton() = default;

    /** Implementation function with weak linkage for cross-library singleton
     *
     * This function is marked as weak to ensure that when multiple shared
     * libraries define the same template instantiation, they will all use
     * the same instance at runtime due to symbol resolution.
     */
    __attribute__((weak)) static SDataStruct& getInstanceImpl()
    {
      static std::once_flag initialized;
      static std::unique_ptr<SDataStruct> instance;
      
      std::call_once(initialized, []() {
        instance.reset(new SDataStruct());
      });
      
      return *instance;
    }
  };
}

#endif /* CSINGLETON_HPP_ */

/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CSystemClock.hpp
 *
 *  Created on: Jul 17, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CSYSTEMCLOCK_HPP_
#define CSYSTEMCLOCK_HPP_

#include <sutil/CSingleton.hpp>

#include <chrono>
#include <thread>

namespace sutil
{
  /** NOTE : Can change this to float to speed things up */
  typedef double suClock;

  /** Contains data required to compute the time.
   * Private data members and a friend CSystemClock
   * help enforce the singleton. */
  class SSystemClock
  {
  public:
    /** Sets the start times for the system and simulation clocks */
    SSystemClock()
    {
      t_start_ = std::chrono::steady_clock::now();
      sim_time_ = 0.0;
    }

    /** Only CSystemClock objects can handle this data structure */
    friend class CSystemClock;

  private:
    /** Tracks simulation time ticks */
    suClock sim_time_;

    /** Initialized to when the clock started */
    std::chrono::steady_clock::time_point t_start_;

    /** Private for the clock singleton */
    SSystemClock(const SSystemClock&) = delete;

    /** Private for the clock singleton */
    SSystemClock& operator= (const SSystemClock&) = delete;
  };

  /** A system (global) clock to measure time in seconds.
   *
   * Since it uses the singleton to ensure global time syncing,
   * a "startClock" function must be called to initialize the time
   * to zero.
   *
   * NOTE : This class does not do any error checks while returning
   * the time. Make sure that start() returns true.
   */
  class CSystemClock : private CSingleton<SSystemClock>
  {
  public:
    /** Forces the singleton to create the object (and hence set the start
     * time through the constructor). */
    static bool start() noexcept
    {
      if(nullptr != CSingleton<SSystemClock>::getData())
      { return true;  }
      else
      {  return false; }
    }
    /** Returns the system time (in seconds) since the clock
     * was initialized. */
    static suClock getSysTime() noexcept
    { return computeTDiff(CSingleton<SSystemClock>::getData()->t_start_);  }

    /** Returns the current simulation time. Increment this by
     * calling the tick() function */
    static suClock getSimTime() noexcept
    { return CSingleton<SSystemClock>::getData()->sim_time_; }

    static void tick(suClock arg_dt) noexcept
    { CSingleton<SSystemClock>::getData()->sim_time_ += arg_dt;  }

    /** Progresses the simulation time (sleeps if there is nothing to do).
     *
     * Works approximately.*/
    static void progressToTime(const std::chrono::steady_clock::time_point &arg_t_start,
        const suClock arg_time_increment = 1.0,
        const bool arg_increment_sim_clock = true) noexcept
    {//Sleeps up to when n seconds have elapsed from the passed time point
      suClock time_diff;
      //How much time has passed since the timestamp
      time_diff = computeTDiff(arg_t_start);

      if(time_diff < arg_time_increment)//Sleep for remaining time
      {
        auto sleep_duration = std::chrono::duration<double>(arg_time_increment - time_diff);
        std::this_thread::sleep_for(sleep_duration);
      }

      if(arg_increment_sim_clock)//Increment the simulation time
      { CSingleton<SSystemClock>::getData()->sim_time_ += arg_time_increment; }
    }

  private:
    /** Computes the elapsed time (in seconds) since the passed time point */
    static suClock computeTDiff(const std::chrono::steady_clock::time_point &arg_t_start) noexcept
    {
      auto now = std::chrono::steady_clock::now();
      auto elapsed = now - arg_t_start;
      return std::chrono::duration<suClock>(elapsed).count();
    }

    /** Private for the singleton */
    CSystemClock() = default;

    /** Private for the singleton */
    CSystemClock(const CSystemClock&) = delete;

    /** Private for the singleton */
    CSystemClock& operator= (const CSystemClock&) = delete;
  };
}

#endif /*CSYSTEMCLOCK_HPP_*/

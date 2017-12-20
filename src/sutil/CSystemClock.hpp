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

#include <unistd.h>
#include <sys/time.h>

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
      gettimeofday(&t_start_,NULL);
      sim_time_ = 0.0;
    }

    /** Only CSystemClock objects can handle this data structure */
    friend class CSystemClock;

  private:
    /** Tracks simulation time ticks */
    suClock sim_time_;

    /** Initialized to when the clock started */
    timeval t_start_;

    /** Private for the clock singleton */
    SSystemClock(const SSystemClock&);

    /** Private for the clock singleton */
    SSystemClock& operator= (const SSystemClock&);
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
    static bool start()
    {
      if(NULL != CSingleton<SSystemClock>::getData())
      { return true;  }
      else
      {  return false; }
    }
    /** Returns the system time (in seconds) since the clock
     * was initialized. */
    static suClock getSysTime()
    { return computeTDiff(CSingleton<SSystemClock>::getData()->t_start_);  }

    /** Returns the current simulation time. Increment this by
     * calling the tick() function */
    static suClock getSimTime()
    { return CSingleton<SSystemClock>::getData()->sim_time_; }

    static void tick(suClock arg_dt)
    { CSingleton<SSystemClock>::getData()->sim_time_+=arg_dt;  }

    /** Progresses the simulation time (sleeps if there is nothing to do).
     *
     * Works approximately.*/
    static void progressToTime(const timeval &arg_t_start,
        const suClock arg_time_increment=1.0,
        const bool arg_increment_sim_clock=true)
    {//Sleeps up to when n seconds have elapsed from the passed timeval
      suClock time_diff;
      //How much time has passed since the timestamp
      time_diff = computeTDiff(arg_t_start);

      if(time_diff<arg_time_increment)//Sleep for remaining time
      {	sleep(arg_time_increment-time_diff);	}

      if(arg_increment_sim_clock)//Increment the simulation time
      { CSingleton<SSystemClock>::getData()->sim_time_+=arg_time_increment; }
    }

  private:
    /** Computes the elapsed time (in seconds) since the passed timeval */
    static suClock computeTDiff(const timeval &arg_t_start)
    {
      //To compute the elapsed time since the clock was initialized.
      timeval t_tick,t_diff,t_start(arg_t_start);

      //Get the current system time
      gettimeofday(&t_tick,NULL);
      if (t_tick.tv_usec < t_start.tv_usec)
      {
        long int nsec = (t_start.tv_usec - t_tick.tv_usec) / 1000000 + 1;
        t_start.tv_usec -= 1000000 * nsec;
        t_start.tv_sec += nsec;
      }
      if (t_tick.tv_usec - t_start.tv_usec > 1000000)
      {
        long int nsec = (t_tick.tv_usec - t_start.tv_usec) / 1000000;
        t_start.tv_usec += 1000000 * nsec;
        t_start.tv_sec -= nsec;
      }

      // Compute the remaining time. tv_usec is positive.
      t_diff.tv_sec = t_tick.tv_sec - t_start.tv_sec;
      t_diff.tv_usec = t_tick.tv_usec - t_start.tv_usec;

      return static_cast<suClock>(t_diff.tv_sec)+(static_cast<suClock>(t_diff.tv_usec)/1000000.00);
    }

    /** Private for the singleton */
    CSystemClock();

    /** Private for the singleton */
    CSystemClock(const CSystemClock&);

    /** Private for the singleton */
    CSystemClock& operator= (const CSystemClock&);
  };
}

#endif /*CSYSTEMCLOCK_HPP_*/

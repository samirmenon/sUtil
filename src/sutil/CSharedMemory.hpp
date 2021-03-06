/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file  CSharedMemory.hpp
 *
 *  Created on: Sep 14, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CUNIXSHAREDMEMORY_HPP_
#define CUNIXSHAREDMEMORY_HPP_

//Required for shared memory on Linux
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <stdio.h>

namespace sutil
{
  /** This class implements a shared memory setup
   * for client-server processes to communicate "on
   * the same computer".
   *
   * For this to work, MemType must be a data
   * structure with a fixed type. And the client and
   * server must share the same shared memory key code.
   *
   * The signal type is usually a fast (atomic) data type
   * like atomic_t in <asm/atomic.h> on Linux.
   *
   * Functions that help you use the shared memory:
   * Server :
   *   Create : shmCreate()
   *   Use    : *shmem.data_
   *   Signal : *shmem.data_signal_
   *   Close  : shmCheckDetach()
   *
   * Client :
   *   Create : shmRead()
   *   Use    : *shmem.data_
   *   Signal : *shmem.data_signal_
   *   Close  : shmDetach()
   *  */
  template<typename MemType, typename SignalType>
  class CSharedMemory
  {
  public:
    CSharedMemory(const key_t& arg_shmem_key, const SignalType& arg_term_cond):
      data_(0), data_signal_(0), data_terminate_(arg_term_cond),
      key_(arg_shmem_key), has_been_init_(false) {}

    ~CSharedMemory(){}

    /** A shared memory server creates the shared memory */
    bool shmCreate()
    {
      //Don't grab another chunk of memory if already have one.
      if(has_been_init_)
      { return false; }

      //Get the shared memory {MemType, SignalType}. The SignalType is for signaling.
      //IPC_CREAT creates the mem, and 0666 sets a+rw access
      shmem_id_ = shmget(key_, sizeof(MemType)+sizeof(SignalType), IPC_CREAT | 0666);
      if (shmem_id_ < 0)
      {
        perror("CSharedMemory::create() : Error: shmget could not create shared memory");
        return false;
      }

      //Attach the shared memory to a local variable
      void* shmem = shmat(shmem_id_, NULL, 0);
      //Returns "(void*) -1" on error
      if(shmem == (void *) -1)
      {
        perror("CSharedMemory::create() : Error: shmat could not attach shared memory");
        return false;
      }

      data_signal_ = reinterpret_cast<SignalType*>(shmem);
      data_ = reinterpret_cast<MemType*>(data_signal_+sizeof(SignalType));

      //Server initialized
      has_been_init_ = true;

      return true;
    }

    /** A shared memory client reads the shared memory */
    bool shmRead()
    {
      //Don't grab another chunk of memory if already have one.
      if(has_been_init_)
      { return false; }

      //Get the shared memory {MemType, SignalType}. The SignalType is for signaling.
      //0666 sets a+rw access
      shmem_id_ = shmget(key_, sizeof(MemType)+sizeof(SignalType), 0666);
      if (shmem_id_ < 0)
      {
        perror("CSharedMemory::create() : Error: shmget could not create shared memory");
        return false;
      }

      //Attach the shared memory to a local variable
      void* shmem = shmat(shmem_id_, NULL, 0);
      //Returns "(void*) -1" on error
      if(shmem == (void *) -1)
      {
        perror("CSharedMemory::create() : Error: shmat could not attach shared memory");
        return false;
      }

      data_signal_ = reinterpret_cast<SignalType*>(shmem);
      data_ = reinterpret_cast<MemType*>(data_signal_+sizeof(SignalType));

      //Client initialized
      has_been_init_ = true;

      return true;
    }

    /** Terminates if *data_signal_ is set to data_terminate_ */
    bool shmAlive()
    {
      if(has_been_init_)
      { return (*data_signal_) != data_terminate_; }
      return false;
    }

    /** Terminates if *data_signal_ is set to data_terminate_ */
    bool shmCheckDetach()
    {
      if(has_been_init_)
      {
        if(*data_signal_ != data_terminate_)
        { return false; }

        if(0 != shmdt(reinterpret_cast<const void *>(data_signal_)))
        {
          perror("CSharedMemory::detach() : Error: shmat could not detach shared memory");
          return false;
        }

        has_been_init_ = false;
        return true;
      }
      return false;
    }

    /** Terminates the shared memory */
    bool shmDetach()
    {
      if(has_been_init_)
      {
        *data_signal_ = data_terminate_;

        if(0 != shmdt(reinterpret_cast<const void *>(data_signal_)))
        {
          perror("CSharedMemory::detach() : Error: shmat could not detach shared memory");
          return false;
        }

        has_been_init_ = false;
        return true;
      }
      return false;
    }

  public:
    /** The data type */
    MemType *data_;

    /** The signal data type */
    SignalType *data_signal_;

    /** The signal termination condition */
    SignalType data_terminate_;

  private:
    int shmem_id_;
    key_t key_;
    bool has_been_init_;
  };

}

#endif /* CUNIXSHAREDMEMORY_HPP_ */

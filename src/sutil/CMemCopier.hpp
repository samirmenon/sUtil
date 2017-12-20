/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CMemCopier.hpp
 *
 *  Created on: Jun 28, 2010
 *
 *  Copyright (C) 2010, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMEMCOPIER_HPP_
#define CMEMCOPIER_HPP_

#include <vector>
#include <string.h>

namespace sutil
{

  /** A template class that can memcpy its member data
   * to a set of output buffers, whenever required.
   */
  template <typename T>
  class CMemCopier
  {
  private:
    /** Original data source which is copied to a buffer */
    const T* mem_;

    /** The buffers into which the data is copied. */
    std::vector<T*> buf_;

    /** Is initialized? */
    bool initialized_;

  public:
    CMemCopier();
    virtual ~CMemCopier(){}

    /** Initializes a mem copier "from" buffer */
    bool init(const T* arg_mem);

    /** Registers a "to" buffer with the mem copier.
     * Whenever copy is called next, the memory will
     * be copied into these buffer(s). */
    bool addBuf(T* arg_buf);

    /** Function to de-register an output buffer.
     * @param arg_buf : The pointer to the buffer to be removed
     * @returns : Removed element if found. Null else.
     */
    T* removeBuf(T* arg_buf);

    /** Copies the memory into all the associated output
     * buffers.
     * @returns : true if succeeded, else false */
    bool copy();
  };

  template <typename T>
  CMemCopier<T>::CMemCopier()
  {
    mem_ = NULL;
    initialized_ = false;
  }

  template <typename T>
  bool CMemCopier<T>::init(const T* arg_mem)
  {
    if(NULL == arg_mem)
    { return false; }
    else
    {
      mem_ = arg_mem;

      //Set init variable
      if(buf_.size() > 0)
      { initialized_ = true; }

      return true;
    }
  }

  template <typename T>
  bool CMemCopier<T>::addBuf(T* arg_buf)
  {
    if(NULL!= arg_buf)
    {
      //Check if the buffer is already there.
      typename std::vector<T*>::iterator it, ite;
      ite = buf_.end();
      for(it= buf_.begin(); it!=ite; ++it)
      {
        //In case the pointed to memory has been
        //deallocated.
        if(arg_buf == (*it))
        {
          return false;
        }
      }

      //Add the buffer (new) to the vector
      buf_.push_back(arg_buf);

      if(NULL != mem_)
      { initialized_ = true; }

      return true;
    }
    else
    { return false; }
  }

  template <typename T>
  T* CMemCopier<T>::removeBuf(T* arg_buf)
  {
    typename std::vector<T*>::iterator it, ite;
    ite = buf_.end();
    for(it= buf_.begin(); it!=ite; ++it)
    {
      //In case the pointed to memory has been
      //deallocated.
      if(arg_buf == (*it))
      {
        buf_.erase(it);

        //Set init variable
        if(buf_.size() <= 0)
        { initialized_ = false; }

        return arg_buf;
      }
    }
    return NULL;
  }

  template <typename T>
  bool CMemCopier<T>::copy()
  {
    if(!initialized_)
    { return false; }
    else
    {
      bool flag = true;
      typename std::vector<T*>::iterator it, ite;
      ite = buf_.end();
      for(it= buf_.begin(); it!=ite; ++it)
      {
        //In case the pointed to memory has been
        //deallocated.
        if(NULL == (*it))
        { flag = false; continue; }
        else
        { memcpy((void*)(*it),mem_,sizeof(T));  }
      }
      return flag;
    }
  }

}

#endif /* CMEMCOPIER_HPP_ */

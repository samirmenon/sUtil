/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CSingleton.hpp
 *
 *  Created on: Jul 8, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CSINGLETON_HPP_
#define CSINGLETON_HPP_

#ifdef DEBUG
//For printing debug messages
#include <iostream>
#endif

namespace sutil
{

  /** Use this to create a singleton with any given data type
   *
   * Works quite well to implement the 'Singleton', a shared
   * data structure that allows different threads in a program
   * to communicate efficiently.
   */
  template<typename SDataStruct>
  class CSingleton
  {
  public:
    /** Creates a singleton if necessary and returns it.
     *
     * Returns a pointer instead of a reference to support
     * a 0 return when there is an error
     */
    static SDataStruct* getData();

    /** Deletes the singleton instance and creates a new one. */
    static bool resetData();

    /** Default destructor : Deletes the singleton instance if it exists */
    virtual ~CSingleton();

  protected:
    /** Shared Memory: The publicly available data that will be
     * shared across the different subsystems */
    SDataStruct data_;

    /** The singleton instance */
    static CSingleton* singleton_;

  private:
    /** Private constructor : for the singleton */
    CSingleton(){}

    /** Private constructor : for the singleton */
    CSingleton(const CSingleton&){}

    /** Private operator : for the singleton */
    CSingleton& operator= (const CSingleton&){}
  };

  /** This initializes the pointer to null */
  template<typename SDataStruct>
  CSingleton<SDataStruct>* CSingleton<SDataStruct>::singleton_ = 0;

  template<typename SDataStruct>
  SDataStruct* CSingleton<SDataStruct>::getData()
  {
    if (0 == singleton_)  // is it the first call?
    {
      singleton_ = new CSingleton<SDataStruct>(); // create sole instance
      if(0 == singleton_)
      {
#ifdef DEBUG
        std::cerr<<"\nCSingleton::createDb() Error: Could not dynamically allocate the database";
#endif
        return 0;
      }
#ifdef DEBUG
      std::cout<<"\nCSingleton::getData() : Creating singleton";
#endif
    }
    return &(singleton_->data_);
  }

  template<typename SDataStruct>
  bool CSingleton<SDataStruct>::resetData()
  {
    if(0 != singleton_)
    {
      CSingleton<SDataStruct>* tmp = singleton_;
      singleton_ = 0;
      delete tmp;
      tmp = 0;

      if(0 != getData())
      {
#ifdef DEBUG
        std::cout<<"\nCSingleton::resetData() : Reset the singleton";
#endif
        return true;
      }
      else
      { return false; }
    }
#ifdef DEBUG
        std::cout<<"\nCSingleton::resetData() : Reset not required";
#endif
    return true;
  }

  template<typename SDataStruct>
  CSingleton<SDataStruct>::~CSingleton()
  {
    //NOTE : Because this doesn't delete the object, but deletes
    //the singleton instead, you can't create two objects of this type
    //EVEN within the member functions. Only one object must exist, else
    //you will get segfaults.
    if(0 != singleton_)
    {
      delete singleton_;
      singleton_ = 0;
#ifdef DEBUG
      std::cout<<"\nCSingleton::~CSingleton() : Destroying singleton";
#endif
    }
  }
}

#endif /* CSINGLETON_HPP_ */

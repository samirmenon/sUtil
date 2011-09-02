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
  protected:
    /** Shared Memory: The publicly available data that will be
     * shared across the different subsystems */
    SDataStruct data_;

  public:
    /** Creates a singleton if necessary and returns it.
     *
     * Returns a pointer instead of a reference to support
     * a 0 return when there is an error
     */
    static SDataStruct* getData();

    /** Default destructor : Deletes the singleton instance if it exists */
    virtual ~CSingleton();

  protected:
    /** Protected constructor : for the singleton */
    CSingleton(){}

    /** The singleton instance */
    static CSingleton* singleton_;

  private:
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
  CSingleton<SDataStruct>::~CSingleton()
  {
    if(0 != singleton_)
    {
      delete singleton_;
#ifdef DEBUG
      std::cout<<"\nCSingleton::~CSingleton() : Destroying singleton";
#endif
    }
  }
}

#endif /* CSINGLETON_HPP_ */

/* This file is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Alternatively, you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License and a copy of the GNU General Public License along with
this file. If not, see <http://www.gnu.org/licenses/>.
*/
/* \file CSingleton.hpp
 *
 *  Created on: Jul 8, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CSINGLETON_HPP_
#define CSINGLETON_HPP_

#include <iostream>

namespace sutil
{

  /** Use this to create a singleton with any given data type
   *
   * Works quite well to implement the 'Database', a shared
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
     * a NULL return when there is an error
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
  CSingleton<SDataStruct>* CSingleton<SDataStruct>::singleton_ = NULL;

  template<typename SDataStruct>
  SDataStruct* CSingleton<SDataStruct>::getData()
  {
    if (NULL == singleton_)  // is it the first call?
    {
      singleton_ = new CSingleton<SDataStruct>(); // create sole instance
      if(NULL == singleton_)
      {
        std::cout<<"\nCDatabase::createDb() Error: Could not dynamically allocate the database";
        return NULL;
      }
    }
    return &(singleton_->data_);
  }

  template<typename SDataStruct>
  CSingleton<SDataStruct>::~CSingleton()
  {
    if(NULL != singleton_)
    { delete singleton_;  }
  }
}

#endif /* CSINGLETON_HPP_ */

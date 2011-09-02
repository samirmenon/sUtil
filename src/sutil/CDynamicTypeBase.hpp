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
/* \file CDynamicTypeBase.hpp
 *
 *  Created on: Jan 6, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CDYNAMICTYPEBASE_HPP_
#define CDYNAMICTYPEBASE_HPP_

namespace sutil
{
  /** To support string inputs for dynamic object allocation,
   * subclass CDynamicTypeBase and implement the createObject()
   * function to return an object of the type you want.
   *
   * How To?
   * 1. Store an object of the subclass in a string indexed map
   * 2. Look up the object with the type-name
   * 3. Call the createObject() function to get a void* pointing
   *    to a new object of the type
   *
   * Example usage:
   * {
   *   //...
   *   //Create a type map
   *   std::map<std::string, CDynamicTypeBase*> typemap;
   *
   *   //Add a type (make sure you subclass and implement
   *   //CDynamicTypeBase for your type first)
   *   CYourType bobo;
   *   typemap.insert( std::pair<std::string,
   *     CDynamicTypeBase*>(std::string("CYourType"), bobo) );
   *
   *   //Get an object of the new type
   *   void* new_obj =
   *     typemap[std::string("CYourType")].second->createObject();
   *
   *   //Make sure you got the right object
   *   CYourType* verified_obj = dynamic_cast<CYourType*>(new_obj);
   *
   *   delete verified_obj;//At the end
   *   //...
   * }
   *
   * Use Case : This is mostly useful when your types are
   * subclassed from some generic class and implement its
   * functions differently.
   *
   * Eg. This would work very well for dynamic typing:
   * CPainter { void paintScreen() = 0;}
   * CBluePainter
   * CRedPainter
   * CGreenPainter
   * etc..
   *
   * Here dynamic typing will save you tons of if/else statements.
   */
  class CDynamicTypeBase
  {
  private:
    /** Must name a type while creating an object */
    CDynamicTypeBase();
    /** Must name a type while creating an object */
    CDynamicTypeBase(const CDynamicTypeBase&);
    /** Must name a type while creating an object */
    CDynamicTypeBase& operator= (const CDynamicTypeBase&);

  protected:
    /** The type of the object */
    std::string type_;

  public:
    /** Must name a type while creating an object */
    CDynamicTypeBase(std::string arg_type)
    { type_ = arg_type; }

    /** Default Destructor : Does nothing */
    virtual ~CDynamicTypeBase(){}

    /** Returns an object's type */
    virtual void getType(std::string & ret_str)
    { ret_str = type_; }

    /** Dynamically allocates an object of the type that any
     * subclass implements, and returns it.
     *
     * Each subclass of CDynamicTypeBase enables another type,
     * say CBobo. This function returns a CBobo object.
     *
     * A dynamic type will implement :
     *  void* CDynamicTypeBobo::createObject()
     *  {
     *    CBobo* bobo = new CMyCoolType();
     *    return static_cast<void*>(bobo);
     *  }
     */
    virtual void* createObject()=0;

    /** Dynamically allocates an object of this API's implementation
     * class and returns it.
     *
     * NOTE: This is not necessary but is useful. It allows some other
     * class to create objects and own them (for instance a singleton
     * database would store an object of this subclass, own it, and use
     * it to create new dynamic type objects).
     *
     * void* CDynamicTypeBobo::createDynamicTypeSubclass()
     *  {
     *    CDynamicTypeBobo* bobo = new CDynamicTypeBobo();
     *    return static_cast<void*>(bobo);
     *  }
     */
    virtual CDynamicTypeBase* createDynamicTypeSubclass()=0;
  };
}

#endif /* CDYNAMICTYPEBASE_HPP_ */

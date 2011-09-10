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
/* \file CDynamicTypeFactory.hpp
 *
 *  Created on: Sep 9, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CDYNAMICTYPEFACTORY_HPP_
#define CDYNAMICTYPEFACTORY_HPP_

#include <sutil/CMappedList.hpp>

namespace sutil
{
  // Read the class comments below
  template <typename Idx> class CDynamicTypeBase; //Forward declaration

  /** This class implements a type factory singleton. In plain English,
   * it is a one-of-a-kind class that can give you an object based on
   * its name.
   *
   * NOTE : Enabling this requires creating a DynamicTypeBase (like the
   * one specified above). The factory creates an object of the DynamicTypeBase
   * and uses that object to generate more of its kind.
   * It uses a map to find the right object create. */
  template <
  /** Must require some index type. Typically a std::string */
  typename Idx,
  /** See the CDynamicTypeBase implementation below to see what
   * is required of a TypeBase */
  typename TypeBase=CDynamicTypeBase<Idx> >
  class CDynamicTypeFactory :
    /** Private inheritance from a singleton, with private constructors for
     * this class, make this class a singleton and hide the singleton's methods
     * from its users.
     *
     * Since this class stores TypeBase* pointers, we use a pointer deleting
     * mapped list to delete the TypeBase* and TypeBase** objects. (See its
     * documentation and working examples).
     *
     * Why so? Because we can only have static functions in this class, and
     * the actual constructor and destructor are called for the singleton.
     * So we can't call a destructor here to deallocate all the TypeBase**
     * objects.*/
    private CSingleton<CPointerDeletingMappedList<Idx,TypeBase*> >
  {
    //A typedef for easy use;
    typedef CSingleton<CPointerDeletingMappedList<Idx,TypeBase*> > singleton;

  public:
    /** This function registers new dynamic types with the factory.
     * You can get objects of this type by calling the getObjectForType function */
    static bool registerType(const Idx& arg_type_name,
        const TypeBase& arg_type_object)
    {
      //We will create a new object of this type and use it to create other
      //objects of this type.

      TypeBase* tmp;
      tmp = dynamic_cast<TypeBase*>(arg_type_object.createDynamicTypeSubclass());
      if(tmp == NULL)
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicTypeFactory::registerType() Error :"
            <<" The passed object's createObject() function did not work.";
#endif
        return false;
      }

      if(NULL != singleton::getData()->at(arg_type_name))
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicTypeFactory::registerType() Error :"
            <<" The passed type is already registered.";
#endif
        return false;
      }

      singleton::getData()->create(arg_type_name,tmp);
      return true;
    }

    /** This function creates new objects of an indexed type (if the
     * type has already been registered with the singleton) */
    static bool getObjectForType(const Idx& arg_type_name,
        void*& ret_object)
    {
      TypeBase* map_pos = singleton::getData()->at(arg_type);

      if(NULL == map_pos)
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicTypeFactory::getObjectForType() Error :"
            <<" The passed type has not been registered.";
#endif
        return false;
      }

      if(NULL != ret_object)
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicTypeFactory::getObjectForType() Error :"
            <<" The passed void* pointer is not NULL.";
#endif
        return false;
      }


      if(NULL == *map_pos)
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicTypeFactory::getObjectForType() Error :"
            <<" The typemap is corrupted. A type creator object pointer is NULL.";
#endif
        return false;
      }

      //Be really careful.
      ret_object = map_pos->createObject();
      if(NULL == ret_object)
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicTypeFactory::getObjectForType() Error :"
            <<" The type object's createObject() function did not work.";
#endif
      }

      return true;
    }

  private:
      /** Private for the singleton */
      CDynamicTypeFactory();

      /** Private for the singleton */
      CDynamicTypeFactory(const CDynamicTypeFactory&);

      /** Private for the singleton */
      CDynamicTypeFactory& operator= (const CDynamicTypeFactory&);
  };

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
  template <typename Idx>
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
    Idx type_;

  public:
    /** Must name a type while creating an object */
    CDynamicTypeBase(Idx arg_type)
    { type_ = arg_type; }

    /** Default Destructor : Does nothing */
    virtual ~CDynamicTypeBase(){}

    /** Returns an object's type */
    virtual void getType(Idx & ret_str)
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

#endif /* CDYNAMICTYPEFACTORY_HPP_ */

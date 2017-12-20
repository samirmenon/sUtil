/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CRegisteredDynamicTypes.hpp
 *
 *  Created on: Sep 9, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef CREGISTEREDDYNAMICTYPES_HPP_
#define CREGISTEREDDYNAMICTYPES_HPP_

#include <sutil/CMappedList.hpp>
#include <sutil/CSingleton.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  // Read the class comments below
  template <typename Idx> class CDynamicTypeBase; //Forward declaration

  /** This class implements a type factory singleton. In plain English,
   * it is a one-of-a-kind class that can give you an object based on
   * its name.
   *
   * Enabling this requires creating a DynamicTypeBase (like the
   * one specified above). The factory creates an object of the DynamicTypeBase
   * and uses that object to generate more of its kind.
   * It uses a map to find the right object create.
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
   *  --- Dynamic typing saves you tons of if/else statements here.
   *  --- It also helps you create generic code that you can package in a library
   *
   * Example usage:
   *   sutil::CDynamicType<IndexType,Type> type(IndexType("Type"));
   *   type.registerType();
   *   OR:
   *   sutil::CDynamicType<std::string,int> type(std::string("int"));
   *   type.registerType();
   *
   *   void *obj=NULL;
   *   sutil::CRegisteredDynamicTypes<std::string>::getObjectForType(std::string("int"), obj);
   *   int *x = reinterpret_cast<int*>(obj);
   *
   *  NOTE : This follows a different convention from the callbacks, objects and printables
   *         because this acts as a dynamic type factory, while those act as lookup tables.
   * */
  template <typename Idx>
  class CRegisteredDynamicTypes :
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
    private CSingleton<CMappedPointerList<Idx,CDynamicTypeBase<Idx>,true> >
  {
    //A typedef for easy use;
    typedef CSingleton<CMappedPointerList<Idx,CDynamicTypeBase<Idx>,true> > singleton;

    friend class CDynamicTypeBase<Idx>;

  public:
    /** Checks whether this type has been registered with the factory */
    static bool typeRegistered(const Idx& arg_type_name)
    {
      CDynamicTypeBase<Idx>** mapped_type = singleton::getData()->at(arg_type_name);

      if(NULL == mapped_type)
      { return false; }

      return true;
    }

    /** This function creates new objects of an indexed type (if the
     * type has already been registered with the singleton) */
    static bool getObjectForType(const Idx& arg_type_name,
        void*& ret_object)
    {
      CDynamicTypeBase<Idx>** mapped_type = singleton::getData()->at(arg_type_name);

      if(NULL == mapped_type)
      {
#ifdef DEBUG
        std::cerr<<"\nCRegisteredDynamicTypes::getObjectForType() Error :"
            <<" The passed type has not been registered.";
#endif
        return false;
      }

      if(NULL != ret_object)
      {
#ifdef DEBUG
        std::cerr<<"\nCRegisteredDynamicTypes::getObjectForType() Error :"
            <<" The passed void* pointer is not NULL.";
#endif
        return false;
      }


      if(NULL == *mapped_type)
      {
#ifdef DEBUG
        std::cerr<<"\nCRegisteredDynamicTypes::getObjectForType() Error :"
            <<" The typemap is corrupted. A type creator object pointer is NULL.";
#endif
        return false;
      }

      //Be really careful.
      ret_object = (*mapped_type)->createObject();
      if(NULL == ret_object)
      {
#ifdef DEBUG
        std::cerr<<"\nCRegisteredDynamicTypes::getObjectForType() Error :"
            <<" The type object's createObject() function did not work.";
#endif
      }

      return true;
    }

    /** Deletes the singleton object and creates a new one
     * in its stead */
    static bool resetDynamicTypes()
    { return singleton::resetData(); }

  private:
    /** This function registers new dynamic types with the factory.
     * You can get objects of this type by calling the getObjectForType function */
    static bool registerType(const Idx& arg_type_name,
        CDynamicTypeBase<Idx>* arg_type_object)
    {
      if(typeRegistered(arg_type_name))
      {
#ifdef DEBUG
        std::cerr<<"\nCRegisteredDynamicTypes::registerType() Warning :"
            <<" The passed type is already registered.";
#endif
        return false;
      }

      CDynamicTypeBase<Idx>** t = singleton::getData()->create(arg_type_name,arg_type_object);
      if(NULL == t)
      {
#ifdef DEBUG
        std::cerr<<"\nCRegisteredDynamicTypes::registerType() Error :"
            <<" Failed to create type in the type mapped list.";
#endif
        return false;
      }

      return true;
    }

    /** Private for the singleton */
    CRegisteredDynamicTypes();

    /** Private for the singleton */
    CRegisteredDynamicTypes(const CRegisteredDynamicTypes&);

    /** Private for the singleton */
    CRegisteredDynamicTypes& operator= (const CRegisteredDynamicTypes&);
  };

  /** To support indexed querying for dynamic object allocation:
   * Subclass CDynamicTypeBase and implement the createObject()
   * function to return an object of the wanted type. */
  template <typename Idx>
  class CDynamicTypeBase
  {
  public:
    /** Must name a type while creating an object */
    CDynamicTypeBase(const Idx &arg_type_name) : type_name_(arg_type_name){}

    /** Default Destructor : Does nothing */
    virtual ~CDynamicTypeBase(){}

    /** Dynamically allocates an object of the type that any
     * subclass implements, and returns it. */
    virtual void* createObject()=0;

  protected:
    virtual bool registerMyType(CDynamicTypeBase* arg_obj)
    {
      return CRegisteredDynamicTypes<Idx>::registerType(type_name_,arg_obj);
    }

    /** The type of the object */
    Idx type_name_;

  private:
    /** Must name a type while creating an object */
    CDynamicTypeBase();
    /** Must name a type while creating an object */
    CDynamicTypeBase(const CDynamicTypeBase&);
    /** Must name a type while creating an object */
    CDynamicTypeBase& operator= (const CDynamicTypeBase&);
  };

  /** This enables supporting dynamic typing for arbitrary
   * index and object types. */
  template <typename Idx, typename Type>
  class CDynamicType : public CDynamicTypeBase<Idx>
  {
  public:
    /** Must name a type while creating an object */
    CDynamicType(const Idx &arg_type_name) :
      CDynamicTypeBase<Idx>::CDynamicTypeBase(arg_type_name)
    { }

    /** Default Destructor : Does nothing */
    virtual ~CDynamicType(){}

    /** Dynamically allocates an object of the type that any
     * subclass implements, and returns it. */
    virtual void* createObject()
    {
      Type* obj = new Type();
      return reinterpret_cast<void*>(obj);
    }

    bool registerType()
    {
      bool flag;
      flag = CRegisteredDynamicTypes<Idx>::typeRegistered(CDynamicTypeBase<Idx>::type_name_);

      if(flag)//Type already registered. Do nothing and return false.
      {
#ifdef DEBUG
        std::cerr<<"\nCDynamicType::registerType() Warning :"
            <<" This object's type has already been registered.";
#endif
        return false;
      }
      else
      {
        CDynamicType<Idx,Type>* obj = new CDynamicType<Idx,Type>(
            CDynamicTypeBase<Idx>::type_name_);
        flag = CDynamicTypeBase<Idx>::registerMyType(obj);
        if(!flag)
        {
#ifdef DEBUG
          std::cerr<<"\nCDynamicType::registerType() Error :"
              <<" This object's type could not be registered.";
#endif
          return false;
        }
      }
      return true;
    }

  private:
      /** Must name a type while creating an object */
      CDynamicType();
      /** Must name a type while creating an object */
      CDynamicType(const CDynamicType&);
      /** Must name a type while creating an object */
      CDynamicType& operator= (const CDynamicType&);
  };

}

#endif /* CREGISTEREDDYNAMICTYPES_HPP_ */

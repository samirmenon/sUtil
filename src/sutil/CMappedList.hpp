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
/* \file CMappedList.hpp
 *
 *  Created on: Jul 12, 2010
 *
 *  Copyright (C) 2010, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMAPPEDLIST_HPP_
#define CMAPPEDLIST_HPP_

#include <map>

#ifdef DEBUG
#include <iostream>
#include <cassert>
#endif

namespace sutil
{

  /** A node in the linked list */
  template <typename IdxS, typename TS>
  class SMLNode
  {
  public:
    TS* data_;
    IdxS* id_;

    //For the linked list
    SMLNode<IdxS,TS> *next_;

    SMLNode()
    {
      data_=NULL;
      id_=NULL;
      next_=NULL;
    }
  };

  /** A linked list to allocate memory for objects and
   * store them, allowing pointer access.
   *
   * Also grants std::map-based access to members, indexed
   * on some type.
   *
   * Main use : Manage a single data store for memory
   * that many must access.
   */
  template <typename Idx, typename T>
  class CMappedList
  {
  public:
    /** ***************************
     * STL container specific code:
     * (a) A set of typedefs
     * (b) An iterator definition
     * (c) Standard methods
     * *************************** */

    /** ***************************
     * The typedefs:
     * ************************** */
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    /** ***************************
     * The iterator definition
     * ************************** */
  public:
    template <typename Node, typename Data> class __iterator; //Forward declaration
  public:
    typedef __iterator<SMLNode<Idx,T>,T> iterator;
    typedef __iterator<const SMLNode<Idx,T>,const T> const_iterator;

    /** ***************************
     * The iterator functions
     * ************************** */
    iterator begin()
    {
      iterator a(front_);
      return a;
    }

    iterator end()
    { return iterator(); }

    /** ***************************
     * The standard methods
     * ************************** */
    /** Constructor : Resets the pilemap. */
    CMappedList() : front_(NULL), back_(NULL),size_(0) {}

//    /** Copy Constructor : Performs a deep-copy (std container requirement).
//     * Beware; This can be quite slow.
//     * 'explicit' makes sure that only a CMappedList can be copied. Ie. Implicit
//     * copy-constructor use is disallowed.*/
//    explicit CMappedList(const CMappedList<Idx,T>& arg_pm);
//
//    /** Assignment operator : Performs a deep-copy (std container requirement).
//     * Beware; This can be quite slow. */
//    CMappedList<Idx,T>& operator = (const CMappedList<Idx,T>& rhs);
//
    /** Destructor : Deallocates all the nodes if someone already hasn't
     * done so. */
    virtual ~CMappedList();
//
//    /** Comparison operator : Performs an element-by-element check (std container requirement).
//     * Beware; This can be quite slow. */
//    bool operator == (const CMappedList<Idx,T>& lhs, const CMappedList<Idx,T>& rhs);
//
//    /** Comparison operator : Performs an element-by-element check (std container requirement).
//     * Beware; This can be quite slow. */
//    bool operator != (const CMappedList<Idx,T>& lhs, const CMappedList<Idx,T>& rhs);
//
//    /** Swaps the elements with the passed pilemap */
//    void swap(CMappedList<Idx,T>& arg_swap_obj);
//
//    /** The current size of the container */
//    size_type size() const;
//
//    /** The maximum size of the container */
//    size_type max_size() const;
//
//    /** Is the container empty */
//    bool empty() const;

  public:
    /** Const pointer access to the list.
     *
     * 1. Can be moved across the list manually to iterate over all the nodes
     * 2. Can be reset to the head of the list.
     */
    const SMLNode<Idx,T> *iterator_;

    /** Reset iterator to head */
    virtual void resetIterator()
    { iterator_ = static_cast<const SMLNode<Idx,T> *>(front_); }

    /** *******************************
     * The mapped list specific methods
     * ****************************** */

    /** Does a deep copy of the mappedlist to
     * get a new one. This is VERY SLOW. */
    virtual bool deepCopy(const CMappedList<Idx,T>* const arg_pmap);

    /** Creates an element, inserts an element into the list
     * and returns the pointer   */
    virtual T* create(const Idx & arg_idx);

    /** Copies the given element, inserts the copied element into the list
     * and returns the pointer to the copied element   */
    virtual T* create(const Idx & arg_idx, const T& arg_t);

    /** Returns the element at the given numerical index
     * in the linked list (usually useful only for
     * debugging)
     *
     * NOTE : The index starts at 0   */
    virtual T* at(const std::size_t arg_idx);

    /** Returns the element referenced by the index
     *
     * NOTE : This uses the std::map (and is rather slow) */
    virtual T* at(const Idx & arg_idx);

    /** Returns the element at the given numerical index
     * in the linked list (usually useful only for
     * debugging)
     *
     * NOTE : The index starts at 0   */
    virtual const T* at_const(const std::size_t arg_idx);

    /** Returns a const pointer to the element referenced by the index
     *
     * NOTE : This uses the std::map (and is rather slow) */
    virtual const T* at_const(const Idx & arg_idx);

    /** Erases an element from the list.
     * Referenced by the element's memory location   */
    virtual bool erase(T* arg_t);

    /** Erases an element from the list.
     * Referenced by its std::map index
     *
     * NOTE : This uses the std::map (and is rather slow) */
    virtual bool erase(const Idx& arg_idx);

    /** Returns the size of the mapped list */
    virtual inline std::size_t size(){ return size_; }

    /** Clears all elements from the list */
    virtual bool clear();

    /** Typical operator access */
    virtual T* operator[](const std::size_t arg_idx)
    { return at(arg_idx); }

    /** Copy-Constructor : Does a deep copy of the mappedlist to
     * get a new one. This is VERY SLOW. */
    virtual CMappedList<Idx,T>& operator = (const CMappedList<Idx,T>& arg_rhs)
    {
      deepCopy(&arg_rhs);
      return *this;
    }

  protected:

    /** Pointer to the head/front/insertion-end of the list */
    SMLNode<Idx,T> *front_;

    /** Pointer to the tail/back/dangling-end of the list */
    SMLNode<Idx,T> *back_;

    /** The map that will enable Idx based data lookup */
    std::map<Idx, SMLNode<Idx,T>*> map_;

    /** The size of the MappedList */
    std::size_t size_;

  public:
    /** An stl style iterator for CMappedList */
    template <typename Node, typename Data>
    class __iterator : public std::iterator<std::forward_iterator_tag, Data>
    {
      Node *pos_;
    public:
      explicit __iterator(): pos_(NULL){}

      /** Explicit so that other iterators don't typecast into this one*/
      explicit __iterator(const iterator& other)
      { pos_ = other.pos_; }

      explicit __iterator(Node* front_node)
      { pos_ = front_node; }

      iterator& operator = (const iterator& other)
      { pos_ = other.pos_; return (*this);  }

      bool operator == (const iterator& other)
      { return (pos_ == other.pos_);  }

      bool operator != (const iterator& other)
      { return (pos_ != other.pos_);  }

      Data& operator * (const iterator& me)
      { return *(me.pos_->data_);  }

      /** Postfix x++ */
      iterator& operator ++(const iterator& me)
      {
        if(NULL!= me.pos_)
        { me.pos_ = me.pos_->next_; }
        return me;
      }

      /** Prefix ++x */
      iterator& operator ++()
      {
        if(NULL!= pos_)
        { pos_ = pos_->next_; }
        return *this;
      }
    };
  };


  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::deepCopy(const CMappedList<Idx,T>* const arg_pmap)
  {//Deep copy.
    this->~CMappedList(); //Delete everything in the mappedlist

    /**Set the current mappedlist to the new mappedlist**/
    if(0 == arg_pmap->size_)
    { front_ = NULL; back_ = NULL; map_.clear(); size_ = 0; }
    else
    {
      SMLNode<Idx,T> *iterator = arg_pmap->front_;
      while(iterator!=NULL)
      {
        T* tmp = create(*(iterator->id_),
            *(iterator->data_));
        if(NULL == tmp)
        {
#ifdef DEBUG
          std::cerr<<"\nCMappedList<Idx,T>::CMappedList(const CMappedList<Idx,T>& arg_pmap) : ";
          std::cerr<<"ERROR : Deep copy failed to duplicate a node. Resetting mappedlist.";
#endif
          this->~CMappedList();//Reset the mappedlist.
          return false;
        }
        iterator = iterator->next_;
      }
    }
    return true;
  }

  template <typename Idx, typename T>
  CMappedList<Idx,T>::~CMappedList()
  {
    SMLNode<Idx,T> *t, *t2;
    t = front_;
    if(NULL!=t)
    { t2 = front_->next_;  }
    while(NULL!=t)
    {
      if(NULL!=t->data_)
      { delete t->data_;  }
      if(NULL!=t->id_)
      { delete t->id_;  }
      delete t;

      t = t2;
      if(NULL!=t)
      { t2 = t->next_;  }
    }

    front_ = NULL;
    back_ = NULL;
    map_.clear();
    size_ = 0;
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::create(const Idx & arg_idx)
  {
    SMLNode<Idx,T> * tmp = new SMLNode<Idx,T>();

    if(NULL==tmp) //Memory not allocated
    { return NULL; }

    //Make sure the idx hasn't already been registered.
    if(map_.find(arg_idx) != map_.end())
    {
#ifdef DEBUG
      std::cerr<<"\nCMappedList<Idx,T>::create() ERROR : Idx exists. Tried to add duplicate entry";
#endif
      return NULL;
    }

    tmp->data_ = new T();
    tmp->id_ = new Idx(arg_idx);
    tmp->next_ = front_;
    front_ = tmp;
    tmp = NULL;
    size_++;

    if(1 == size_)
    { back_ = front_; }

    map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, front_) );

    return front_->data_;
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::create(const Idx & arg_idx, const T& arg_t)
  {
    SMLNode<Idx,T> * tmp = new SMLNode<Idx,T>();

    if(NULL==tmp) //Memory not allocated
    { return NULL; }

    //Make sure the idx hasn't already been registered.
    if(map_.find(arg_idx) != map_.end())
    {
#ifdef DEBUG
      std::cerr<<"\nCMappedList<Idx,T>::create() ERROR : Idx exists. Tried to add duplicate entry";
#endif
      return NULL;
    }

    tmp->data_ = new T(arg_t);
    tmp->id_ = new Idx(arg_idx);
    tmp->next_ = front_;
    front_ = tmp;
    tmp = NULL;
    size_++;

    if(1 == size_)
    { back_ = front_; }

    map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, front_) );

    return front_->data_;
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::at(const std::size_t arg_idx)
  {
    if(NULL==front_)
    { return NULL;  }
    else
    {
      if(arg_idx > size_)
      { return NULL; }
      SMLNode<Idx,T> * t = front_;

      for(std::size_t i=0; i<arg_idx; ++i)
      {
#ifdef DEBUG
        assert(i<=size_);
#endif

        if(NULL==t)
        { return NULL;  }
        t = t->next_;
      }
      if(NULL==t)
      { return NULL;  }

      return t->data_;
    }
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::at(const Idx & arg_idx)
  {
    if(NULL==front_)
    { return NULL;  }
    else
    {
      try
      {
        if(map_.find(arg_idx) == map_.end())
        {
          return NULL;
        }

        SMLNode<Idx,T> * t = map_[arg_idx];

        if(NULL==t)
        { return NULL;  }
        else
        { return t->data_;  }
      }
      catch(std::exception &e)
      {
        //TODO : print something
        return NULL;
      }
    }
  }



  template <typename Idx, typename T>
  const T* CMappedList<Idx,T>::at_const(const std::size_t arg_idx)
  { return (const T*) at(arg_idx);  }

  template <typename Idx, typename T>
  const T* CMappedList<Idx,T>::at_const(const Idx & arg_idx)
  { return (const T*) at(arg_idx);  }


  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::erase(T* arg_t)
  {
    if((NULL==front_) || (NULL==arg_t))
    { return false;  }

    SMLNode<Idx,T> * t, *tpre;

    //Head is a special case
    if(front_->data_ == arg_t)
    {
      t = front_;
      front_ = front_->next_;

      if(NULL!= t->data_)
      {
        delete t->data_;
        if(NULL!= t->id_)
        {
          map_.erase(*(t->id_));
          delete t->id_;
        }
        delete t;
        size_--;

        if(0 == size_)
        { back_ = NULL; }

        return true; // Deleted head.
      }

      return false;//Head was NULL --> Error condition.
    }
    else
    {
      //The head doesn't match.
      tpre = front_;
      t = front_->next_;

      //Find the node
      while(NULL!=t)
      {
        if(t->data_ == arg_t)
        {
          tpre->next_ = t->next_;
          if(NULL!= t->data_)
          {
            delete t->data_;
            if(NULL!= t->id_)
            {
              map_.erase(*(t->id_));
              delete t->id_;
            }
            delete t;
            size_--;

            if(0 == size_)
            { back_ = NULL; }

            return true; // Deleted node.
          }
          else
          { return false; }//Node to delete was NULL --> Error condition.
        }
        tpre = t;
        t = t->next_;
      }
      return false; // Didn't delete anything.
    }//End of if/else
  }



  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::erase(const Idx& arg_idx)
  {
    if(NULL==front_)
    { return false;  }

    //Make sure the node exists
    if(map_.find(arg_idx) == map_.end())
    {
#ifdef DEBUG
      std::cout<<"\nCMappedList<Idx,T>::erase() WARNING : Tried to erase a nonexistent entry";
#endif
      return false;
    }

    SMLNode<Idx,T> * t, *tpre;
    SMLNode<Idx,T> * node = map_[arg_idx];

    //Head is a special case
    if(front_->data_ == node->data_)
    {
      t = front_;
      front_ = front_->next_;

      if(NULL!= t->data_)
      {
        delete t->data_;
        if(NULL!= t->id_)
        { delete t->id_;  }
        size_--;
        map_.erase(arg_idx);

        if(0 == size_)
        { back_ = NULL; }

        delete t;
        return true; // Deleted head.
      }

      return false;//Head was NULL --> Error condition.
    }
    else
    {
      //The head doesn't match.
      tpre = front_;
      t = front_->next_;

      //Find the node
      while(NULL!=t)
      {
        if(t->data_ == node->data_)
        {
          tpre->next_ = t->next_;
          if(NULL!= t->data_)
          {
            delete t->data_;
            if(NULL!= t->id_)
            { delete t->id_;  }
            size_--;
            map_.erase(arg_idx);

            if(0 == size_)
            { back_ = NULL; }

            delete t;
            return true; // Deleted node.
          }
          else
          { return false; }//Node to delete was NULL --> Error condition.
        }
        tpre = t;
        t = t->next_;
      }
      return false; // Didn't delete anything.
    }//End of if/else
  }

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::clear()
  {
    SMLNode<Idx,T> *tpre;
    tpre = front_;

    if(tpre == NULL)
    {
      size_=0;
      return true;
    } //Nothing in the list.

    front_ = front_->next_;

    while(NULL!=tpre)
    {
      if(NULL!=tpre->data_)
      { delete tpre->data_; }
      if(NULL!=tpre->id_)
      { delete tpre->id_; }
      delete tpre;

      tpre = front_;
      if(NULL==tpre)//Reached the end.
      { break; }
      front_ = front_->next_;
    }

    size_=0;
    back_ = NULL;
    map_.clear(); // Clear the map.
    return true;
  }

}

#endif /* CMAPPEDLIST_HPP_ */


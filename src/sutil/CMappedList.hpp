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
#include <cstddef>

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
   *
   * When to use:
   * (a) You want to store pointers to the contained objects
   *     and guarantee that the pointed-to memory doesn't change
   *     (Eg. Adding an element to a vector or a map might
   *     invalidate the contained objects's addresses)
   * (b) You usage is. Access a contained object once, and then
   *     perform lots of operations on it.
   * (c) You don't want to manage either map access or memory for
   *     contained objects
   * (d) You want the leanest possible code that does the above
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
     * The standard stl typedefs:
     * ************************** */
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef T        value_type;

    /** ***************************
     * The iterator definition
     * ************************** */
    class iterator; //Forward declaration
    class const_iterator;

    /** ***************************
     * The standard methods
     * ************************** */
    /** Constructor : Resets the pilemap. */
    CMappedList() : front_(NULL), back_(NULL),size_(0) {}

  protected:
    /** Does a deep copy of the mappedlist to
     * get a new one. This is VERY SLOW. */
    virtual bool deepCopy(const CMappedList<Idx,T>* const arg_pmap);

  public:
    /** Copy Constructor : Performs a deep-copy (std container requirement).
     * Beware; This can be quite slow.
     * 'explicit' makes sure that only a CMappedList can be copied. Ie. Implicit
     * copy-constructor use is disallowed.*/
    explicit CMappedList(const CMappedList<Idx,T>& arg_pm)
    { deepCopy(&arg_pm);  }

    /** Assignment operator : Performs a deep-copy (std container requirement).
     * Beware; This can be quite slow. */
    virtual CMappedList<Idx,T>& operator = (const CMappedList<Idx,T>& arg_rhs)
    {
      deepCopy(&arg_rhs);
      return *this;
    }

    /** Destructor : Deallocates all the nodes if someone already hasn't
     * done so. */
    virtual ~CMappedList();

    /** Comparison operator : Performs an element-by-element check (std container requirement).
     * Beware; This can be quite slow. */
    bool operator == (const CMappedList<Idx,T>& rhs);

    /** Comparison operator : Performs an element-by-element check (std container requirement).
     * Beware; This can be quite slow. */
    bool operator != (const CMappedList<Idx,T>& rhs);

//    /** Swaps the elements with the passed pilemap */
//    void swap(CMappedList<Idx,T>& arg_swap_obj);

//    /** Example usage:
//     *   first.assign (7,100);                      // 7 ints with value 100
//     *   second.assign (first.begin(),first.end()); // a copy of first */
//    void assign ( iterator first, iterator last );
//    void assign ( size_type n, const T& u );

    /** *******************************
     * The mapped list specific methods
     * ****************************** */
  public:

    /** Creates an element, inserts an element into the list
     * and returns the pointer   */
    virtual T* create(const Idx & arg_idx, const bool insert_at_start=true);

    /** Copies the given element, inserts the copied element into the list
     * and returns the pointer to the copied element   */
    virtual T* create(const Idx & arg_idx, const T& arg_t, const bool insert_at_start=true);

    /** Returns the element at the given numerical index
     * in the linked list (usually useful only for
     * debugging)
     *
     * NOTE : The index starts at 0   */
    virtual T* at(const std::size_t arg_idx);

    /** Returns the element referenced by the index
     *
     * NOTE : This uses the std::map (and is somewhat slow) */
    virtual T* at(const Idx & arg_idx);

    /** Returns the typed index at the given numerical index
     * in the linked list
     *
     * NOTE : The index starts at 0   */
    virtual const Idx* getIndexAt(const std::size_t arg_idx);

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
    virtual std::size_t size() const
    { return size_; }

    /** Is the container empty */
    virtual bool empty() const
    { return (size_ == 0);  }

    /** Clears all elements from the list */
    virtual bool clear();

    /** Typical operator access */
    virtual T* operator[](const std::size_t arg_idx)
    { return at(arg_idx); }

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
    class iterator : public std::iterator<std::forward_iterator_tag, T>
    {
      //To allow : const_iterator x = iterator();
      friend class const_iterator;

      SMLNode<Idx,T> *pos_;
    public:
      explicit iterator(): pos_(NULL){}

      /** Explicit so that other iterators don't typecast into this one*/
      iterator(const iterator& other)
      { pos_ = other.pos_; }

      explicit iterator(SMLNode<Idx,T>* node_ptr)
      { pos_ = node_ptr; }

      iterator&
      operator = (const iterator& other)
      { pos_ = other.pos_; return (*this);  }

      bool
      operator == (const iterator& other)
      { return (pos_ == other.pos_);  }

      bool
      operator != (const iterator& other)
      { return (pos_ != other.pos_);  }

      T&
      operator * ()
      { return *(pos_->data_);  }

      T*
      operator -> ()
      { return pos_->data_;  }

      Idx&
      operator ! ()
      { return *(pos_->id_);  }

      /** Postfix x++. Note that its argument must be an int */
      iterator&
      operator ++(int unused)
      {
        if(NULL!= pos_)
        { pos_ = pos_->next_; }
        return *this;
      }

      /** Prefix ++x */
      iterator&
      operator ++()
      {
        if(NULL!= pos_)
        { pos_ = pos_->next_; }
        return *this;
      }

      iterator
      operator +(int offset)
      {
        SMLNode<Idx,T> *ptr = this->pos_;
        for(int i=0; i <offset; ++i)
        {
          if(NULL== ptr) { break; }
          ptr = ptr->next_;
        }

        return iterator(ptr);
      }
    };

    /** An stl style const_iterator for CMappedList */
    class const_iterator : public std::iterator<std::forward_iterator_tag, T>
    {
      const SMLNode<Idx,T> *pos_;
    public:
      explicit const_iterator(): pos_(NULL){}

      /** Explicit so that other const_iterators don't typecast into this one*/
      const_iterator(const const_iterator& other)
      { pos_ = other.pos_; }

      explicit const_iterator(const SMLNode<Idx,T>* node_ptr)
      { pos_ = node_ptr; }

      const const_iterator& operator = (const const_iterator& other)
      { pos_ = other.pos_; return (*this);  }

      const const_iterator& operator = (const iterator& other)
      { pos_ = static_cast<const SMLNode<Idx,T> *>(other.pos_); return (*this);  }

      bool
      operator == (const const_iterator& other)
      { return (pos_ == other.pos_);  }

      bool
      operator != (const const_iterator& other)
      { return (pos_ != other.pos_);  }

      const T&
      operator * ()
      { return *(pos_->data_);  }

      const T*
      operator -> ()
      { return pos_->data_;  }

      const Idx&
      operator ! ()
      { return *(pos_->id_);  }

      /** Postfix x++. Note that its argument must be an int */
      const_iterator&
      operator ++(int unused)
      {
        if(NULL!= pos_)
        { pos_ = pos_->next_; }
        return *this;
      }

      /** Prefix ++x */
      const_iterator&
      operator ++()
      {
        if(NULL!= pos_)
        { pos_ = pos_->next_; }
        return *this;
      }

      const_iterator
      operator +(int offset)
      {
        const SMLNode<Idx,T> *ptr = this->pos_;
        for(int i=0; i <offset; ++i)
        {
          if(NULL== ptr) { break; }
          ptr = ptr->next_;
        }

        return const_iterator(ptr);
      }
    };

    /** ***************************
     * The iterator functions
     * ************************** */
    iterator begin()
    { return iterator(front_); }

    const_iterator begin() const
    { return const_iterator(front_); }

    iterator end()
    { return iterator(); }

    const_iterator end() const
    { return const_iterator(); }
  };

  /** This is to delete the second pointers in the destructor. Useful
   * if you want to manage pointers to pointers.
   *
   * Ie. if T is actually something like CSuperClass*, and so
   * the objects the pilemap stores are CSuperClass**.
   *
   * This implementation will delete the objects all through. */
  template <typename Idx, typename T, bool ManageMemory>
  class CMappedPointerList : public CMappedList<Idx,T*>
  {
  public:
    typedef typename CMappedList<Idx,T*>::iterator iterator;
    typedef typename CMappedList<Idx,T*>::const_iterator const_iterator;

    virtual ~CMappedPointerList()
    {
      if(ManageMemory)
      {
        typename CMappedList<Idx,T*>::iterator it,ite;
        for(it = this->begin(), ite = this->end();
            it!=ite; ++it)
        {
          T*& tmp = *it;
          delete tmp;
          tmp = NULL;
        }
      }
    }
  };

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::deepCopy(const CMappedList<Idx,T>* const arg_pmap)
  {//Deep copy.
    //No need if both are identical
    if(this == arg_pmap)
    { return true;  }

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
            *(iterator->data_),false);
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
  bool CMappedList<Idx,T>::operator == (const CMappedList<Idx,T>& rhs)
  {
    CMappedList<Idx,T>::const_iterator it, ite, it2, it2e;
    for(it = begin(), ite = end(),
        it2 = rhs.begin(), it2e = rhs.end();
        it!=ite || it2!=it2e; ++it, ++it2)
    {
      if(*it != *it2)
      { return false; }
    }
    //Would exit loop if atleast one was at the end.
    //If both aren't at the end, return false;
    if( ((it != ite)&&(it2 == it2e)) ||
        ((it == ite)&&(it2 != it2e)) )
    { return false; }
    //If both are at the end then the two mapped lists are equal
    return true;
  }

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::operator != (const CMappedList<Idx,T>& rhs)
  { return !(*this == rhs);}

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::create(const Idx & arg_idx, const bool insert_at_start)
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

    if(insert_at_start)
    {
      tmp->next_ = front_;
      front_ = tmp;
      tmp = NULL;
    }
    else
    {
      back_->next_ = tmp;
      tmp->next_ = NULL;
      back_ = tmp;
      tmp = NULL;
    }
    size_++;

    if(1 == size_)
    { back_ = front_; }

    map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, front_) );

    return front_->data_;
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::create(const Idx & arg_idx, const T& arg_t, const bool insert_at_start)
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

    if(insert_at_start)
    {
      tmp->next_ = front_;
      front_ = tmp;
      tmp = NULL;
    }
    else
    {
      back_->next_ = tmp;
      tmp->next_ = NULL;
      back_ = tmp;
      tmp = NULL;
    }

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
  const Idx* CMappedList<Idx,T>::getIndexAt(const std::size_t arg_idx)
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

      return t->id_;
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


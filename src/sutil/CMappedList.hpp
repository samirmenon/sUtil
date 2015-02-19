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
#include <vector>

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
    SMLNode<IdxS,TS> *next_,*prev_;

    SMLNode()
    {
      data_=NULL;
      id_=NULL;
      next_=NULL;
      prev_=NULL;
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
    CMappedList() : front_(NULL), back_(NULL), size_(0), flag_is_sorted_(false) {}

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
    {
      front_ = NULL; back_ = NULL; null_.prev_ = NULL; size_ = 0;
      deepCopy(&arg_pm);
    }

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

    /** Swaps the elements with the passed pilemap */
    void swap(CMappedList<Idx,T>& arg_swap_obj);

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
     * and returns the pointer to the copied element.
     *
     * By default inserts element at the start/begin() position. If specified,
     * flag is false, inserts at the end of the list. */
    virtual T* create(const Idx & arg_idx, const T& arg_t, const bool insert_at_start=true);

    /** Inserts the given object into the list and returns the pointer to the copied element.
     * NOTE : The recommended method is to use create. Else the object memory deallocation might
     * be ambiguous. Remember that the mapped list "always" clears its own data.
     *
     * By default inserts element at the start/begin() position. If specified,
     * flag is false, inserts at the end of the list. */
    virtual T* insert(const Idx & arg_idx, T* arg_t, const bool insert_at_start=true);

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
    virtual const Idx* getIndexAt(const std::size_t arg_idx) const;

    /** Returns the numeric index at the given typed index
     * in the linked list
     *
     * NOTE : The index starts at 0. Returns -1 if node not found. */
    virtual int getIndexNumericAt(const Idx& arg_idx) const;

    /** Returns the numeric index at the given typed index
     * in the linked list
     *
     * NOTE : The index starts at 0. Returns -1 if node not found.   */
    virtual int getIndexNumericAt(const T* const arg_node) const;

    /** Returns the element at the given numerical index
     * in the linked list (usually useful only for
     * debugging)
     *
     * NOTE : The index starts at 0   */
    virtual const T* at_const(const std::size_t arg_idx) const;

    /** Returns a const pointer to the element referenced by the index
     *
     * NOTE : This uses the std::map (and is rather slow) */
    virtual const T* at_const(const Idx & arg_idx) const;

    /** Erases an element from the list.
     * Referenced by the element's memory location   */
    virtual bool erase(const T* arg_t);

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

    /** *******************************************************
     *                      Mapped List Data
     * ******************************************************* */
  protected:

    /** Pointer to the head/front/insertion-end of the list */
    SMLNode<Idx,T> *front_;

    /** Pointer to the tail/back/dangling-end of the list */
    SMLNode<Idx,T> *back_;

    /** Pointer to the element obtained with the end() call.
     * Does not exist in the map. */
    SMLNode<Idx,T> null_;

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

      Idx& getIdx(){ return *(pos_->id_);  }

      /** Postfix x++. Note that its argument must be an int */
      iterator&
      operator ++(int unused)
      {
        if(NULL!= pos_->next_)
        { pos_ = pos_->next_; }
        return *this;
      }

      /** Prefix ++x */
      iterator&
      operator ++()
      {
        if(NULL!= pos_->next_)
        { pos_ = pos_->next_; }
        return *this;
      }

      iterator
      operator +(int offset)
      {
        SMLNode<Idx,T> *ptr = this->pos_;
        for(int i=0; i <offset; ++i)
        {
          if(NULL== ptr->next_) { break; }
          ptr = ptr->next_;
        }

        return iterator(ptr);
      }

      /** Postfix x--. Note that its argument must be an int */
      iterator&
      operator --(int unused)
      {
        if(NULL!= pos_)
        { pos_ = pos_->prev_; }
        return *this;
      }

      /** Prefix --x */
      iterator&
      operator --()
      {
        if(NULL!= pos_)
        { pos_ = pos_->prev_; }
        return *this;
      }

      iterator
      operator -(int offset)
      {
        SMLNode<Idx,T> *ptr = this->pos_;
        for(int i=0; i < offset; ++i)
        {
          if(NULL== ptr->prev_) { break; }
          ptr = ptr->prev_;
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

      const Idx& getIdx(){ return *(pos_->id_);  }

      /** Postfix x++. Note that its argument must be an int */
      const_iterator&
      operator ++(int unused)
      {
        if(NULL!= pos_->next_)
        { pos_ = pos_->next_; }
        return *this;
      }

      /** Prefix ++x */
      const_iterator&
      operator ++()
      {
        if(NULL!= pos_->next_)
        { pos_ = pos_->next_; }
        return *this;
      }

      const_iterator
      operator +(int offset)
      {
        const SMLNode<Idx,T> *ptr = this->pos_;
        for(int i=0; i <offset; ++i)
        {
          if(NULL== ptr->next_) { break; }
          ptr = ptr->next_;
        }

        return const_iterator(ptr);
      }

      /** Postfix x--. Note that its argument must be an int */
      const_iterator&
      operator --(int unused)
      {
        if(NULL!= pos_)
        { pos_ = pos_->prev_; }
        return *this;
      }

      /** Prefix --x */
      const_iterator&
      operator --()
      {
        if(NULL!= pos_)
        { pos_ = pos_->prev_; }
        return *this;
      }

      const_iterator
      operator -(int offset)
      {
        const SMLNode<Idx,T> *ptr = this->pos_;
        for(int i=0; i <offset; --i)
        {
          if(NULL== ptr) { break; }
          ptr = ptr->prev_;
        }

        return const_iterator(ptr);
      }
    };

    /** ***************************
     * The iterator functions
     * ************************** */
    iterator begin()
    {
      if(NULL!=front_){ return iterator(front_);  }
      else{ return iterator(&null_);  }
    }

    const_iterator begin() const
    {
      if(NULL!=front_){ return const_iterator(front_);  }
      else{ return const_iterator(&null_);  }
    }

    iterator end()
    { return iterator(&null_); }

    const_iterator end() const
    { return const_iterator(&null_); }

    /** *******************************************************
     *                 Sorting related functions
     * ******************************************************* */
  public:
    /** Sorts the list so that iterator access matches the given
     * vector's index. Ie. Given indices 0 to n in the vector,
     * the begin() iterator will match 0, and the end() will
     * appear after iterating over n.
     *
     * Any create/erase/swap etc. function call will invalidate
     * the sort ordering.
     *
     * Sorting type : Insertion Sort.
     */
    virtual bool sort(const std::vector<Idx> &arg_order);

    /** Get the sorting order if there is one */
    virtual bool sort_get_order(std::vector<Idx>& ret_order) const
    {
      if(flag_is_sorted_)
      { ret_order = sorting_order_; return true;  }
      else
      { return false; }
    }

    /** Whether the list has been sorted or not */
    virtual bool isSorted() const
    { return flag_is_sorted_;  }

  protected:
    /** An index that specifies a sort ordering if required */
    std::vector<Idx> sorting_order_;

    /** Whether the list is sorted or not */
    bool flag_is_sorted_;
  }; //End of class.

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
    {
      front_ = NULL; back_ = NULL; null_.prev_ = NULL;
      map_.clear();
      size_ = 0;
    }
    else
    {
      CMappedList<Idx,T>::const_iterator it,ite;
      for(it = arg_pmap->begin(), ite = arg_pmap->end(); it!=ite; ++it)
      {
        T* tmp = CMappedList<Idx,T>::create(!it,*it,false);
        if(NULL == tmp)
        {
#ifdef DEBUG
          std::cerr<<"\nCMappedList<Idx,T>::CMappedList(const CMappedList<Idx,T>& arg_pmap) : ";
          std::cerr<<"ERROR : Deep copy failed to duplicate a node. Resetting mappedlist.";
#endif
          this->~CMappedList();//Reset the mappedlist.
          return false;
        }
      }
    }
    return true;
  }

  template <typename Idx, typename T>
  CMappedList<Idx,T>::~CMappedList()
  {
    SMLNode<Idx,T> *t;

    //Nothing to do if already empty
    if(0==size_) {  return; }

    //Terminate the end (just in case)
    null_.next_ = NULL;

    //Now start deleting everything
    t = front_->next_;
    while(NULL!=t)
    {
      if(NULL!=t->prev_->data_)
      { delete t->prev_->data_;  }
      if(NULL!=t->prev_->id_)
      { delete t->prev_->id_;  }
      delete t->prev_;

      t = t->next_;
    }

    front_ = NULL; back_ = NULL; null_.prev_ = NULL;
    map_.clear();
    size_ = 0;
    flag_is_sorted_ = false;
  }

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::operator == (const CMappedList<Idx,T>& rhs)
  {
    CMappedList<Idx,T>::const_iterator it, ite, it2, it2e;
    for(it = begin(), ite = end(),
        it2 = rhs.begin(), it2e = rhs.end();
        it!=ite && it2!=it2e; ++it, ++it2)
    {
      if(*it != *it2)
      { return false; }
    }
    //Would exit loop if atleast one was at the end.
    //If both are at the end then the two mapped lists are equal
    if((it == ite) && (it2 == it2e))
    { return true;  }
    //If both aren't at the end, return false;
    else
    { return false; }
  }

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::operator != (const CMappedList<Idx,T>& rhs)
  { return !(*this == rhs);}


  template <typename Idx, typename T>
  void CMappedList<Idx,T>::swap(CMappedList<Idx,T>& arg_swap_obj)
  {
    //Be lazy
    CMappedList<Idx,T> *lhs, *rhs;
    if(size_ >= arg_swap_obj.size_)
    { lhs = this; rhs = &arg_swap_obj;  }
    else
    { lhs = &arg_swap_obj; rhs = this;  }

    //List status.
    SMLNode<Idx,T> *tf = lhs->front_;
    SMLNode<Idx,T> *tb = lhs->back_;
    std::map<Idx, SMLNode<Idx,T>*> tmap(lhs->map_);
    size_t ts = lhs->size_;

    //Sorting status
    bool tmp_lhs_sorted = lhs->flag_is_sorted_;
    std::vector<Idx> tmp_lhs_sorting_order;
    if(lhs->flag_is_sorted_)
    { tmp_lhs_sorting_order = lhs->sorting_order_;  }

    //Copy list status
    lhs->front_ = rhs->front_;
    lhs->back_ = rhs->back_;
    lhs->map_ = rhs->map_;
    lhs->size_ = rhs->size_;
    lhs->null_.prev_ = lhs->back_;
    if(0 < lhs->size_) {  lhs->back_->next_ = &(lhs->null_);  }

    //Copy sorting status
    lhs->flag_is_sorted_ = rhs->flag_is_sorted_;
    if(lhs->flag_is_sorted_)
    { lhs->sorting_order_ = rhs->sorting_order_;  }
    else { lhs->sorting_order_.clear();  }

    //Copy list status from tmp
    rhs->front_ = tf;
    rhs->back_ = tb;
    rhs->map_ = tmap;
    rhs->size_ = ts;
    rhs->null_.prev_ = rhs->back_;
    if(0 < rhs->size_) {  rhs->back_->next_ = &(rhs->null_);  }

    //Copy sorting status from tmp
    rhs->flag_is_sorted_ = tmp_lhs_sorted;
    if(rhs->flag_is_sorted_)
    { rhs->sorting_order_ = tmp_lhs_sorting_order;  }
    else { rhs->sorting_order_.clear();  }
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::create(const Idx & arg_idx, const bool insert_at_start)
  { return CMappedList<Idx,T>::create(arg_idx,T(),insert_at_start); }

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

    /** If size is zero, insert at start/end doesn't matter. */
    if(0 == size_)
    {
      back_ = tmp;
      front_ = tmp;
      front_->prev_ = NULL;
      front_->next_ = &null_;
      null_.prev_ = back_;
      tmp = NULL;
    }
    else if(insert_at_start)
    {
      tmp->next_ = front_;
      front_->prev_ = tmp;
      front_ = tmp;
      front_->prev_ = NULL;
      tmp = NULL;
    }
    else
    {
      back_->next_ = tmp;
      tmp->next_ = &null_;
      tmp->prev_ = back_;
      back_ = tmp;
      null_.prev_ = back_;
      back_->next_ = &null_;
      tmp = NULL;
    }

    size_++;
    flag_is_sorted_ = false;

    if((0 == size_) || insert_at_start) {
      map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, front_) );
      return front_->data_;
    } else {
      map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, back_) );
      return back_->data_;
    }
  }

  template <typename Idx, typename T>
  T* CMappedList<Idx,T>::insert(const Idx & arg_idx, T* arg_t, const bool insert_at_start)
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

    tmp->data_ = arg_t;
    tmp->id_ = new Idx(arg_idx);

    /** If size is zero, insert at start/end doesn't matter. */
    if(0 == size_)
    {
      back_ = tmp;
      front_ = tmp;
      front_->prev_ = NULL;
      front_->next_ = &null_;//Set the end "null" terminator node..
      null_.prev_ = back_;
      tmp = NULL;
    }
    else if(insert_at_start)
    {
      tmp->next_ = front_;
      front_->prev_ = tmp;
      front_ = tmp;
      front_->prev_ = NULL;
      tmp = NULL;
    }
    else
    {
      back_->next_ = tmp;
      tmp->next_ = &null_;//Set the end "null" terminator node..
      tmp->prev_ = back_;
      back_ = tmp;
      null_.prev_ = back_;
      back_->next_ = &null_;
      tmp = NULL;
    }

    size_++;
    flag_is_sorted_ = false;

    if((0 == size_) || insert_at_start) {
      map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, front_) );
      return front_->data_;
    } else {
      map_.insert( std::pair<Idx, SMLNode<Idx,T> *>(arg_idx, back_) );
      return back_->data_;
    }
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
  }

  template <typename Idx, typename T>
  const Idx* CMappedList<Idx,T>::getIndexAt(const std::size_t arg_idx) const
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

  /** Returns the numeric index at the given typed index in the linked list */
  template <typename Idx, typename T>
  int CMappedList<Idx,T>::getIndexNumericAt(const Idx& arg_idx) const
  {
    const T *tdes = at_const(arg_idx);
    return getIndexNumericAt(tdes);
  }

  /** Returns the numeric index at the given typed index in the linked list*/
  template <typename Idx, typename T>
  int CMappedList<Idx,T>::getIndexNumericAt(const T* const arg_node) const
  {
    if(NULL == arg_node) { return -1; }

    SMLNode<Idx,T> *t = front_;

    int idx = 0;
    while(t->data_ != arg_node && NULL!=t)
    { t = t->next_; idx++;  }

    if(NULL == t){idx = -1;}

    return idx;
  }


  template <typename Idx, typename T>
  const T* CMappedList<Idx,T>::at_const(const std::size_t arg_idx) const
  {
    if(NULL==front_)
    { return NULL;  }
    else
    {
      if(arg_idx > size_)
      { return NULL; }
      const SMLNode<Idx,T> * t = front_;

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

      return (const T*) t->data_;
    }
  }

  template <typename Idx, typename T>
  const T* CMappedList<Idx,T>::at_const(const Idx & arg_idx) const
  {
    if(NULL==front_)
    { return NULL;  }
    else
    {
      if(map_.find(arg_idx) == map_.end())
      {
        return NULL;
      }

      const SMLNode<Idx,T> * t = map_.at(arg_idx);

      if(NULL==t)
      { return NULL;  }
      else
      { return (const T*) t->data_;  }
    }
  }


  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::erase(const T* arg_t)
  {
    if((NULL==front_) || (NULL==arg_t))
    { return false;  }

    SMLNode<Idx,T> * t, *tpre;

    //Head is a special case
    if(front_->data_ == arg_t)
    {
      t = front_;
      front_ = front_->next_;
      front_->prev_ = NULL;

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
        { back_ = NULL; null_.prev_=NULL; }

        flag_is_sorted_ = false;

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
          tpre->next_->prev_ = tpre;
          if(NULL!= t->data_)
          {
            delete t->data_;
            if(NULL!= t->id_)
            {
              map_.erase(*(t->id_));
              delete t->id_;
            }
            if(back_ == t)//Removing the ending node; have to reassign
            { back_ = tpre; }
            delete t;
            size_--;

            if(0 == size_)
            { back_ = NULL; null_.prev_=NULL; }

            flag_is_sorted_ = false;

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
    if(0>=size_)
    { return false;  }

    //Make sure the node exists
    if(map_.find(arg_idx) == map_.end())
    {
#ifdef DEBUG
      std::cerr<<"\nCMappedList<Idx,T>::erase() WARNING : Tried to erase a nonexistent entry";
#endif
      return false;
    }

    SMLNode<Idx,T> * node = map_[arg_idx];

    if(1==size_)
    {
      if(front_!=node) { return false; } //This should never happen when Size 1 + idx exists.

      if(NULL!= front_->data_)
      { delete front_->data_; }
      if(NULL!= front_->id_)
      { delete front_->id_;  }
      delete front_;

      front_ = NULL; back_ = NULL; null_.prev_ = NULL;
      node = NULL;
    }
    else
    {//At least two nodes. Remove the node
      if(NULL!= node->data_)
      { delete node->data_; }
      if(NULL!= node->id_)
      { delete node->id_;  }

      if(front_ == node)
      {
        front_ = front_->next_;
        front_->prev_ = NULL;
      }
      else if(back_ == node)
      {
        back_ = back_->prev_;
        back_->next_ = &null_;
        null_.prev_ = back_;
      }
      else
      {
        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;
      }

      delete node;
      node = NULL;
    }

    size_--;
    map_.erase(arg_idx);
    flag_is_sorted_ = false;

    return true; // Deleted head.
  }

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::clear()
  {
    SMLNode<Idx,T> *tpre;
    tpre = front_;

    if(tpre == NULL)
    {
      size_=0;
      flag_is_sorted_ = false;
      return true;
    } //Nothing in the list.

    front_ = front_->next_;

    while(&null_ != tpre)
    {
      if(NULL!=tpre->data_)
      { delete tpre->data_; }
      if(NULL!=tpre->id_)
      { delete tpre->id_; }
      delete tpre;

      tpre = front_;
      if(&null_ == tpre)//Reached the end.
      { break; }
      front_ = front_->next_;
    }

    size_=0;
    front_ = NULL; back_ = NULL; null_.prev_ = NULL;
    map_.clear(); // Clear the map.
    flag_is_sorted_ = false; //Not ordered anymore
    return true;
  }

  template <typename Idx, typename T>
  bool CMappedList<Idx,T>::sort(const std::vector<Idx> &arg_order)
  {
    if(1>=size_)
    {//Already sorted.
      flag_is_sorted_ = true;
      sorting_order_ = arg_order;
      return true;
    }//Now we know for sure that there are atleast 2 elements, and that front, and back are set.

    // Check that size matches the passed order
    if(arg_order.size() != size_)
    {
#ifdef DEBUG
      std::cout<<"\nCMappedList<Idx,T>::sort() ERROR : Number of indices in passed order doesn't match mapped list.";
#endif
      return false;
    }

    //First check all the indices actually appear in the list.
    typename std::vector<Idx>::const_iterator it,ite;
    for(it = arg_order.begin(), ite = arg_order.end(); it!=ite; ++it)
    {
      T* ptr = this->at(*it);
      if(NULL == ptr)
      {
#ifdef DEBUG
      std::cout<<"\nCMappedList<Idx,T>::sort() ERROR : Passed order contains invalid index element.";
#endif
        return false;
      }
    }

    // Verified that all the elements are in the mapped list
    // So proceed to insertion sort.
    for(it = arg_order.begin(), ite = arg_order.end(); it!=ite; ++it)
    {
      // Find the node, starting from idx 0 to end.
      SMLNode<Idx,T>* node = map_[*it]; //Idx is already tested to exist in the map.

      if(node==back_) { continue;   } //Nothing to do. Node is already in the right place.

      // Detach the node from the list.
      if(node==front_)
      {
        node->next_->prev_ = NULL;
        front_ = node->next_;
      }
      else
      {
        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;
      }

      // Re-add the node at the end.
      back_->next_ = node;
      node->prev_ = back_;
      back_ = node;
      null_.prev_ = node;
      node->next_ = &null_;
    }

    flag_is_sorted_ = true;
    sorting_order_ = arg_order;
    return true;
  }
}

#endif /* CMAPPEDLIST_HPP_ */


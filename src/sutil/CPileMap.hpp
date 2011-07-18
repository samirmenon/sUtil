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
/* \file CPileMap.hpp
 *
 *  Created on: Jul 12, 2010
 *
 *  Copyright (C) 2010, Samir Menon <smenon@stanford.edu>
 */

#ifndef CPILEMAP_HPP_
#define CPILEMAP_HPP_

#include <map>

#ifdef DEBUG
#include <iostream>
#endif

#ifdef ASSERT
#include <cassert>
#endif

namespace sutil
{

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
class CPileMap
{
protected:
  /** A node in the linked list */
  template <typename IdxS, typename TS>
  class SPMNode
  {
  public:
    TS* data_;
    IdxS* id_;

    //For the linked list
    SPMNode<IdxS,TS> *next_;

    SPMNode()
    {
      data_=NULL;
      id_=NULL;
      next_=NULL;
    }
  };

  /** Pointers to the head of the list */
  SPMNode<Idx,T> *front_;
  std::map<Idx, SPMNode<Idx,T>*> map_;

  std::size_t size_;

public:
  /** Const pointer access to the list.
   *
   * 1. Can be moved across the list manually to iterate over all the nodes
   * 2. Can be reset to the head of the list.
   */
  const SPMNode<Idx,T> *iterator_;

  /** Reset iterator to head */
  virtual void resetIterator()
  { iterator_ = static_cast<const SPMNode<Idx,T> *>(front_); }

  /** Constructor : Resets the pilemap. */
  CPileMap()
  { front_ = NULL; map_.clear(); size_ = 0; }

  /** Copy-Constructor : Does a deep copy of the pilemap to
   * get a new one.
   *
   * NOTE : This uses the passed pilemap's iterator construct. */
  virtual bool deepCopy(CPileMap<Idx,T>* arg_pmap);

  /** Destructor : Deallocates all the nodes if someone already hasn't
   * done so. */
  virtual ~CPileMap();

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
  virtual T* at(const std::size_t & arg_idx);

  /** Returns the element referenced by the index
   *
   * NOTE : This uses the std::map (and is rather slow) */
  virtual T* at(const Idx & arg_idx);

  /** Returns the element at the given numerical index
   * in the linked list (usually useful only for
   * debugging)
   *
   * NOTE : The index starts at 0   */
  virtual const T* at_const(const std::size_t & arg_idx);

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

  /** Returns the size of the pile */
  virtual inline std::size_t size(){ return size_; }

  /** Clears all elements from the list */
  virtual bool clear();
};

template <typename Idx, typename T>
bool CPileMap<Idx,T>::deepCopy(CPileMap<Idx,T>* arg_pmap)
{//Deep copy.
  this->~CPileMap(); //Delete everything in the pilemap

  /**Set the current pilemap to the new pilemap**/
  if(0 == arg_pmap->size())
  { front_ = NULL; map_.clear(); size_ = 0; }
  else
  {
    arg_pmap->resetIterator();
    while(arg_pmap->iterator_!=NULL)
    {
      T* tmp = create(*(arg_pmap->iterator_->id_),
      *(arg_pmap->iterator_->data_));
      if(NULL == tmp)
      {
#ifdef DEBUG
        std::cout<<"\nCPileMap<Idx,T>::CPileMap(const CPileMap<Idx,T>& arg_pmap) : ";
        std::cout<<"ERROR : Copy constructor failed. Resetting pilemap.";
#endif
        this->~CPileMap();//Reset the pilemap.
        return false;
      }
      arg_pmap->iterator_ = arg_pmap->iterator_->next_;
    }
    arg_pmap->resetIterator();
  }
  return true;
}

template <typename Idx, typename T>
CPileMap<Idx,T>::~CPileMap()
{
  SPMNode<Idx,T> *t, *t2;
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
  map_.clear();
  size_ = 0;
}

template <typename Idx, typename T>
T* CPileMap<Idx,T>::create(const Idx & arg_idx)
{
  SPMNode<Idx,T> * tmp = new SPMNode<Idx,T>();

  if(NULL==tmp) //Memory not allocated
  { return NULL; }

  //Make sure the idx hasn't already been registered.
  if(map_.find(arg_idx) != map_.end())
  {
#ifdef DEBUG
    std::cout<<"\nCPileMap<Idx,T>::create() ERROR : Idx exists. Tried to add duplicate entry";
#endif
    return NULL;
  }

  tmp->data_ = new T();
  tmp->id_ = new Idx(arg_idx);
  tmp->next_ = front_;
  front_ = tmp;
  tmp = NULL;
  size_++;

  map_.insert( std::pair<Idx, SPMNode<Idx,T> *>(arg_idx, front_) );

  return front_->data_;
}

template <typename Idx, typename T>
T* CPileMap<Idx,T>::create(const Idx & arg_idx, const T& arg_t)
{
  SPMNode<Idx,T> * tmp = new SPMNode<Idx,T>();

  if(NULL==tmp) //Memory not allocated
  { return NULL; }

  //Make sure the idx hasn't already been registered.
  if(map_.find(arg_idx) != map_.end())
  {
#ifdef DEBUG
    std::cout<<"\nCPileMap<Idx,T>::create() ERROR : Idx exists. Tried to add duplicate entry";
#endif
    return NULL;
  }

  tmp->data_ = new T(arg_t);
  tmp->id_ = new Idx(arg_idx);
  tmp->next_ = front_;
  front_ = tmp;
  tmp = NULL;
  size_++;

  map_.insert( std::pair<Idx, SPMNode<Idx,T> *>(arg_idx, front_) );

  return front_->data_;
}

template <typename Idx, typename T>
T* CPileMap<Idx,T>::at(const std::size_t & arg_idx)
{
  if(NULL==front_)
  { return NULL;  }
  else
  {
    if(arg_idx > size_)
    { return NULL; }
    SPMNode<Idx,T> * t = front_;

    for(std::size_t i=0; i<arg_idx; ++i)
    {
#ifdef ASSERT
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
T* CPileMap<Idx,T>::at(const Idx & arg_idx)
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

      SPMNode<Idx,T> * t = map_[arg_idx];

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
const T* CPileMap<Idx,T>::at_const(const std::size_t & arg_idx)
{ return (const T*) at(arg_idx);  }

template <typename Idx, typename T>
const T* CPileMap<Idx,T>::at_const(const Idx & arg_idx)
{ return (const T*) at(arg_idx);  }


template <typename Idx, typename T>
bool CPileMap<Idx,T>::erase(T* arg_t)
{
  if((NULL==front_) || (NULL==arg_t))
  { return false;  }

  SPMNode<Idx,T> * t, *tpre;

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
bool CPileMap<Idx,T>::erase(const Idx& arg_idx)
{
  if(NULL==front_)
  { return false;  }

  //Make sure the node exists
  if(map_.find(arg_idx) == map_.end())
  {
#ifdef DEBUG
    std::cout<<"\nCPileMap<Idx,T>::erase() WARNING : Tried to erase a nonexistent entry";
#endif
    return false;
  }

  SPMNode<Idx,T> * t, *tpre;
  SPMNode<Idx,T> * node = map_[arg_idx];

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
bool CPileMap<Idx,T>::clear()
{
  SPMNode<Idx,T> *tpre;
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
  map_.clear(); // Clear the map.
  return true;
}

}

#endif /* CPILEMAP_HPP_ */


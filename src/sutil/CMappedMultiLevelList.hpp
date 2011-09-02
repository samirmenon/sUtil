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
/* \file CMappedMultiLevelList.hpp
 *
 *  Created on: Jun 28, 2010
 *
 *  Copyright (C) 2010, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMAPPEDMULTILEVELLIST_HPP_
#define CMAPPEDMULTILEVELLIST_HPP_

#include <sutil/CMappedList.hpp>

#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  /** This template class contains a multi level linked list.
   *
   * It is an extension of a PileMap, which itself is a
   * collection of pointers (memory managed) stored in a
   * linked list. */
  template <typename Idx, typename T>
  class CMappedMultiLevelList  : public CMappedList<Idx,T>
  {
  public:
    /** Constructor : Empties the data structure */
    CMappedMultiLevelList();

    /** Destructor : Deallocates all the nodes if someone already hasn't
     * done so. */
    virtual ~CMappedMultiLevelList();

    /** Adds a node to the mapped list and inserts its vector into
     * the appropriate slot in the vector-list */
    virtual T* create(const Idx& arg_idx, const T &arg_node2add,
        const std::size_t arg_priority);

    /** Adds a node to the mapped list and inserts its vector into
     * the appropriate slot in the vector-list. Uses the copy-constructor. */
    virtual T* create(const Idx& arg_idx, const std::size_t arg_priority);

    /** Copy-Constructor : Does a deep copy of the mapped multi level list to
     *  get a new one.
     * NOTE : This uses the passed mapped list's iterator construct. */
    virtual bool deepCopy(CMappedMultiLevelList<Idx,T>* arg_br);

    /** Erases an element from the list.
     * Referenced by the element's memory location */
    virtual bool erase(T* arg_t);

    /** Erases an element from the list.
     * Referenced by its index */
    virtual bool erase(const Idx& arg_idx);

    /** Clears all elements from the list */
    virtual bool clear();

    /** Returns the tasks at a level */
    std::vector<T*>* getSinglePriorityLevel(std::size_t arg_pri);

    std::size_t getNumPriorityLevels() const
    { return pri_levels_; }

    /** Returns the priority level of the element
     * Note: This returns an int because -1 is the error
     * condition*/
    int getPriorityLevel(T* arg_t);

    /** Returns the priority level of the element pointed
     * to by the index
     * Note: This returns an int because -1 is the error
     * condition*/
    int getPriorityLevel(const Idx & arg_idx);

    /** Create a node on the mapped list and also insert it into the vector
     * of lists. That way someone can lookup or iterate over the mapped list
     * and can also exploit the structure of the vec<list>
     */
    std::vector<std::vector<T*> > mlvec_;

  protected:
    /** Maintain a map of the node pointer to the priority levels as
     * well. This allows chaining to find out the pri level even from
     * the index (pri == map_nodeptr2pri_[map_[idx]]) &&
     * (pri == map_nodeptr2pri_[nodeptr]) */
    std::map<T*, std::size_t> map_nodeptr2pri_;

    /** The priority levels this multi-level map has */
    std::size_t pri_levels_;
  }; //End of template class

  /***************************************************************
   *******************************Function Definitions*************
   ****************************************************************
   */

  /** Constructor. Sets default values.   */
  template <typename Idx, typename T>
  CMappedMultiLevelList<Idx,T>::CMappedMultiLevelList() :
  CMappedList<Idx,T>::CMappedList()
  { mlvec_.clear(); pri_levels_ = 0; }

  /** Does nothing itself.
   * Calls CMappedList::~CMappedList() which
   * destroys the (dynamically allocated)
   * nodes in the node linked list.   */
  template <typename Idx, typename T>
  CMappedMultiLevelList<Idx,T>::~CMappedMultiLevelList()
  { mlvec_.clear(); map_nodeptr2pri_.clear(); pri_levels_=0; }


  /** Adds a node at the given level and returns its address.   */
  template <typename Idx, typename T>
  T* CMappedMultiLevelList<Idx,T>::create(
      const Idx& arg_idx, const T & arg_node2add,
      const std::size_t arg_priority)
  {
    //Add the node.
    T* tLnk = CMappedList<Idx,T>::create(arg_idx,arg_node2add);

    if(NULL!=tLnk)
    {
      for(std::size_t i=mlvec_.size(); i <= arg_priority; i++)
      {
        std::vector<T*> tmp;
        mlvec_.push_back(tmp);
        pri_levels_++;//Every push back increases pri levels.
      }
      mlvec_[arg_priority].push_back(tLnk);
      map_nodeptr2pri_.insert(std::pair<T*,std::size_t>(tLnk,arg_priority));
    }
    return tLnk;
  }

  template <typename Idx, typename T>
  T* CMappedMultiLevelList<Idx,T>::create(
      const Idx& arg_idx, const std::size_t arg_priority)
  {
    //Add the node.
    T* tLnk = CMappedList<Idx,T>::create(arg_idx);

    if(NULL!=tLnk)
    {
      for(std::size_t i=mlvec_.size(); i <= arg_priority; i++)
      {
        std::vector<T*> tmp;
        mlvec_.push_back(tmp);
        pri_levels_++;//Every push back increases pri levels.
      }
      mlvec_[arg_priority].push_back(tLnk);
      map_nodeptr2pri_.insert(std::pair<T*,std::size_t>(tLnk,arg_priority));
    }

    return tLnk;
  }

  template <typename Idx, typename T>
  bool CMappedMultiLevelList<Idx,T>::
  deepCopy(CMappedMultiLevelList<Idx,T>* arg_br)
  {//Deep copy.
    this->~CMappedMultiLevelList(); //Delete everything in the mapped list

    /**Set the current mapped list to the new mapped list**/
    if(0 == arg_br->size())
    {
      CMappedList<Idx,T>::front_ = NULL;
      CMappedList<Idx,T>::map_.clear();
      CMappedList<Idx,T>::size_ = 0;
      mlvec_.clear(); map_nodeptr2pri_.clear();
    }
    else
    {
      arg_br->resetIterator();
      while(arg_br->iterator_!=NULL)
      {
        T* tmp = CMappedList<Idx,T>::create(*(arg_br->iterator_->id_),
            *(arg_br->iterator_->data_));
        if(NULL == tmp)
        {
#ifdef DEBUG
          std::cerr<<"\nCMultiLevelPileMap<Idx,T>::deepCopy() Error :Deep copy failed. Resetting multi-level mapped list.";
#endif
          clear(); return false;
        }

        //Now recreate the priority map
        if(arg_br->map_nodeptr2pri_.find(tmp) ==
            arg_br->map_nodeptr2pri_.end())
        {
#ifdef DEBUG
          std::cerr<<"\nCMultiLevelPileMap<Idx,T>::deepCopy() Error :Did not find a node in the priority map.";
#endif
          clear(); return false;
        }

        std::size_t pri = arg_br->map_nodeptr2pri_[tmp];
        map_nodeptr2pri_.insert(std::pair<T*,std::size_t>(tmp,pri));

        //Now add the entry to the vector
        for(std::size_t i=mlvec_.size(); i <= pri; i++)
        {
          std::vector<T*> tmp;
          mlvec_.push_back(tmp);
          pri_levels_++;//Every push back increases pri levels.
        }
        mlvec_[pri].push_back(tmp);

        //Move the iterator along
        arg_br->iterator_ = arg_br->iterator_->next_;
      }
      arg_br->resetIterator();
    }
    return true;
  }

  template <typename Idx, typename T>
  bool CMappedMultiLevelList<Idx,T>::erase(T* arg_t)
  {
    bool flag;
    if((NULL==CMappedList<Idx,T>::front_) || (NULL==arg_t))
    { return false;  }
    else
    {
      if(map_nodeptr2pri_.find(arg_t) ==
          map_nodeptr2pri_.end())
      { return false; }

      std::size_t pri = map_nodeptr2pri_[arg_t];

      //Remove it from the priority level vectors
      typename std::vector<T*>::iterator it,ite;
      for(it = mlvec_[pri].begin(),ite = mlvec_[pri].end();
          it!=ite; ++it)
      {
        if(*it == arg_t)
        { mlvec_[pri].erase(it); break; }
      }

      if(pri == pri_levels_ -1)//Deleted from lowest level.
      {
        if(mlvec_[pri].size()==0)
        {//If last one is now empty, remove it (and others if possible)
          typename std::vector<std::vector<T*> >::reverse_iterator itr,itre;
          for(itr = mlvec_.rbegin(),itre = mlvec_.rend();
              itr!=itre; ++itr)
          {
            if((*itr).size() == 0)
              //Use rev_iter.base() func to convert it to normal iter
              //Can only erase a normal iter
              //reverse iterator points to one element closer to begin
              //than the base() function
            { mlvec_.erase((itr+1).base()); }
            else
            { break;  }
          }
        }
      }

      //Remove it from the nodeptr2pri map
      map_nodeptr2pri_.erase(arg_t);

      //Remove it from the mapped list (deallocate memory)
      flag = CMappedList<Idx,T>::erase(arg_t);
      if(false == flag)
      { return false; }
    }
    pri_levels_ = mlvec_.size();
    return true;
  }

  template <typename Idx, typename T>
  bool CMappedMultiLevelList<Idx,T>::erase(const Idx& arg_idx)
  {
    bool flag;
    if(NULL==CMappedList<Idx,T>::front_)
    { return false;  }
    else
    {
      T* t_ptr = CMappedList<Idx,T>::at(arg_idx);
      if(NULL == t_ptr)
      { return false; }

      if(map_nodeptr2pri_.find(t_ptr) ==
          map_nodeptr2pri_.end())
      { return false; }

      std::size_t pri = map_nodeptr2pri_[t_ptr];

      //Remove it from the priority level vectors
      typename std::vector<T*>::iterator it,ite;
      for(it = mlvec_[pri].begin(),ite = mlvec_[pri].end();
          it!=ite; ++it)
      {
        if(*it == t_ptr)
        { mlvec_[pri].erase(it); break; }
      }

      if(pri == pri_levels_ -1)//Deleted from lowest level.
      {
        if(mlvec_[pri].size()==0)
        {//If last one is now empty, remove it (and others if possible)
          typename std::vector<std::vector<T*> >::reverse_iterator itr,itre;
          for(itr = mlvec_.rbegin(),itre = mlvec_.rend();
              itr!=itre; ++itr)
          {
            if((*itr).size() == 0)
              //Use rev_iter.base() func to convert it to normal iter
              //Can only erase a normal iter
              //reverse iterator points to one element closer to begin
              //than the base() function
            { mlvec_.erase((itr+1).base()); }
            else
            { break;  }
          }
        }
      }

      //Remove it from the nodeptr2pri map
      map_nodeptr2pri_.erase(t_ptr);

      //Remove it from the mapped list (deallocate memory)
      flag = CMappedList<Idx,T>::erase(arg_idx);
      if(false == flag)
      { return false; }
    }
    pri_levels_ = mlvec_.size();
    return true;
  }
  template <typename Idx, typename T>
  bool CMappedMultiLevelList<Idx,T>::clear()
  {
    CMappedList<Idx,T>::clear();
    mlvec_.clear();
    map_nodeptr2pri_.clear();
    pri_levels_=0;
    return true;
  }

  template <typename Idx, typename T>
  std::vector<T*>* CMappedMultiLevelList<Idx,T>::
  getSinglePriorityLevel(std::size_t arg_pri)
  {
    if(arg_pri > mlvec_.size())
    { return NULL;  }
    else
    {
      std::vector<T*>* ret = &(mlvec_.at(arg_pri));
      return ret;
    }
  }

  template <typename Idx, typename T>
  int CMappedMultiLevelList<Idx,T>::
  getPriorityLevel(T* arg_t)
  {
    int ret;
    if(map_nodeptr2pri_.find(arg_t) ==
        map_nodeptr2pri_.end())
    { ret = -1; }
    else
    { ret = static_cast<int>(map_nodeptr2pri_[arg_t]);  }

    return ret;
  }

  template <typename Idx, typename T>
  int CMappedMultiLevelList<Idx,T>::
  getPriorityLevel(const Idx & arg_idx)
  {
    T* t_ptr = CMappedList<Idx,T>::at(arg_idx);
    if(NULL == t_ptr)
    { return -1;  }

    int ret;
    if(map_nodeptr2pri_.find(t_ptr) ==
        map_nodeptr2pri_.end())
    { ret = -1; }
    else
    { ret = static_cast<int>(map_nodeptr2pri_[t_ptr]);  }

    return ret;
  }

}//End of namespace sutil

#endif /*CMAPPEDMULTILEVELLIST_HPP_*/

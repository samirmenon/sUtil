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
/* \file CMappedGraph.hpp
 *
 *  Created on: Jul 7, 2014
 *
 *  Copyright (C) 2014, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMAPPEDGRAPH_HPP_
#define CMAPPEDGRAPH_HPP_

#include <sutil/CMappedList.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  /** This template class contains a mapped graph.
   *
   * It is an extension of a MappedList, which itself is a
   * collection of pointers (memory managed) stored in a
   * linked list.
   *
   * The mapped graph extends the MappedList by using
   * a graph representation where root nodes are connected to
   * subgraphs of nodes. This representation is overlaid on
   * the MappedList's linked list so iterating over both is
   * possible.
   * The graph can branch arbitrarily.
   *
   * The TNode structure MUST contain:
   * a) TIdx name_;
   * b) std::vector<TIdx> parent_names_;
   * c) std::vector<TNode*> parent_addrs_;
   * d) std::vector<TNode*> child_addrs_;
   *
   * NOTE : You MUST call CMappedGraph's create functions.
   *
   * NOTE 2 : You MUST set the name_ and parent_names_ fields for the objects
   *          in the mapped graph. The linkNodes function requires this
   *          to organize your (unordered) list of nodes into a graph.
   */
  template <typename TIdx, typename TNode>
  class CMappedGraph : public sutil::CMappedList<TIdx,TNode>
  {
  protected:
    /** The root of the mapped graph */
    TNode* root_node_;

    /** True if the mapped graph has a root. */
    bool has_been_init_;

  public:
    CMappedGraph();
    virtual ~CMappedGraph();
    /** Copy-Constructor : Does a deep copy of the mapped graph to
     * get a new one.
     *
     * NOTE : This uses the passed mappedlist's iterator construct. */
    virtual bool deepCopy(CMappedGraph<TIdx,TNode>* arg_mt);

    /** Adds a node to the mapped graph. The passed node is
     * copied and stored in a vector. A map between the idx and the
     * node is also stored.
     *
     * NOTE : Assumes you have set the name_  and parent_names_ fields
     * in the passed arg_node2add
     *
     * NOTE 2 : There can only be one root node. */
    virtual TNode* create(const TIdx& arg_idx, const TNode & arg_node2add,
        const bool arg_is_root_);

    /** Adds a node to the mapped graph.
     *
     * NOTE : Assumes you will set the name_  and parent_names_ fields
     * in the returned arg_node2add
     *
     * NOTE 2 : There can only be one root node. */
    virtual TNode* create(const TIdx& arg_idx, const bool arg_is_root_);

    /** Adds an existing object to the mapped graph. The passed node is
     * stored in a vector. A map between the idx and the node is also stored.
     *
     * NOTE : Assumes you have set the name_  and parent_names_ fields
     * in the passed arg_node2add
     * NOTE 2 : There can only be one root node. */
    virtual TNode* insert(const TIdx& arg_idx, TNode *arg_node2add,
        const bool arg_is_root_);

    /** Organizes the links into a graph. */
    virtual bool linkNodes();

    /** Returns a pointer to the root node */
    virtual const TNode* getRootNodeConst() const
    { return static_cast<const TNode*>(root_node_); }

    /** Returns a pointer to the root node */
    virtual TNode* getRootNode()
    { return root_node_; }

//    /** Determines if the child has the other node as an ancestor */
//    virtual bool isAncestor(const TIdx& arg_idx_child,
//        const TIdx& arg_idx_ancestor)  const;
//
//    /** Determines if the child has the other node as an ancestor */
//    virtual bool isAncestor(const TNode* arg_node_child,
//        const TNode* arg_node_ancestor)  const;
  }; //End of template class

  /***************************************************************
   *******************************Function Definitions*************
   ****************************************************************
   */

  /**
   * Constructor. Sets default values.
   */
  template <typename TIdx, typename TNode>
  CMappedGraph<TIdx,TNode>::CMappedGraph()
  {
    root_node_ = NULL;
    has_been_init_ = false;
  }

  /**
   * Does nothing itself.
   * Calls CMappedList::~CMappedList() which
   * destroys the (dynamically allocated)
   * nodes in the node-vector.
   */
  template <typename TIdx, typename TNode>
  CMappedGraph<TIdx,TNode>::~CMappedGraph()
  {}

  template <typename TIdx, typename TNode>
  bool CMappedGraph<TIdx,TNode>::
  deepCopy(CMappedGraph<TIdx,TNode>* arg_mt)
  {//Deep copy.
    bool flag;
    flag = sutil::CMappedList<TIdx,TNode>::
        deepCopy(arg_mt);
    if(true == flag)
    {
      this->root_node_ = CMappedList<TIdx,TNode>::at(arg_mt->getRootNode()->name_);
      this->has_been_init_ = arg_mt->has_been_init_;
      return true;
    }
    else
    { return false; }
  }

  /**
   * Adds a node to the root or child node vectors depending on the
   * type of node to be added.
   *
   * NOTE : Assumes you have set the name_  and parent_names_ fields
   * in the passed arg_node2add
   *
   * Passed: node to add, its index name and whether it is a root.
   */
  template <typename TIdx, typename TNode>
  TNode* CMappedGraph<TIdx,TNode>::create(
      const TIdx& arg_idx, const TNode & arg_node2add,
      const bool arg_is_root_)
      {
    if((arg_is_root_)&&(NULL!=root_node_))
    {
#ifdef DEBUG
      std::cerr<<"\nCBranchingStructure::create() : Error. Tried to insert a root node when one already exists.";
#endif
      return NULL;
    }

    //Add the node.
    TNode* tLnk =
        sutil::CMappedList<TIdx,TNode>::create(arg_idx,arg_node2add);

    if((arg_is_root_) && (root_node_==NULL))
    { root_node_ = tLnk;  }

    return tLnk;
      }

  /**
   * Adds a node to the root or child node vectors depending on the
   * type of node to be added.
   *
   * Passed: node to add, its index name and whether it is a root.
   */
  template <typename TIdx, typename TNode>
  TNode* CMappedGraph<TIdx,TNode>::create(
      const TIdx& arg_idx, const bool arg_is_root_)
      {
    if((arg_is_root_)&&(NULL!=root_node_))
    {
#ifdef DEBUG
      std::cerr<<"\nCBranchingStructure::create() : Error. Tried to insert a root node when one already exists.";
#endif
      return NULL;
    }

    //Add the node.
    TNode* tLnk = sutil::CMappedList<TIdx,TNode>::create(arg_idx);

    if((arg_is_root_) && (NULL==root_node_))
    { root_node_ = tLnk;  }

    return tLnk;
      }

  /**
   * Adds an existing object to the root or child node vectors depending on the
   * type of node to be added.
   *
   * NOTE : Assumes you have set the name_  and parent_names_ fields
   * in the passed arg_node2add
   * Passed: node to add, its index name and whether it is a root.
   */
  template <typename TIdx, typename TNode>
  TNode* CMappedGraph<TIdx,TNode>::insert(
      const TIdx& arg_idx, TNode *arg_node2add,
      const bool arg_is_root_)
  {
    if((arg_is_root_)&&(NULL!=root_node_))
    {
#ifdef DEBUG
      std::cerr<<"\nCBranchingStructure::create() : Error. Tried to insert a root node when one already exists.";
#endif
      return NULL;
    }

    //Add the node.
    TNode* tLnk = sutil::CMappedList<TIdx,TNode>::create(arg_idx,arg_node2add);

    if((arg_is_root_) && (root_node_==NULL))
    { root_node_ = tLnk;  }

    return tLnk;
  }


  /**
   * Organizes all the links in the graph by connecting them to their
   * parents.
   *
   * O(n*log(n))
   */
  template <typename TIdx, typename TNode>
  bool CMappedGraph<TIdx,TNode>::linkNodes()
  {
    //Clear previous links (if any)
    typename CMappedList<TIdx,TNode>::iterator it,ite;
    for(it = CMappedList<TIdx,TNode>::begin(),
        ite = CMappedList<TIdx,TNode>::end();
        it != ite; ++it)
    {
      TNode& tmp_node = *it;
      tmp_node.parent_addrs_.clear();
      tmp_node.child_addrs_.clear();
    }

    //Form the new links
    for(it = CMappedList<TIdx,TNode>::begin(),
        ite = CMappedList<TIdx,TNode>::end();
        it != ite; ++it)
    {
      TNode& tmp_node = *it;
      //Iterate over all nodes and connect them to their
      //parents
      if(&tmp_node == root_node_)
      {//No parents
        has_been_init_ = true;
        continue;
      }
      else
      {
        typename std::vector<TIdx>::iterator itp,itpe; //Each TIdx corresponds to an entry in : std::vector<TNode*> parent_addrs_;
        for(itp = tmp_node.parent_names_.begin(), itpe = tmp_node.parent_names_.end();
            itp!=itpe;++itp)
        {
          TNode * tmp_node2add = sutil::CMappedList<TIdx,TNode>::at(*itp);
          if(tmp_node2add == NULL)
          {//No parent -- Ignore this node
#ifdef DEBUG
            std::cerr<<"\nCBranchingStructure::linkNodes(): Warning.";
            std::cerr<<"Orphan node found: "<<*itp<<". Ignoring.";
#endif
            continue;
          }
          //Link the node
          tmp_node.parent_addrs_.push_back(tmp_node2add);
          tmp_node2add->child_addrs_.push_back(&tmp_node);

#ifdef DEBUG
          std::cerr<<"\n\tAdding child "<<tmp_node.name_
              <<" to (parent) "<<tmp_node2add->name_;
          std::cerr<<std::flush;
#endif
        }
      }
    }//End of while loop
    return has_been_init_;
  }


//  /** Determines if the child has the other node as an ancestor */
//  template <typename TIdx, typename TNode>
//  bool CMappedGraph<TIdx,TNode>::isAncestor(const TIdx& arg_idx_child,
//      const TIdx& arg_idx_ancestor) const
//  {
//    const TNode *child = this->at_const(arg_idx_child);
//    const TNode *ancestor = this->at_const(arg_idx_child);
//    if( NULL == child || NULL == ancestor)
//    { return false; }
//
//    while(NULL != child)
//    {
//      if(ancestor == child)
//      { return true; }
//      child = child->parent_addrs_;
//    }
//    return false;
//  }
//
//  /** Determines if the child has the other node as an ancestor */
//  template <typename TIdx, typename TNode>
//    bool CMappedGraph<TIdx,TNode>::isAncestor(const TNode* arg_node_child,
//      const TNode* arg_node_ancestor)  const
//  {
//    const TNode *child = arg_node_child;
//    if( NULL == child || NULL == arg_node_ancestor)
//    { return false; }
//
//    while(NULL != child)
//    {
//      if(arg_node_ancestor == child)
//      { return true; }
//      child = child->parent_addrs_;
//    }
//    return false;
//  }

}//End of namespace sutil

#endif /*CMAPPEDGRAPH_HPP_*/

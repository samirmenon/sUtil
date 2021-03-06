/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CMappedTree.hpp
 *
 *  Created on: Dec 27, 2010
 *
 *  Copyright (C) 2010, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMAPPEDTREE_HPP_
#define CMAPPEDTREE_HPP_

#include <sutil/CMappedList.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace sutil
{
  /** This template class contains a mapped tree.
   *
   * It is an extension of a MappedList, which itself is a
   * collection of pointers (memory managed) stored in a
   * linked list.
   *
   * The mapped tree extends the MappedList by using
   * a tree representation where root nodes are connected to
   * subtrees of nodes. This representation is overlaid on
   * the MappedList's linked list so iterating over both is
   * possible.
   * The tree can branch arbitrarily.
   *
   * The TNode structure MUST contain:
   * a) TIdx name_;
   * b) TIdx parent_name_;
   * c) TNode* parent_addr_;
   * d) std::vector<TNode*> child_addrs_;
   *
   * NOTE : You MUST call CMappedTree's create functions.
   *
   * NOTE 2 : You MUST set the name_ and parent_name_ fields for the objects
   *          in the mapped tree. The linkNodes function requires this
   *          to organize your (unordered) list of nodes into a tree.
   */
  template <typename TIdx, typename TNode>
  class CMappedTree : public sutil::CMappedList<TIdx,TNode>
  {
  protected:
    /** The root of the mapped tree */
    TNode* root_node_;

    /** True if the mapped tree has a root. */
    bool has_been_init_;

    /** Copy-Constructor : Does a deep copy of the mapped tree to
     * get a new one.
     *
     * NOTE : This uses the passed mappedlist's iterator construct. */
    virtual bool deepCopy(const CMappedTree<TIdx,TNode>* arg_mt);

  public:
    /** Base class to simplify tree node specification (parent pointers etc.) */
    struct SMTNodeBase;

    /** Default constructor : Sets defaults */
    CMappedTree();

    /** Copy Constructor : Performs a deep-copy (std container requirement).
     * 'explicit' makes sure that only a CMappedTree can be copied. Ie. Implicit
     * copy-constructor use is disallowed.*/
    explicit CMappedTree(const CMappedTree<TIdx,TNode>& arg_mt):
        CMappedList<TIdx,TNode>()
    { CMappedTree<TIdx,TNode>::deepCopy(&arg_mt); }

    /** Default destructor : Deallocs stuff */
    virtual ~CMappedTree();

    /** Assignment operator : Performs a deep-copy (std container requirement).
     * Beware; This can be quite slow. */
    virtual CMappedTree<TIdx,TNode>& operator = (const CMappedTree<TIdx,TNode>& arg_rhs)
    {
      CMappedTree<TIdx,TNode>::deepCopy(&arg_rhs);
      return *this;
    }

    /** Adds a node to the mapped tree. The passed node is
     * copied and stored in a vector. A map between the idx and the
     * node is also stored.
     *
     * NOTE : Assumes you have set the name_  and parent_name_ fields
     * in the passed arg_node2add
     *
     * NOTE 2 : There can only be one root node. */
    virtual TNode* create(const TIdx& arg_idx, const TNode & arg_node2add,
        const bool arg_is_root_);

    /** Adds a node to the mapped tree.
     *
     * NOTE : Assumes you will set the name_  and parent_name_ fields
     * in the returned arg_node2add
     *
     * NOTE 2 : There can only be one root node. */
    virtual TNode* create(const TIdx& arg_idx, const bool arg_is_root_);

    /** Adds an existing object to the mapped tree. The passed node is
     * stored in a vector. A map between the idx and the node is also stored.
     *
     * NOTE : Assumes you have set the name_  and parent_name_ fields
     * in the passed arg_node2add
     * NOTE 2 : There can only be one root node. */
    virtual TNode* insert(const TIdx& arg_idx, TNode *arg_node2add,
        const bool arg_is_root_);

    /** Organizes the links into a tree. */
    virtual bool linkNodes();

    /** Returns a pointer to the root node */
    virtual const TNode* getRootNodeConst() const
    { return static_cast<const TNode*>(root_node_); }

    /** Returns a pointer to the root node */
    virtual TNode* getRootNode()
    { return root_node_; }

    /** Determines if the child has the other node as an ancestor */
    virtual bool isAncestor(const TIdx& arg_idx_child,
        const TIdx& arg_idx_ancestor)  const;

    /** Determines if the child has the other node as an ancestor */
    virtual bool isAncestor(const TNode* arg_node_child,
        const TNode* arg_node_ancestor)  const;

    /** Determines if the parent has the other node as a descendant in the tree */
    virtual bool isDescendant(const TIdx& arg_idx_parent,
        const TIdx& arg_idx_descendant)  const;

    /** Determines if the parent has the other node as a descendant in the tree */
    virtual bool isDescendant(const TNode* arg_node_parent,
        const TNode* arg_node_descendant)  const;

    /** Clears all elements from the tree */
    virtual bool clear();
  }; //End of template class


  /** Node type base class (sets all the pointers etc. that will be required */
  template <typename TIdx, typename TNode>
  struct CMappedTree<TIdx,TNode>::SMTNodeBase
  {
  public:
    /** The index of this node */
    TIdx name_;
    /** The parent index in the graph */
    TIdx parent_name_;
    /** The parent node address pointer in the graph */
    TNode* parent_addr_;
    /** The child node address pointers in the graph */
    std::vector<TNode*> child_addrs_;

    /** Constructor. Sets stuff to NULL */
    SMTNodeBase()
    {
      name_ = "";
      parent_name_ = "";
      parent_addr_ = NULL;
      child_addrs_.clear();
    }
  };

  /***************************************************************
   *******************************Function Definitions*************
   ****************************************************************
   */

  /**
   * Constructor. Sets default values.
   */
  template <typename TIdx, typename TNode>
  CMappedTree<TIdx,TNode>::CMappedTree() : CMappedList<TIdx,TNode>()
  {
    root_node_ = NULL;
    has_been_init_ = false;
  }

  /** Sets stuff to null
   * Calls CMappedList::~CMappedList() which
   * destroys the (dynamically allocated)
   * nodes in the node-vector.
   */
  template <typename TIdx, typename TNode>
  CMappedTree<TIdx,TNode>::~CMappedTree()
  {
    root_node_ = NULL;
    has_been_init_ = false;
  }

  template <typename TIdx, typename TNode>
  bool CMappedTree<TIdx,TNode>::
  deepCopy(const CMappedTree<TIdx,TNode>* const arg_mt)
  {//Deep copy.
    bool flag;
    clear(); //Clear stuff (avoids memory leaks).
    flag = CMappedList<TIdx,TNode>::deepCopy(
        dynamic_cast<const CMappedTree<TIdx,TNode>*>(arg_mt));
    if(true == flag)
    {
      this->root_node_ = CMappedList<TIdx,TNode>::at(arg_mt->getRootNodeConst()->name_);
      flag = linkNodes();
      if(flag)
      {
        this->has_been_init_ = arg_mt->has_been_init_;
        return true;
      }
    }
    // Else.. Failed for some reason..
    clear(); //Leave a clean state for the future.
    return false;
  }

  /**
   * Adds a node to the root or child node vectors depending on the
   * type of node to be added.
   *
   * NOTE : Assumes you have set the name_  and parent_name_ fields
   * in the passed arg_node2add
   *
   * Passed: node to add, its index name and whether it is a root.
   */
  template <typename TIdx, typename TNode>
  TNode* CMappedTree<TIdx,TNode>::create(
      const TIdx& arg_idx, const TNode & arg_node2add,
      const bool arg_is_root_)
      {
    if((arg_is_root_)&&(NULL!=root_node_))
    {
#ifdef DEBUG
      std::cerr<<"\nCMappedTree::create() : Error. Tried to insert a root node when one already exists.";
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
  TNode* CMappedTree<TIdx,TNode>::create(
      const TIdx& arg_idx, const bool arg_is_root_)
      {
    if((arg_is_root_)&&(NULL!=root_node_))
    {
#ifdef DEBUG
      std::cerr<<"\nCMappedTree::create() : Error. Tried to insert a root node when one already exists.";
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
   * NOTE : Assumes you have set the name_  and parent_name_ fields
   * in the passed arg_node2add
   * Passed: node to add, its index name and whether it is a root.
   */
  template <typename TIdx, typename TNode>
  TNode* CMappedTree<TIdx,TNode>::insert(
      const TIdx& arg_idx, TNode *arg_node2add,
      const bool arg_is_root_)
  {
    if((arg_is_root_)&&(NULL!=root_node_))
    {
#ifdef DEBUG
      std::cerr<<"\nCMappedTree::create() : Error. Tried to insert a root node when one already exists.";
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
   * Organizes all the links in the tree by connecting them to their
   * parents.
   *
   * O(n*log(n))
   */
  template <typename TIdx, typename TNode>
  bool CMappedTree<TIdx,TNode>::linkNodes()
  {
    if(NULL == getRootNodeConst())
    { return false; }

    //Clear previous links (if any)
    typename CMappedList<TIdx,TNode>::iterator it,ite;
    for(it = CMappedList<TIdx,TNode>::begin(),
        ite = CMappedList<TIdx,TNode>::end();
        it != ite; ++it)
    {
      TNode& tmp_node = *it;
      tmp_node.parent_addr_ = NULL;
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
        tmp_node.parent_addr_ =
            sutil::CMappedList<TIdx,TNode>::at(tmp_node.parent_name_);
        if(tmp_node.parent_addr_ == NULL)
        {//No parent -- Ignore this node
#ifdef DEBUG
          std::cerr<<"\nCMappedTree::linkNodes(): Warning.";
          std::cerr<<"Orphan node found: "<<tmp_node.name_<<". Ignoring.";
#endif
          continue;
        }
        tmp_node.parent_addr_->child_addrs_.push_back(&tmp_node);

#ifdef DEBUG
        std::cerr<<"\n\tAdding child "<<tmp_node.name_
            <<" to (parent) "<<tmp_node.parent_addr_->name_;
        std::cerr<<std::flush;
#endif
      }
    }//End of while loop
    return has_been_init_;
  }


  /** Determines if the child has the other node as an ancestor */
  template <typename TIdx, typename TNode>
  bool CMappedTree<TIdx,TNode>::isAncestor(const TIdx& arg_idx_child,
      const TIdx& arg_idx_ancestor) const
  { return isAncestor(this->at_const(arg_idx_child), this->at_const(arg_idx_child));  }

  /** Determines if the child has the other node as an ancestor */
  template <typename TIdx, typename TNode>
    bool CMappedTree<TIdx,TNode>::isAncestor(const TNode* arg_node_child,
      const TNode* arg_node_ancestor)  const
  {
    const TNode *child = arg_node_child;
    if( NULL == child || NULL == arg_node_ancestor)
    { return false; }

    while(NULL != child)
    {
      if(arg_node_ancestor == child)
      { return true; }
      child = child->parent_addr_;
    }
    return false;
  }

  /** Determines if the parent has the other node as a descendant */
  template <typename TIdx, typename TNode>
  bool CMappedTree<TIdx,TNode>::isDescendant(const TIdx& arg_idx_parent,
      const TIdx& arg_idx_descendant) const
  { return isDescendant(this->at_const(arg_idx_parent), this->at_const(arg_idx_descendant)); }

  /** Determines if the parent has the other node as a descendant */
  template <typename TIdx, typename TNode>
  bool CMappedTree<TIdx,TNode>::isDescendant(const TNode* arg_node_parent,
      const TNode* arg_node_descendant)  const
  {
    const TNode *parent = arg_node_parent;
    if( NULL == parent || NULL == arg_node_descendant)
    { return false; }

    if(NULL != parent)
    {
      if(arg_node_descendant == parent)
      { return true; }

      typename std::vector<TNode*>::const_iterator it,ite;
      for(it = parent->child_addrs_.begin(), ite = parent->child_addrs_.end(); it!=ite; ++it)
      {
        if(isDescendant(*it, arg_node_descendant))
        { return true;  }
      }
    }
    return false;
  }

  /** Clears all elements from the tree */
  template <typename TIdx, typename TNode>
  bool CMappedTree<TIdx,TNode>::clear()
  {
    bool flag = CMappedList<TIdx,TNode>::clear();
    if(flag)
    {
      root_node_ = NULL;
      has_been_init_ = false;
    }
    return flag;
  }

}//End of namespace sutil

#endif /*CMAPPEDTREE_HPP_*/

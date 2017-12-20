/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file CMappedDirGraph.hpp
 *
 *  Created on: Jul 7, 2014
 *
 *  Copyright (C) 2014, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMAPPEDDIRGRAPH_HPP_
#define CMAPPEDDIRGRAPH_HPP_

#include <sutil/CMappedTree.hpp>
#include <limits>

#ifdef DEBUG
#include <iostream>
#include <cassert>
#endif

namespace sutil
{
  /** This template class contains a mapped graph.
   *
   * It is an extension of a MappedTree, which itself is a
   * collection of pointers (memory managed) stored in a
   * tree (which is further stored in a linked list).
   *
   * The mapped graph extends the MappedTree by using
   * a graph representation where root nodes are connected to
   * subgraphs of nodes. This representation is overlaid on
   * the MappedTree's tree so iterating over both is
   * possible.
   * The graph can branch arbitrarily.
   *
   * The TNode structure MUST contain (in addition to MappedTree's
   * node requirements):
   * a) std::vector<TIdx> parent_names_;
   * b) std::vector<TNode*> parent_addrs_;
   * c) std::vector<TNode*> child_addrs_;
   *
   * NOTE : You MUST call CMappedDirGraph's create functions.
   *
   * NOTE 2 : You MUST set the name_ and parent_names_ fields for the objects
   *          in the mapped graph. The linkNodes function requires this
   *          to organize your (unordered) list of nodes into a graph.
   */
  template <typename TIdx, typename TNode>
  class CMappedDirGraph : public sutil::CMappedTree<TIdx,TNode>
  {
  public:
    /** These are the edges broken while creating the spanning tree
     * (useful for solving constraints etc. in closed loop / recurrent systems) */
    std::vector<std::pair<TNode*, TNode*> > st_broken_edges_;

    /** Base class to simplify graph node specification (parent pointers etc.) */
    struct SMGNodeBase;

    CMappedDirGraph() : CMappedTree<TIdx,TNode>::CMappedTree() { st_broken_edges_.clear(); }

    /** Copy Constructor : Performs a deep-copy (std container requirement).
     * 'explicit' makes sure that only a CMappedDirGraph can be copied. Ie. Implicit
     * copy-constructor use is disallowed.*/
    explicit CMappedDirGraph(const CMappedDirGraph<TIdx,TNode>& arg_dg) :
        st_broken_edges_(arg_dg.st_broken_edges_)
    {
      const CMappedTree<TIdx,TNode> &tmp_ref = arg_dg;
      CMappedTree<TIdx,TNode>::deepCopy(&tmp_ref);
    }


    virtual ~CMappedDirGraph() { }

    /** Organizes the links into a graph. */
    virtual bool linkNodes();

    /** Generates the spanning tree for the graph and store it in the mapped tree pointer structure*/
    virtual bool genSpanningTree();

    /** Clears all elements from the tree */
    virtual bool clear();
  }; //End of template class

  /** Node type base class (sets all the pointers etc. that will be required */
  template <typename TIdx, typename TNode>
  struct CMappedDirGraph<TIdx,TNode>::SMGNodeBase : public CMappedTree<TIdx,TNode>::SMTNodeBase
  {
  public:
    /** The parent indices in the graph */
    std::vector<TIdx> gr_parent_names_;
    /** The parent node address pointers in the graph */
    std::vector<TNode*> gr_parent_addrs_;
    /** The child node address pointers in the graph */
    std::vector<TNode*> gr_child_addrs_;

    /** Constructor. Sets stuff to NULL */
    SMGNodeBase() : CMappedTree<TIdx,TNode>::SMTNodeBase()
    {
      gr_parent_names_.clear();
      gr_parent_addrs_.clear();
      gr_child_addrs_.clear();
    }
  };

  /***************************************************************
   *******************************Function Definitions*************
   ****************************************************************
   */
  /** Organizes all the links in the graph by connecting them to their
   * parents. Also creates a spanning tree for the graph.
   *
   * O(n*log(n))
   */
  template <typename TIdx, typename TNode>
  bool CMappedDirGraph<TIdx,TNode>::linkNodes()
  {
    //Clear previous links (if any)
    typename CMappedList<TIdx,TNode>::iterator it,ite;
    for(it = CMappedList<TIdx,TNode>::begin(),
        ite = CMappedList<TIdx,TNode>::end();
        it != ite; ++it)
    {
      TNode& tmp_node = *it;

      //Clear the graph
      tmp_node.gr_parent_addrs_.clear();
      tmp_node.gr_child_addrs_.clear();
    }

    //Form the new links for the graph
    for(it = CMappedList<TIdx,TNode>::begin(), ite = CMappedList<TIdx,TNode>::end();
        it != ite; ++it)
    {
      TNode& tmp_node = *it;
      //Iterate over all nodes and connect them to their
      //parents
      if(&tmp_node == CMappedTree<TIdx,TNode>::root_node_)
      {//No parents
        continue;
      }
      else
      {
        typename std::vector<TIdx>::iterator itp,itpe; //Each TIdx corresponds to an entry in : std::vector<TNode*> parent_addrs_;
        for(itp = tmp_node.gr_parent_names_.begin(), itpe = tmp_node.gr_parent_names_.end();
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
          tmp_node.gr_parent_addrs_.push_back(tmp_node2add);
          tmp_node2add->child_addrs_.push_back(&tmp_node);

#ifdef DEBUG
          std::cerr<<"\n\tAdding child "<<tmp_node.name_
              <<" to (parent) "<<tmp_node2add->name_;
          std::cerr<<std::flush;
#endif
        }
      }
    }//End of while loop

    //Now set up the spanning tree and affirm initialization is complete.
    bool flag = genSpanningTree();
    CMappedTree<TIdx,TNode>::has_been_init_ = false; //Not done yet.

    //Now compute the broken edges.
    st_broken_edges_.clear();
    for(it = CMappedList<TIdx,TNode>::begin(), ite = CMappedList<TIdx,TNode>::end();
        it != ite; ++it)
    {
      TNode &tmp_node = *it;
      typename std::vector<TIdx>::iterator itp,itpe; //Each TIdx corresponds to an entry in : std::vector<TNode*> parent_addrs_;
      for(itp = tmp_node.gr_parent_names_.begin(), itpe = tmp_node.gr_parent_names_.end();
          itp!=itpe;++itp)
      {
        TIdx &pidx = *itp;
        TNode* test_parent = CMappedList<TIdx,TNode>::at(pidx);
        if(test_parent != tmp_node.parent_addr_)
        {//Found a parent who is disconnected in the spanning tree
          std::pair<TNode*, TNode*> tmp_broken_edge;
          tmp_broken_edge.first = test_parent;
          tmp_broken_edge.second = &tmp_node;
          st_broken_edges_.push_back(tmp_broken_edge);
        }
      }
    }

    CMappedTree<TIdx,TNode>::has_been_init_ = true; //Finally done.

    //Return the end result
    return CMappedTree<TIdx,TNode>::has_been_init_;
  }

  /** Generates the spanning tree for the graph and stores it in the mapped tree pointer structure
   *
   * NOTE TODO : Make this function more efficient.
   */
  template <typename TIdx, typename TNode>
  bool CMappedDirGraph<TIdx,TNode>::genSpanningTree()
  {
    // Must have a root node to be able to create the spanning tree.
    // NOTE TODO : Potentially eliminate this requirement and pick a suitable root node.
    TNode* root = CMappedTree<TIdx,TNode>::getRootNode();
    if(NULL == root)
    { return false; }

    int graph_sz = CMappedList<TIdx,TNode>::size();

    struct SSTreeStruct{  bool in_stree_; TNode* node_; };
    SSTreeStruct *in_stree = new SSTreeStruct[graph_sz];

    //First populate the node pointers (create a temp data struct to store stuff).
    typename CMappedTree<TIdx,TNode>::iterator it,ite;
    int i=0;
    for(it = CMappedTree<TIdx,TNode>::begin(), ite = CMappedTree<TIdx,TNode>::end(); it!=ite; ++it)
    {
      //Get the node's numeric index in the underlying mapped list.
      i = CMappedList<TIdx,TNode>::getIndexNumericAt(&(*it));
      //Set values at corresponding position in the data struct array
      in_stree[i].node_ = &(*it);
      if(CMappedTree<TIdx,TNode>::getRootNodeConst() == in_stree[i].node_)
      { in_stree[i].in_stree_ = true; }
      else
      { in_stree[i].in_stree_ = false;  }
    }

    //Now compute the spanning tree.
    bool complete = false;
    int nodes_in_stree_pre = 1, nodes_in_stree_curr = 1;//Only root so far.
    while(false == complete)
    {
      //Update distances from the spanning tree for each node
      for(int i=0; i<graph_sz; ++i)
      {
        if(in_stree[i].in_stree_){  continue; } //I'm already in.

        // Else try to find a parent for me.
        typename std::vector<TIdx>::iterator itp, itpe;
        for(itp = in_stree[i].node_->gr_parent_names_.begin(),itpe = in_stree[i].node_->gr_parent_names_.end();
            itp!=itpe;++itp)
        {
          TIdx &pidx = *itp;
          int pnidx = CMappedList<TIdx,TNode>::getIndexNumericAt(pidx); //numeric parent index
#ifdef DEBUG
          assert(-1 != pnidx);
#endif
          if(in_stree[pnidx].in_stree_)
          {//Parent is in the spanning tree. Then add me as a child and forget about other parents.
            in_stree[i].node_->parent_name_ = in_stree[pnidx].node_->name_;
            in_stree[i].in_stree_ = true;
            nodes_in_stree_curr++;
            break;
          }
        }
      }

      // Check if all nodes have been added to the spanning tree.
      complete = true;
      for(int i=0; complete && i<graph_sz; ++i)
      { complete = (complete && in_stree[i].in_stree_); }

      if(false == complete && nodes_in_stree_curr == nodes_in_stree_pre)
      { return false; }//Could not add any node this time. And not complete. Algo is stuck.
      else
      { nodes_in_stree_pre = nodes_in_stree_curr; }
    }

    //Start at the root node (which has, presumably, been set)
    return CMappedTree<TIdx,TNode>::linkNodes();
  }

  /** Clears all elements from the tree */
  template <typename TIdx, typename TNode>
  bool CMappedDirGraph<TIdx,TNode>::clear()
  {
    bool flag = CMappedTree<TIdx,TNode>::clear();
    if(flag)
    { st_broken_edges_.clear(); }
    return flag;
  }


}//End of namespace sutil

#endif /*CMAPPEDDIRGRAPH_HPP_*/

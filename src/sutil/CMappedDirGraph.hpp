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
/* \file CMappedDirGraph.hpp
 *
 *  Created on: Jul 7, 2014
 *
 *  Copyright (C) 2014, Samir Menon <smenon@stanford.edu>
 */

#ifndef CMAPPEDDIRGRAPH_HPP_
#define CMAPPEDDIRGRAPH_HPP_

#include <sutil/CMappedTree.hpp>

#ifdef DEBUG
#include <iostream>
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
    /** Base class to simplify graph node specification (parent pointers etc.) */
    struct SMGNodeBase;

    CMappedDirGraph() : CMappedTree<TIdx,TNode>::CMappedTree() { }
    virtual ~CMappedDirGraph() { }

    /** Organizes the links into a graph. */
    virtual bool linkNodes();

    /** Generates the spanning tree for the graph and store it in the mapped tree pointer structure*/
    virtual bool genSpanningTree();
  }; //End of template class

  /** Node type base class (sets all the pointers etc. that will be required */
  template <typename TIdx, typename TNode>
  struct CMappedDirGraph<TIdx,TNode>::SMGNodeBase : public CMappedTree<TIdx,TNode>::SMTNodeBase
  {
  public:
    /** The parent index in the graph */
    TIdx &st_parent_name_;
    /** The parent node address pointer in the graph */
    TNode* &st_parent_addr_;
    /** The child node address pointers in the graph */
    std::vector<TNode*> &st_child_addrs_;
    /** These are the edges broken while creating the spanning tree
     * (useful for solving constraints etc. in closed loop / recurrent systems) */
    struct SEdgeBrokenBySTree{  const TNode *parent, *child;  };
    std::vector<SEdgeBrokenBySTree> st_broken_edges_;

    /** The parent indices in the graph */
    std::vector<TIdx> gr_parent_names_;
    /** The parent node address pointers in the graph */
    std::vector<TNode*> gr_parent_addrs_;
    /** The child node address pointers in the graph */
    std::vector<TNode*> gr_child_addrs_;

    /** Constructor. Sets stuff to NULL */
    SMGNodeBase() : CMappedTree<TIdx,TNode>::SMTNodeBase(),
        st_parent_name_(CMappedTree<TIdx,TNode>::SMTNodeBase::parent_name_),
        st_parent_addr_(CMappedTree<TIdx,TNode>::SMTNodeBase::parent_addr_),
        st_child_addrs_(CMappedTree<TIdx,TNode>::SMTNodeBase::child_addrs_)
    {
      gr_parent_names_.clear();
      gr_parent_addrs_.clear();
      gr_child_addrs_.clear();

      // Add a st_ prefix for data related to the spanning tree over this graph.
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

      //Clear the spanning tree
      tmp_node.st_parent_addr_ = NULL;
      tmp_node.st_child_addrs_.clear();
    }

    //Form the new links for the graph
    for(it = CMappedList<TIdx,TNode>::begin(),
        ite = CMappedList<TIdx,TNode>::end();
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
    CMappedTree<TIdx,TNode>::has_been_init_ = genSpanningTree();

    //Return the end result
    return CMappedTree<TIdx,TNode>::has_been_init_;
  }

  /** Generates the spanning tree for the graph and store it in the mapped tree pointer structure*/
  template <typename TIdx, typename TNode>
  bool CMappedDirGraph<TIdx,TNode>::genSpanningTree()
  {
    // Must have a root node to be able to create the spanning tree.
    // NOTE TODO : Potentially eliminate this requirement and pick a suitable root node.
    TNode* root = CMappedTree<TIdx,TNode>::getRootNode();
    if(NULL == root)
    { return false; }

    std::vector<TNode*> nodes_in_tree;
    nodes_in_tree.push_back(root);

    //addChildren

    //Start at the root node (which has, presumably, been set)
    return true;
  }

}//End of namespace sutil

#endif /*CMAPPEDDIRGRAPH_HPP_*/

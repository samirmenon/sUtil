/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file test-mapped-graph.cpp
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#include "test-mapped-graph.hpp"

#include <sutil/CMappedDirGraph.hpp>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <stdio.h>

namespace sutil_test
{
  /** Test Structure - Mapped graph Node.
   * Inherit all the pointers etc. */
  struct _testSMGNode : public sutil::CMappedDirGraph<std::string,_testSMGNode>::SMGNodeBase
  {
  public:
    int random_data_;
  };

  /**
   * Tests the mapped graph with the graph:
   *            ground (not a link)
   *              --
   *             root
   *            /    \
   *           l1    r1
   *          /  \  /  \
   *         l2   l3<--r2
   */
  void test_mapped_graph(int arg_id)
  {
    unsigned int test_id=0;
    try
    {
      //0. Create a mapped graph
      sutil::CMappedDirGraph<std::string,_testSMGNode> mgraph;
      _testSMGNode node;

      //1. Test adding a node (l1)
      node.name_ = "l1";
      node.gr_parent_names_.push_back("root");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l1";  }

      //2. Test adding the same node again (should fail)
      if(NULL!=mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Added Duplicate Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Did not add Duplicate Node";  }

      //3. Test adding a root node
      node.name_ = "root";
      node.gr_parent_names_.clear();
      node.gr_parent_names_.push_back("ground");
      if(NULL==mgraph.create(node.name_,node,true))
      { throw(std::runtime_error("Add Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Root Node";  }


      //4. Test adding another root node (should fail)
      node.name_ = "bad_root";
      node.gr_parent_names_.clear();
      node.gr_parent_names_.push_back("ground");
      if(NULL!=mgraph.create(node.name_,node,true))
      { throw(std::runtime_error("Added Duplicate Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Rejected Duplicate Root Node";  }

      //5. Add some more nodes. Then build the graph and verify it
      node.name_ = "l2";
      node.gr_parent_names_.clear();
      node.gr_parent_names_.push_back("l1");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l2 (parent=l1)";  }

      node.name_ = "l3";
      node.gr_parent_names_.clear();
      node.gr_parent_names_.push_back("l1");
      node.gr_parent_names_.push_back("r1");
      node.gr_parent_names_.push_back("r2");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l3 (parent=l1)";  }

      node.name_ = "r1";
      node.gr_parent_names_.clear();
      node.gr_parent_names_.push_back("root");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r1 (parent=root)";  }

      node.name_ = "r2";
      node.gr_parent_names_.clear();
      node.gr_parent_names_.push_back("r1");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r2 (parent=r1)";  }

      if(false==mgraph.linkNodes())
      { throw(std::runtime_error("Could not link nodes into a \'branching representation\' graph : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Linked nodes into a \'branching representation\' graph";  }

      //Print graph:
      sutil::CMappedDirGraph<std::string,_testSMGNode>::iterator it, ite;
      std::cout<<"\nTest Result ("<<test_id++<<") : Printing graph :";
      for(it = mgraph.begin(), ite = mgraph.end();
          it!=ite; ++it)
      {
        printf("\n\tNode: %s.\tChildren:",it->name_.c_str());

        _testSMGNode& tmp_node = *it;
        std::vector<_testSMGNode*>::const_iterator it2, ite2;
        ite2 = tmp_node.gr_child_addrs_.end();
        for(it2 = tmp_node.gr_child_addrs_.begin();it2!=ite2;++it2)
        { printf(" %s",(*it2)->name_.c_str()); }

        printf("\tParents:");
        if(&tmp_node == mgraph.getRootNodeConst())
        { printf(" none (I am root)"); continue; }
        ite2 = tmp_node.gr_parent_addrs_.end();
        for(it2 = tmp_node.gr_parent_addrs_.begin();it2!=ite2;++it2)
        {
          if(NULL==*it2)
          { throw(std::runtime_error(std::string("NULL Parent at node: ")+it->name_)); }
          printf(" %s",(*it2)->name_.c_str());
        }
      }

      //Print spanning tree:
      std::cout<<"\nTest Result ("<<test_id++<<") : Printing spanning tree for graph :";
      //Print tree:
      for(it = mgraph.begin(), ite = mgraph.end();
          it!=ite; ++it)
      {
        printf("\n\tNode: %s. Parent %s. Children:",it->name_.c_str(), it->parent_name_.c_str());

        _testSMGNode& tmp_node = *it;
        std::vector<_testSMGNode*>::const_iterator it2, ite2;
        ite2 = tmp_node.child_addrs_.end();
        for(it2 = tmp_node.child_addrs_.begin();it2!=ite2;++it2)
        { printf(" %s",(*it2)->name_.c_str()); }
      }

      //Print broken edges in the spanning tree:
      std::cout<<"\nTest Result ("<<test_id++<<") : Printing edges broken by the spanning tree for graph :";
      //Print broken edges:
      std::vector<std::pair<_testSMGNode*, _testSMGNode*> >::iterator itb, itbe;
      for(itb = mgraph.st_broken_edges_.begin(), itbe = mgraph.st_broken_edges_.end();
          itb!=itbe; ++itb)
      { printf("\n\tEdge: Parent %s -> Child: %s",itb->first->name_.c_str(), itb->second->name_.c_str()); }

      //7. Test Map (Idx and name of pointed object should match)
      _testSMGNode* tmp_mapped_node = mgraph.at("l1");

      if(tmp_mapped_node->name_ != "l1")
      { throw(std::runtime_error("String-AddressPointer map is incorrect Node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : String-AddressPointer map verified for Node l1";  }

      //8. Test ancestor and descendant code
      if( false == mgraph.isAncestor(mgraph.at("l2"),mgraph.at("l1")) )
      { throw(std::runtime_error("Node l1 is not the ancestor of node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l1 ancestor verified for node l2";  }

      if( false == mgraph.isAncestor(mgraph.at("l2"),mgraph.at("root")) )
      { throw(std::runtime_error("Root is not the ancestor of node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Root ancestor verified for node l2";  }

      if( false == mgraph.isAncestor(mgraph.at("l3"),mgraph.at("root")) )
      { throw(std::runtime_error("Root is not the ancestor of node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Root ancestor verified for node l3";  }

      if( false == mgraph.isAncestor(mgraph.at("r2"),mgraph.at("root")) )
      { throw(std::runtime_error("Root is not the ancestor of node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Root ancestor verified for node r2";  }

      if( true == mgraph.isAncestor(mgraph.at("r2"),mgraph.at("l1")) )
      { throw(std::runtime_error("Node l1 reported to be the ancestor of node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l1 is not the ancestor of node r2";  }

      //8. Test descendant code
      if( false == mgraph.isDescendant(mgraph.at("l1"),mgraph.at("l2")) )
      { throw(std::runtime_error("Node l2 is not the descendant of node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l2 descendant verified for node l1";  }

      if( false == mgraph.isDescendant("l1","l2") )
      { throw(std::runtime_error("Index : Node l2 is not the descendant of node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Index : Node l2 descendant verified for node l1";  }

      if( false == mgraph.isDescendant(mgraph.at("root"),mgraph.at("l2")) )
      { throw(std::runtime_error("Node l2 is not the descendant of node root : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l2 descendant verified for node root";  }

      if( false == mgraph.isDescendant(mgraph.at("root"),mgraph.at("l3")) )
      { throw(std::runtime_error("Node l3 is not the descendant of node root : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l3 descendant verified for node root";  }

      if( false == mgraph.isDescendant(mgraph.at("root"),mgraph.at("r2")) )
      { throw(std::runtime_error("Node r2 is not the descendant of node root : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node r2 descendant verified for node root";  }

      if( true == mgraph.isDescendant(mgraph.at("l1"),mgraph.at("r2")) )
      { throw(std::runtime_error("Node r2 reported to be the descendant of node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node r2 is not the descendant of node l1";  }

      std::cout<<"\nTest #"<<arg_id<<" (Mapped Graph Test) Succeeded.";
    }
    catch (std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") : "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Mapped Graph Test) Failed.";
    }
  }

}

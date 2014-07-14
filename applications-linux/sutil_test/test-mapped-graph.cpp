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
/* \file test-mapped-graph.cpp
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#include "test-mapped-graph.hpp"

#include <sutil/CMappedGraph.hpp>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <stdio.h>

namespace sutil_test
{
  /**
   * Test Structure - Mapped graph Node
   */
  struct _testSMGNode
  {
  public:
    std::string name_;
    std::vector<std::string> parent_names_;
    std::vector<_testSMGNode*> parent_addrs_;
    std::vector<_testSMGNode*> child_addrs_;

    /**
     * Init/Reset the object
     */
    void init()
    {
      name_ = "";
      parent_names_.clear();
      parent_addrs_.clear();
      child_addrs_.clear();
    }
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
      sutil::CMappedGraph<std::string,_testSMGNode> mgraph;
      _testSMGNode node;

      //1. Test adding a node (l1)
      node.name_ = "l1";
      node.parent_names_.push_back("root");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l1";  }

      //2. Test adding the same node again (should fail)
      if(NULL!=mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Added Duplicate Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Did not add Duplicate Node";  }

      //3. Test adding a root node
      node.name_ = "root";
      node.parent_names_.clear();
      node.parent_names_.push_back("ground");
      if(NULL==mgraph.create(node.name_,node,true))
      { throw(std::runtime_error("Add Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Root Node";  }


      //4. Test adding another root node (should fail)
      node.name_ = "bad_root";
      node.parent_names_.clear();
      node.parent_names_.push_back("ground");
      if(NULL!=mgraph.create(node.name_,node,true))
      { throw(std::runtime_error("Added Duplicate Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Rejected Duplicate Root Node";  }

      //5. Add some more nodes. Then build the graph and verify it
      node.name_ = "l2";
      node.parent_names_.clear();
      node.parent_names_.push_back("l1");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l2 (parent=l1)";  }

      node.name_ = "l3";
      node.parent_names_.clear();
      node.parent_names_.push_back("l1");
      node.parent_names_.push_back("r1");
      node.parent_names_.push_back("r2");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l3 (parent=l1)";  }

      node.name_ = "r1";
      node.parent_names_.clear();
      node.parent_names_.push_back("root");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r1 (parent=root)";  }

      node.name_ = "r2";
      node.parent_names_.clear();
      node.parent_names_.push_back("r1");
      if(NULL==mgraph.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r2 (parent=r1)";  }

      if(false==mgraph.linkNodes())
      { throw(std::runtime_error("Could not link nodes into a \'banching representation\' graph : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Linked nodes into a \'banching representation\' graph";  }

      //Print graph:
      sutil::CMappedGraph<std::string,_testSMGNode>::iterator it, ite;
      std::cout<<"\nTest Result ("<<test_id++<<") : Printing graph :";
      for(it = mgraph.begin(), ite = mgraph.end();
          it!=ite; ++it)
      {
        printf("\n\tNode: %s.\tChildren:",it->name_.c_str());

        _testSMGNode& tmp_node = *it;
        std::vector<_testSMGNode*>::const_iterator it2, ite2;
        ite2 = tmp_node.child_addrs_.end();
        for(it2 = tmp_node.child_addrs_.begin();it2!=ite2;++it2)
        { printf(" %s",(*it2)->name_.c_str()); }

        printf("\tParents:");
        if(&tmp_node == mgraph.getRootNodeConst())
        { printf(" none (I am root)"); continue; }
        ite2 = tmp_node.parent_addrs_.end();
        for(it2 = tmp_node.parent_addrs_.begin();it2!=ite2;++it2)
        {
          if(NULL==*it2)
          { throw(std::runtime_error(std::string("NULL Parent at node: ")+it->name_)); }
          printf(" %s",(*it2)->name_.c_str());
        }
      }

      //7. Test Map (Idx and name of pointed object should match)
      _testSMGNode* tmp_mapped_node = mgraph.at("l1");

      if(tmp_mapped_node->name_ != "l1")
      { throw(std::runtime_error("String-AddressPointer map is incorrect Node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : String-AddressPointer map verified for Node l1";  }

      std::cout<<"\nTest #"<<arg_id<<" (Mapped Graph Test) Succeeded.";
    }
    catch (std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") : "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Mapped Graph Test) Failed.";
    }
  }

}

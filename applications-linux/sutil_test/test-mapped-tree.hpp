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
/* \file test-mapped-tree.hpp
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_BRANCHING_STRUCT_HPP_
#define TEST_BRANCHING_STRUCT_HPP_

#include <sutil/CMappedTree.hpp>

#include <string>
#include <stdexcept>
#include <vector>
#include <stdio.h>

namespace sutil_test
{

  /**
   * Test Structure - Mapped tree Node
   */
  struct _testSMTNode
  {
  public:
    std::string name_;
    std::string parent_name_;
    _testSMTNode* parent_addr_;
    std::vector<_testSMTNode*> child_addrs_;

    /**
     * Init/Reset the object
     */
    void init()
    {
      name_ = "";
      parent_name_ = "";
      parent_addr_ = NULL;
      child_addrs_.clear();
    }
  };

  /**
   * Tests the mapped tree with the tree:
   *            ground (not a link)
   *              --
   *             root
   *            /    \
   *           l1    r1
   *          /  \     \
   *         l2  l3     r2
   */
  void test_mapped_tree(int arg_id)
  {
    unsigned int test_id=0;
    try
    {
      //0. Create a br struct
      sutil::CMappedTree<std::string,_testSMTNode> br;
      _testSMTNode node;

      //1. Test adding a node (l1)
      node.name_ = "l1";
      node.parent_name_ = "root";
      if(NULL==br.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l1";  }

      //2. Test adding the same node again (should fail)
      if(NULL!=br.create(node.name_,node,false))
      { throw(std::runtime_error("Added Duplicate Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Did not add Duplicate Node";  }

      //3. Test adding a root node
      node.name_ = "root";
      node.parent_name_ = "ground";
      if(NULL==br.create(node.name_,node,true))
      { throw(std::runtime_error("Add Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Root Node";  }


      //4. Test adding another root node (should fail)
      node.name_ = "bad_root";
      node.parent_name_ = "ground";
      if(NULL!=br.create(node.name_,node,true))
      { throw(std::runtime_error("Added Duplicate Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Rejected Duplicate Root Node";  }

      //5. Add some more nodes. Then build the tree and verify it
      node.name_ = "l2";
      node.parent_name_ = "l1";
      if(NULL==br.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l2 (parent=l1)";  }

      node.name_ = "l3";
      node.parent_name_ = "l1";
      if(NULL==br.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l3 (parent=l1)";  }

      node.name_ = "r1";
      node.parent_name_ = "root";
      if(NULL==br.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r1 (parent=root)";  }

      node.name_ = "r2";
      node.parent_name_ = "r1";
      if(NULL==br.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r2 (parent=r1)";  }

      if(false==br.linkNodes())
      { throw(std::runtime_error("Could not link nodes into a \'banching representation\' tree : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Linked nodes into a \'banching representation\' tree";  }

      //Print tree:
      br.resetIterator();
      std::cout<<"\nTest Result ("<<test_id++<<") : Printing tree :";
      while(br.iterator_!=NULL)
      {
        _testSMTNode* tmp_node = br.iterator_->data_;
        printf("\n\tNode: %s. Children:",tmp_node->name_.c_str());

        std::vector<_testSMTNode*>::const_iterator it2, ite2;
        ite2 = tmp_node->child_addrs_.end();
        for(it2 = tmp_node->child_addrs_.begin();it2!=ite2;++it2)
        {
          printf(" %s",(*it2)->name_.c_str());
        }

        br.iterator_ = br.iterator_->next_;
      }

      //7. Test Map (Idx and name of pointed object should match)
      _testSMTNode* tmp_mapped_node = br.at("l1");

      if(tmp_mapped_node->name_ != "l1")
      { throw(std::runtime_error("String-AddressPointer map is incorrect Node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : String-AddressPointer map verified for Node l1";  }

      std::cout<<"\nTest #"<<arg_id<<" (Mapped Tree Test) Succeeded.";
  }
  catch (std::exception& ee)
  {
    std::cout<<"\nTest Error ("<<test_id++<<") : "<<ee.what();
    std::cout<<"\nTest #"<<arg_id<<" (Mapped Tree Test) Failed.";
  }
}

}

#endif /* TEST_BRANCHING_STRUCT_HPP_ */

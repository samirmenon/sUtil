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
/* \file test-mapped-tree.cpp
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#include "test-mapped-tree.hpp"

#include <sutil/CMappedTree.hpp>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <stdio.h>

namespace sutil_test
{
  /** Test Structure - Mapped tree Node */
  struct _testSMTNode : public sutil::CMappedTree<std::string,_testSMTNode>::SMTNodeBase
  {
  public:
    int random_data_;
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
      //0. Create a mapped tree
      sutil::CMappedTree<std::string,_testSMTNode> mtree;
      _testSMTNode node;

      //1. Test adding a node (l1)
      node.name_ = "l1";
      node.parent_name_ = "root";
      if(NULL==mtree.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l1";  }

      //1.b. Link nodes should fail.
      if(true==mtree.linkNodes())
      { throw(std::runtime_error("Linked nodes without a root")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Did not link nodes since root is missing";  }

      //2. Test adding the same node again (should fail)
      if(NULL!=mtree.create(node.name_,node,false))
      { throw(std::runtime_error("Added Duplicate Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Did not add Duplicate Node";  }

      //3. Test adding a root node
      node.name_ = "root";
      node.parent_name_ = "ground";
      if(NULL==mtree.create(node.name_,node,true))
      { throw(std::runtime_error("Add Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Root Node";  }


      //4. Test adding another root node (should fail)
      node.name_ = "bad_root";
      node.parent_name_ = "ground";
      if(NULL!=mtree.create(node.name_,node,true))
      { throw(std::runtime_error("Added Duplicate Root Node : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Rejected Duplicate Root Node";  }

      //5. Add some more nodes. Then build the tree and verify it
      node.name_ = "l2";
      node.parent_name_ = "l1";
      if(NULL==mtree.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l2 (parent=l1)";  }

      node.name_ = "l3";
      node.parent_name_ = "l1";
      if(NULL==mtree.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node l3 (parent=l1)";  }

      node.name_ = "r1";
      node.parent_name_ = "root";
      if(NULL==mtree.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r1 (parent=root)";  }

      node.name_ = "r2";
      node.parent_name_ = "r1";
      if(NULL==mtree.create(node.name_,node,false))
      { throw(std::runtime_error("Add Node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Added Node r2 (parent=r1)";  }

      if(false==mtree.linkNodes())
      { throw(std::runtime_error("Could not link nodes into a \'banching representation\' tree : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Linked nodes into a \'banching representation\' tree";  }

      //Print tree:
      sutil::CMappedTree<std::string,_testSMTNode>::iterator it, ite;
      std::cout<<"\nTest Result ("<<test_id++<<") : Printing tree :";
      for(it = mtree.begin(), ite = mtree.end();
          it!=ite; ++it)
      {
        printf("\n\tNode: %s. Children:",it->name_.c_str());

        _testSMTNode& tmp_node = *it;
        std::vector<_testSMTNode*>::const_iterator it2, ite2;
        ite2 = tmp_node.child_addrs_.end();
        for(it2 = tmp_node.child_addrs_.begin();it2!=ite2;++it2)
        { printf(" %s",(*it2)->name_.c_str()); }
      }

      //7. Test Map (Idx and name of pointed object should match)
      _testSMTNode* tmp_mapped_node = mtree.at("l1");

      if(tmp_mapped_node->name_ != "l1")
      { throw(std::runtime_error("String-AddressPointer map is incorrect Node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : String-AddressPointer map verified for Node l1";  }

      //8. Test ancestor code
      if( false == mtree.isAncestor(mtree.at("l3"),mtree.at("l1")) )
      { throw(std::runtime_error("Node l1 is not the ancestor of node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l1 ancestor verified for node l3";  }

      if( false == mtree.isAncestor("l3","l1") )
      { throw(std::runtime_error("Index : Node l1 is not the ancestor of node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Index : Node l1 ancestor verified for node l3";  }

      if( false == mtree.isAncestor(mtree.at("l2"),mtree.at("root")) )
      { throw(std::runtime_error("Root is not the ancestor of node l2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Root ancestor verified for node l2";  }

      if( false == mtree.isAncestor(mtree.at("l3"),mtree.at("root")) )
      { throw(std::runtime_error("Root is not the ancestor of node l3 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Root ancestor verified for node l3";  }

      if( false == mtree.isAncestor(mtree.at("r2"),mtree.at("root")) )
      { throw(std::runtime_error("Root is not the ancestor of node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Root ancestor verified for node r2";  }

      if( true == mtree.isAncestor(mtree.at("r2"),mtree.at("l1")) )
      { throw(std::runtime_error("Node l1 reported to be the ancestor of node r2 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l1 is not the ancestor of node r2";  }

      //8. Test descendant code
      if( false == mtree.isDescendant(mtree.at("l1"),mtree.at("l3")) )
      { throw(std::runtime_error("Node l3 is not the descendant of node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l3 descendant verified for node l1";  }

      if( false == mtree.isDescendant("l1","l3") )
      { throw(std::runtime_error("Index : Node l3 is not the descendant of node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Index : Node l3 descendant verified for node l1";  }

      if( false == mtree.isDescendant(mtree.at("root"),mtree.at("l2")) )
      { throw(std::runtime_error("Node l2 is not the descendant of node root : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l2 descendant verified for node root";  }

      if( false == mtree.isDescendant(mtree.at("root"),mtree.at("l3")) )
      { throw(std::runtime_error("Node l3 is not the descendant of node root : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node l3 descendant verified for node root";  }

      if( false == mtree.isDescendant(mtree.at("root"),mtree.at("r2")) )
      { throw(std::runtime_error("Node r2 is not the descendant of node root : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node r2 descendant verified for node root";  }

      if( true == mtree.isDescendant(mtree.at("l1"),mtree.at("r2")) )
      { throw(std::runtime_error("Node r2 reported to be the descendant of node l1 : Failed")); }
      else { std::cout<<"\nTest Result ("<<test_id++<<") : Node r2 is not the descendant of node l1";  }

      std::cout<<"\nTest #"<<arg_id<<" (Mapped Tree Test) Succeeded.";
    }
    catch (std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") : "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Mapped Tree Test) Failed.";
    }
  }

}

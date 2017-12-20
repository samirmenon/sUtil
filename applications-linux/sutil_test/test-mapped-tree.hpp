/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file test-mapped-tree.hpp
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_MAPPED_TREE_HPP_
#define TEST_MAPPED_TREE_HPP_


namespace sutil_test
{
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
  void test_mapped_tree(int arg_id);
}

#endif /* TEST_MAPPED_TREE_HPP_ */

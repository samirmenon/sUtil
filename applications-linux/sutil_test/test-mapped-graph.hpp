/* This file is part of sUtil, a random collection of utilities.

See the Readme.txt file in the root folder for licensing information.
 */
/* \file test-mapped-tree.hpp
 *
 *  Copyright (C) 2010
 *
 *  Author: Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_MAPPED_GRAPH_HPP_
#define TEST_MAPPED_GRAPH_HPP_


namespace sutil_test
{
  /**
   * Tests the mapped graph with the graph:
   *            ground (not a link)
   *              --
   *             root
   *            /    \
   *           l1    r1
   *          /  \  /  \
   *         l2   l3---r2
   */
  void test_mapped_graph(int arg_id);
}

#endif /* TEST_MAPPED_GRAPH_HPP_ */

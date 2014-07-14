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

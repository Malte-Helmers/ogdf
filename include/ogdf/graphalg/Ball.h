/** \file
 * \brief Declaration of a naive algorithm to calculate a Ball; ogdf::Ball .
 *
 * \author Malte Helmers
 *
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 *
 * \par
 * Copyright (C)<br>
 * See README.md in the OGDF root directory for details.
 *
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation;
 * see the file LICENSE.txt included in the packaging of this file
 * for details.
 *
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * \par
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see
 * http://www.gnu.org/copyleft/gpl.html
 */

#pragma once

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
/**
 * Constructs a vertex induced subgraph (or Ball) of a graph by defining a center node #m_u and an edge radius #m_p.
 *
 * The Ball only consists of nodes v with dist(#m_u, v) <= #m_p .
 *
 * The original Graph without the Ball's edges decomposes into multiple components that are put into a Component-List.
 * 
 * If the node u is a #m_p-center the amount of vertices of each component are at most half the amount of vertices of the original graph.
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT Ball {
public :

	//! Creates an instance of Ball.
	Ball() {	}

	//! Destructor.
	~Ball() = default;

	bool findRhoCenter(const Graph& G, NodeArray<int>& Components) const;

	bool isHub(const Graph& G, Array<NodeArray<bool>>& Clusters, NodeArray<int>& Components) const ;
};

class OGDF_EXPORT DistanceMetric {
public:

	DistanceMetric(const Graph& G) ;

protected:
	NodeArray<NodeArray<int>> m_distMatrix;
	NodeArray<NodeArray<node>> m_predMatrix;

	NodeArray<ArrayBuffer<ArrayBuffer<node>>> m_distMap;
	Array<ArrayBuffer<node>> m_maxDistMap;

	ArrayBuffer<ArrayBuffer<node>> m_nodesByMaxDist;

	int better_bfs_SPAP
	(
		const Graph& G, 
		NodeArray<Array<SListPure<node>>>& distance, 
		NodeArray<NodeArray<node>>& predecessors, 
		Array<SListPure<node>>& sortedNodes
	) const ;

	void better_bfs_SPSS
	(
		node s, 
		const Graph& G, 
		Array<SListPure<node>>& distanceArray, // Saves a queue of nodes for each distance corresponding to the position in matrix
		NodeArray<node>& predecessorArray // Saves the predecessor in Path from s to node with id corresponding to position in matrix
	) const ;
};

class OGDF_EXPORT ConnectivityPartition {
public:


};

class OGDF_EXPORT ComponentSet {
public:

	int constructComponents
	(
		const Graph& G, 
		NodeArray<bool>& ball, 
		node u, 
		int rho,
		Array<Graph> compArray,
		NodeArray<int> compIndices,
		Array<NodeArray<node>> nodeMap
	) const ;
};

}

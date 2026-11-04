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
#include <ogdf/basic/GraphSets.h>
//#include <ogdf/basic/SortedSequence.h>

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
	Ball() {}

	Ball(const Graph& G, const DistanceMetric& delta, int rho) {this->findRhoCenter(G, delta, rho);}

	Ball(const Graph& G, const DistanceMetric& delta, int rho, node u) {this->init(G, delta, rho, u);}


	//! Destructor.
	~Ball() = default;

	bool findRhoCenter(const Graph& G, const DistanceMetric& delta, int rho) ;

	void init(const Graph& G, const DistanceMetric& delta, int rho, node u) ;

	NodeArray<bool> internalNodes;
	//EdgeArray<bool> internalEdges; //kinda redundant right now
	EdgeSet intEdgeSet;

	ComponentSet decomposedGraph;

	node rhoCenter;

protected:

	bool validRhoCenter(const Graph& G) const ;

};

class OGDF_EXPORT DistanceMetric {
public:
	DistanceMetric() {}

	DistanceMetric(const Graph& G) {this->init(G);}

	void init(const Graph& G) ;

	//NodeArray<NodeArray<int>> distances;
	NodeArray<NodeArray<node>> predecessors;

	NodeArray<ArrayBuffer<ArrayBuffer<node>>> distanceMapArray;
	ArrayBuffer<ArrayBuffer<node>> maxDistanceMap;

	Array<node> nodesByMaxDistance;

	int maxDistance;

protected:

	int better_bfs_SPAP
	(
		const Graph& G, 
		NodeArray<NodeArray<node>>& predMatrix, 
		NodeArray<ArrayBuffer<ArrayBuffer<node>>>& distMapArr, 
		ArrayBuffer<ArrayBuffer<node>>& maxDistMap
	) const ;

	void better_bfs_SPSS
	(
		node s, 
		const Graph& G, 
		NodeArray<node>& predArr, // Saves the predecessor in Path from s to node with id corresponding to position in matrix
		ArrayBuffer<ArrayBuffer<node>>& distMap // Saves a queue of nodes for each distance corresponding to the position in matrix
	) const ;
};

class OGDF_EXPORT ComponentSet {
public:
	ComponentSet() {}

	ComponentSet(const Graph& G, EdgeSet removedEdges) {this->init(G, removedEdges);}

	void init(const Graph& G, EdgeSet removedEdges) ;

	NodeArray<int> compIndices;
	Array<Graph> components;

	Array<NodeArray<node>> nodeMap;

	int compAmount;

};

class OGDF_EXPORT ConnectivityPartition {
public:
	ConnectivityPartition() {}

	ConnectivityPartition(const Graph& G) {this->init(G);}


	void init(const Graph& G) ;


	bool merge(edge e) ;

	bool isHub(const Graph& G, ComponentSet& Components, node kernel) const ;

	NodeArray<int> clusterIndices;
	ArrayBuffer<NodeSet> clusters;

};

}

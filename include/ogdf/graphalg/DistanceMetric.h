/** \file
 * \brief Declaration of a simple Distance Metric Structure. Heavily inspired by the bfs-searches in ShortestPathAlgorithms.h .
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

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {


/**
 * Constructs 5 variables related to the distances in a given weightless, undirected and connected Graph.
 * 
 * A NodeMatrix containing the Predecessors to the target node for a shortest path for each source node.
 * 
 * A Map-NodeArray containing the target nodes for a shortest path categorized by distance for each source node.
 * 
 * A Map containing the source nodes categorized by their max-distance.
 * 
 * An Array containg the source nodes sorted by their max-distance.
 * 
 * And the max-distance across the Graph
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT DistanceMetric {
public:

	//! Creates a DistanceMetric
	DistanceMetric() = default;

	/**
	 * Creates and constructs the DistanceMetric
	 * \param G The Graph to determine the distances of
	 */
	DistanceMetric(const Graph& G) {this->init(G);}

	/**
	 * Re-Constructs the DistanceMetric
	 * \param G The Graph to determine the distances of
	 */
	void init(const Graph& G) ;

	NodeArray<NodeArray<node>> predecessors; //!< A NodeMatrix containing the Predecessors to the target node for a shortest path for each source node.

	NodeArray<ArrayBuffer<ArrayBuffer<node>>> distanceMapArray; //!< A Map-NodeArray containing the target nodes for a shortest path categorized by distance for each source node.
	ArrayBuffer<ArrayBuffer<node>> maxDistanceMap; //!< A Map containing the source nodes categorized by their max-distance.

	Array<node> nodesByMaxDistance; //!< An Array containg the source nodes sorted by their max-distance.

	int maxDistance; //!< The max-distance across the Graph

protected:

	/**
	 * Does essentially the same as bfs_SPAP from ShortestPathAlgorithms.h, but saves the distances differently
	 * \param G The Graph to determine the distances of
	 * \param predMatrix Is Assigned the predecessor NodeMatrix
	 * \param distMapArr Is Assigned the Map-NodeArray containing the target nodes for each distance for each source node.
	 * \param maxDistMap Is Assigned the Map containing the node for each max distance.
	 */
	int better_bfs_SPAP
	(
		const Graph& G, 
		NodeArray<NodeArray<node>>& predMatrix, 
		NodeArray<ArrayBuffer<ArrayBuffer<node>>>& distMapArr, 
		ArrayBuffer<ArrayBuffer<node>>& maxDistMap
	) const ;

	/**
	 * Does essentially the same as bfs_SPSS from ShortestPathAlgorithms.h, but saves the distances differently
	 * \param s The source node to start bfs from
	 * \param G The Graph to determine the distances of
	 * \param predMatrix Is Assigned the predecessor NodeArray
	 * \param distMapArr Is Assigned the Map containing the target node for each distance.
	 */
	void better_bfs_SPSS
	(
		node s, 
		const Graph& G, 
		NodeArray<node>& predArr, // Saves the predecessor in Path from s to node with id corresponding to position in matrix
		ArrayBuffer<ArrayBuffer<node>>& distMap // Saves a queue of nodes for each distance corresponding to the position in matrix
	) const ;

public:

	/**
	 * Prints: The Graph's Max-Distance, and for each Source-Node's Max Distance: the Source Node,
	 * the target Nodes in each of the source node's distances and their predecessor.
	 * 
	 * Also prints a list of the source nodes sorted by max distance.
	 */
	void print() ;
};

}

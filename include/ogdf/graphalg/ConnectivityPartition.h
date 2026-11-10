/** \file
 * \brief Declaration of a simple Connectivity Partition.
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
#include <ogdf/graphalg/ComponentSet.h>

#include <ogdf/basic/DisjointSets.h>


#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

/**
 * Constructs a Connectivity Partition for the Nodes of a given Graph, it resembles the connectivity of a spanning Forest on that Graph.
 * 
 * It will at first have a cluster for each Node, therefore resembling an edgeless spanning Forest.
 * 
 * When merged along a certain edge, the 2 clusters connected along the edge will become one Cluster.
 * 
 * This can be repeated for every Edge until the Clusters resemble the Connected Components of the given Graph.
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT ConnectivityPartition {
public:

	//! Creates a ConnectivityPartition
	ConnectivityPartition() = default;

	/**
	 * Creates a Connectivity Partition and creates a Cluster for each Node in G
	 * \param G The Graph to construct an empty Connectivity Partition for
	 */
	ConnectivityPartition(const Graph& G) {this->init(G);}


	/**
	 * Re-Initializes the Connectivity Partition, clearing it and creating a Cluster for each Node in G
	 * \param G The Graph to construct an empty Connectivity Partition for
	 */
	void init(const Graph& G) ;

	/**
	 * Merges the Clusters connected by the given Edge
	 * \param e The edge in the original Graph along which to merge two Clusters
	 * \returns false if the two clusters to merge are the same, true otherwise
	 */
	bool merge(edge e) ;

	/**
	 * Checks whether the Connectivity Partition is a Hub on G regarding \p Components, 
	 * 
	 * That means whether its non-kernel Clusters are each part of only one Component instead of being divided among multiple
	 * \param G The Graph to check for (needs to be the same Graph used for Initialization) To Do: maybe remove and get through clusterIndices.graphof()
	 * \param Components The Array of Components with references to G to check
	 * \param kernel a Node in the Kernel-Cluster
	 * \returns true if the Conncetivity Partition is Hub on G regarding \p Components, false otherwise
	 */
	bool isHub(const Graph& G, ComponentSet& Components, node kernel) const ;

	/**
	 * \param v The Node for which the Cluster's index should be returned
	 * \returns the index of the Cluster containing the given Node
	 */
	int getIndex(node v) ;

protected:
	NodeArray<int> clusterIndices; //!< NodeArray saving each node's original Cluster's index
	DisjointSets<LinkOptions::Index, CompressionOptions::PathSplitting, InterleavingOptions::Disabled> clusters; //!< DisjointSets storing the Clusters

public:
	/**
	 * Prints the Connectivity Partition's Amount of Clusters and the index of the current Cluster for each Node in the associated Graph
	 */
	void print() ;

};

}

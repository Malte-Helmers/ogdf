/** \file
 * \brief Implementation of a simple Connectivity Partition.
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

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphCopy.h>
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/GraphSets.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
#include <ogdf/basic/simple_graph_alg.h>

#include <ogdf/planarity/embedder/ConnectedSubgraph.h>

#include <ogdf/graphalg/ShortestPathAlgorithms.h>
#include <ogdf/graphalg/ConnectivityPartition.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

void ConnectivityPartition::init(const Graph& G) {
	clusterIndices = NodeArray<int>(G, -1);
	clusters = ArrayBuffer<NodeSet>(G.numberOfNodes(),false);

	for (int i = 0; i < G.numberOfNodes();i++) { //to do maybe optimise, idk.
		clusters.push(NodeSet(G));
	}

	for (node v : G.nodes) {
		clusterIndices[v] = v->index();

		clusters[v->index()].clear();
		clusters[v->index()].insert(v);
	}
}

bool ConnectivityPartition::merge(edge e) {
	int sourceIndex = clusterIndices[e->source()];
	int targetIndex = clusterIndices[e->target()];

	//std::cout << "Merge for Edge " << e->source()->index() << "-" << e->target()->index() << ", with Clusters " << sourceIndex << "-" << targetIndex << std::endl;

	if (sourceIndex == targetIndex) {
		return false;
	}

	for (node v : clusterIndices.graphOf()->nodes) { // to do: optimise
		if (clusterIndices[v] == targetIndex) {
			clusterIndices[v] = sourceIndex;
		}
		if (clusterIndices[v] > targetIndex) {
			clusterIndices[v] -= 1;
		}
		
	}

	for (node v : clusters[targetIndex]) {
		clusters[sourceIndex].insert(v);
	}

	//to do: maybe have clusters organized by distance to rho-center, then this step'd be easier, but it'd need to be reSorted for each rho-center.
	ArrayBuffer<int> removedCluster(1);
	removedCluster.push(targetIndex);
	clusters.leftShift(removedCluster);

	return true;
}

bool ConnectivityPartition::isHub(const Graph& G, ComponentSet& Components, node kernel) const {
	for (int i = 0; i < clusters.size(); i++) {
		if (!(i == clusterIndices[kernel])) {
			int compNumber = -1;

			for (node v : clusters[i]) {
				if (compNumber != Components.compIndices[v]) {
					if (compNumber > -1) {
						return false;
					}
					compNumber = Components.compIndices[v];
				}
			}
		}
	}

	return true;
}

void ConnectivityPartition::print() {
#ifdef OGDF_DEBUG
	std::cout << "Connectivity Partition with " << clusters.size() << " Clusters:" << std::endl;
	for (int i = 0; i < clusters.size(); i++) {
		std::cout << "Cluster " << i << "/" << clusters.size()-1 << ", with size " << clusters[i].size() << std::endl;
			for (node v : clusters[i]) {
				std::cout << "Node " << v->index() << " with ClusterIndex " << clusterIndices[v] << std::endl;
			}
	}
#endif
}

}

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
#include <ogdf/basic/DisjointSets.h>


#include <ogdf/planarity/embedder/ConnectedSubgraph.h>

#include <ogdf/graphalg/ShortestPathAlgorithms.h>
#include <ogdf/graphalg/ConnectivityPartition.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

void ConnectivityPartition::init(const Graph& G) {
	clusters = DisjointSets(G.numberOfNodes());
	clusterIndices = NodeArray<int>(G, -1);

	for (node v : G.nodes) {
		clusterIndices[v] = clusters.makeSet();
	}
}

bool ConnectivityPartition::merge(edge e) {
	int sourceIndex = clusters.getRepresentative(clusterIndices[e->source()]);
	int targetIndex = clusters.getRepresentative(clusterIndices[e->target()]);

#ifdef OGDF_DEBUG
	//std::cout << "Merge P along Edge " << e->source()->index() << "-" << e->target()->index() << ", with Clusters " << sourceIndex << "-" << targetIndex << std::endl;
#endif

	return clusters.quickUnion(sourceIndex,targetIndex);
}

bool ConnectivityPartition::isHub(const Graph& G, ComponentSet& Components, node kernel) const {
	Array<int> ClusterToCompMap(0, G.numberOfNodes(), -1); // to do: fix by using other templateparams, also maybe use find instead of get representative.
	for (node v : G.nodes) {
		int curInd = clusters.getRepresentative(clusterIndices[v]);
		if (curInd != clusters.getRepresentative(clusterIndices[kernel])) {
			if (ClusterToCompMap[curInd] != Components.compIndices[v]) {
					if (ClusterToCompMap[curInd] > -1) {
						return false;
					}
					ClusterToCompMap[curInd] = Components.compIndices[v];
				}
		}
	}

	return true;
}

int ConnectivityPartition::getIndex(node v) {
	return clusters.getRepresentative(clusterIndices[v]);
}


void ConnectivityPartition::print() {
#ifdef OGDF_DEBUG
	std::cout << "Connectivity Partition with " << clusters.getNumberOfSets() << " Clusters:" << std::endl;
	for (node v : clusterIndices.graphOf()->nodes) {
		std::cout << "Node " << v->index() << " has clusterIndex " << clusters.getRepresentative(clusterIndices[v]) << std::endl;
	}
#endif
}

}

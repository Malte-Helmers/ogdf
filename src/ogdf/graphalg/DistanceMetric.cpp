/** \file
 * \brief Implementation of a simple Distance Metric Structure. Heavily inspired by the bfs-searches in ShortestPathAlgorithms.h .
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
#include <ogdf/graphalg/DistanceMetric.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

void DistanceMetric::init(const Graph& G) {

	predecessors.init(G);
	for (node v : G.nodes) {
		predecessors[v].init(G);
	}

	distanceMapArray.init(G);
	for (node v : G.nodes) {
		distanceMapArray[v].init(G.numberOfNodes()); //currently is allocated max size and made not to grow, but if space is an issue, it could instead start small.
		distanceMapArray[v].setGrowable(false);
	}

	maxDistanceMap.init(G.numberOfNodes()); // same optimization possible as above, allthough here we also have the problem of holes within our Array.
	maxDistanceMap.setGrowable(false);

	nodesByMaxDistance.init(G.numberOfNodes());

	maxDistance = better_bfs_SPAP(G, predecessors, distanceMapArray, maxDistanceMap);

	// construct nodesByMaxDistance based on maxDistanceMap. Using it cuts the Runtime of n in best case and 2n in worst case down to a runtime of always n. 
	for (int i = 0, j = 0; i < maxDistanceMap.size() && j < G.numberOfNodes(); i++) {
		for (node v : maxDistanceMap[i]) {
			nodesByMaxDistance[j] = v;
			j++;
		}
	}
}

int DistanceMetric::better_bfs_SPAP(const Graph& G, NodeArray<NodeArray<node>>& predMatrix,
		NodeArray<ArrayBuffer<ArrayBuffer<node>>>& distMapArr, ArrayBuffer<ArrayBuffer<node>>& maxDistMap) const {
	
	int maxDist = 0;
	for (node v : G.nodes) {
		better_bfs_SPSS(v, G, predMatrix[v], distMapArr[v]);


		int max = distMapArr[v].size() - 1;
		if (max > maxDist) {
			maxDist = max;
		}
		for (int i = maxDistMap.size(); i <= max; i++) { // initialize matrices up to max
			maxDistMap.push(ArrayBuffer<node>());
		}
		maxDistMap[max].push(distMapArr[v][0][0]); //to do: maybe make a sorted list instead, 
		//the purpose of SortedNodes is mainly for iterating through rho-centers with smallest max distance to biggest max distance
		// in the end it still has to go through all to exclude all, as a big max distance doesn't necessarily deny a rho-center 
		//(exception being when rho >= max distance of node). Another heuristic could be to count how many nodes are within a certain distance, 
		//but this'd need to be done and sorted for each possible rho, 
		//to make such an algorithm you'd just count the sum of all destination nodes within the smallest distance, 
		//then go to the next and add that ones amount + the previous, and so on. but having to sort it for each rho would still take time.
		//maybe less time than testing for rho-centers though.
	}

	return maxDist;

}

void DistanceMetric::better_bfs_SPSS(node s, const Graph& G, 
		NodeArray<node>& predArr, ArrayBuffer<ArrayBuffer<node>>& distMap) const {
	NodeArray<int> mark(G, 0);
	SListPure<node> bfs;
	bfs.pushBack(s);
	// mark s and set distance to itself 0
	mark[s] = 1;
	distMap.push(ArrayBuffer<node>());
	distMap[0].push(s);
	predArr[s] = s;
	while (!bfs.empty()) {
		node w = bfs.popFrontRet();
		int d = mark[w];
		for (adjEntry adj : w->adjEntries) {
			node v = adj->twinNode();
			if (mark[v] == 0) {
				mark[v] = d + 1;
				bfs.pushBack(v);
				if (d == distMap.size()) { // could be avoided by just using an Array instead, but this way it keeps track of size.
					distMap.push(ArrayBuffer<node>());
				}
				distMap[d].push(v);
				predArr[v] = w;
			}
		}
	}
}

void DistanceMetric::print() {
#ifdef OGDF_DEBUG
	std::cout << "Max Max Distance: " << maxDistance <<"; Shortest Distances in Graph:" << std::endl;
	for (int i = 0; i < maxDistanceMap.size(); i++) {
		std::cout << "Max Distance "<< i << ": {" << std::endl << "	";
		for (node v : maxDistanceMap[i]) {
			std::cout << "Source Node "<< v->index() << ": [" << std::endl << "		";
			for (int j = 0; j < distanceMapArray[v].size(); j++) {
				std::cout << "dest-nodes at dist "<< j << ": { ";
				for (node u : distanceMapArray[v][j]) {
					std::cout << u->index() << " (pre: " << predecessors[v][u]->index() << "), ";
				}
				std::cout << "}" << std::endl << "		";
			}
			std::cout << "]" << std::endl << "	";

		}
		std::cout << "}" << std::endl;
	}

	std::cout << "All Nodes, in order of their Max Distance:" << std::endl;
	for (node v : nodesByMaxDistance) {
		std::cout << "Node "<< v->index() << ", ";
	}
	std::cout << std::endl;
#endif
}


}

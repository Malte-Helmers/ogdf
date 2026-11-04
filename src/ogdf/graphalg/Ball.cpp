/** \file
 * \brief Implementation of a naive algorithm to calculate a Ball; ogdf::Ball .
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
#include <ogdf/graphalg/Ball.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
	
bool Ball::findRhoCenter(const Graph& G, const DistanceMetric& delta, int rho) {

	bool found = false;

	for (int i = 0; i < G.numberOfNodes() && !found; i++) {
		node u = delta.nodesByMaxDistance[i];

		init(G, delta, rho, u);

		if (validRhoCenter(G)) {
			rhoCenter = u;
			found = true;
		}
	}

	return found;
}

void Ball::init(const Graph& G, const DistanceMetric& delta, int rho, node u) {
	internalNodes.init(G, false);
	//internalEdges.init(G, false);

	intEdgeSet.init(G);
	intEdgeSet.clear();

	for (int i = 0; i <= rho && i < G.numberOfNodes(); i++) {
		for (node v : delta.distanceMapArray[u][i]) {
			internalNodes[v] = true;
		}
	}

	for (edge e : G.edges) { // to do: optimize so that it doesn't iterate through every edge for this, may need to change DistanceMetric to track edges for this... Or maybe I can use destnode as source node and search for destnode with dist 1 to find edges, maybe in practise too slow
		if (internalNodes[e->source()] && internalNodes[e->target()]) {
			//internalEdges[e] = true;
			intEdgeSet.insert(e);
		}
	}

	decomposedGraph.init(G, intEdgeSet);
}

bool Ball::validRhoCenter(const Graph& G) const {
	
	for (int i = 0; i < decomposedGraph.compAmount; i++) {

		int compSize = decomposedGraph.components[i].numberOfNodes();
		
		if (compSize > G.numberOfNodes()) {
			return false;
		}
	}
	return true;
}



void DistanceMetric::init(const Graph& G) {

	/*
	NodeArray<NodeArray<int>> m_distMatrix(G);
	for (node v : G.nodes) {
		distMatrix[v].init(G);
		distMatrix[v][v] = 0;
	}*/

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
	int j = 0;
	for (int i = 0; i < maxDistanceMap.size() && j < G.numberOfNodes(); i++) {
		for (node v : maxDistanceMap[i]) {
			nodesByMaxDistance[j] = v;
			j++;
		}
	}

#ifdef OGDF_DEBUG
	std::cout << "Max Max Distance: " << maxDistance <<"; Shortest Distances in G:" << std::endl;
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

int DistanceMetric::better_bfs_SPAP(const Graph& G, NodeArray<NodeArray<node>>& predMatrix,
		NodeArray<ArrayBuffer<ArrayBuffer<node>>>& distMapArr, ArrayBuffer<ArrayBuffer<node>>& maxDistMap) const {
	
	int maxDist = 0;
	for (node v : G.nodes) {
		better_bfs_SPSS(v, G, predMatrix[v], distMapArr[v]);


		int max = distMapArr[v].size();
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



void ComponentSet::init(const Graph& G, EdgeSet removedEdges) {
	compIndices = NodeArray<int>(G);

	ArrayBuffer<node> repr;

	GraphCopySimple tempG;
	tempG.setOriginalGraph(G);

	for (edge e : removedEdges) {
		tempG.delEdge(e);
	}
	
	compAmount = connectedComponents(tempG, compIndices, nullptr, &repr);
	
	components.init(compAmount);
	nodeMap.init(compAmount);


	for (int i = 0; i < compAmount; i++) {
		nodeMap[i].init(G);
		embedder::ConnectedSubgraph<int>::call(G, components[i], repr[i], nodeMap[i]);
	}
}



void ConnectivityPartition::init(const Graph& G) {
	clusterIndices = NodeArray<int>(G);
	clusters = ArrayBuffer<NodeSet>(G.numberOfNodes());

	for (node v : G.nodes) {
		clusterIndices[v] = v->index();

		clusters[v->index()] = NodeSet(G);
		clusters[v->index()].clear();
		clusters[v->index()].insert(v);
	}
}

bool ConnectivityPartition::merge(edge e) {
	int sourceIndex = clusterIndices[e->source()];
	int targetIndex = clusterIndices[e->target()];

	if (sourceIndex == targetIndex) {
		return false;
	}

	clusterIndices[e->target()] = sourceIndex;

	for (node v : clusters[targetIndex]) {
		clusters[sourceIndex].insert(v);
	}

	//to do: maybe have clusters organized by distance to rho-center, then this step'd be easier, but it'd need to be reSorted for each rho-center.
	ArrayBuffer<int> removedCluster(1, targetIndex);
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

}

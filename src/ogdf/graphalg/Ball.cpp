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
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>
#include <ogdf/graphalg/Ball.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
bool Ball::findRhoCenter(const Graph& G, ComponentSet& Components) const {
	Array<int> compSizes(m_compAmnt,0);
	for (node v : G.nodes) {
		compSizes[compIndices[v]]++;
	}
	
	for (int i = 0; i < m_compAmnt; i++) {
		if (compSizes[i] > G.numberOfNodes()) {
			return false;
		}
	}
	return true;
}

bool Ball::isHub(const Graph& G, ConnectivityPartition& P, ComponentSet& components) const {
	for (int i = 1; i < Clusters.size(); i++) { //assumes 0 is kernel cluster
		int compNumber = -1;
		bool init = false;
		for (node v : G.nodes) { //to do: optimize so that it doesn't iterate through all nodes in G for every Cluster
			if (Clusters[i][v]) {
				if (compIndices[v] != compNumber) {
					if (init) {
						return false;
					}
					compNumber = compIndices[v];
					init = true;
				}
			}
		}
	}
	return true;
}



DistanceMetric::DistanceMetric(const Graph& G) {

	//initialize matrices:
	NodeArray<NodeArray<int>> distMatrix(G);
	for (node v : G.nodes) {
		distMatrix[v].init(G);
		distMatrix[v][v] = 0;
	}

	NodeArray<NodeArray<node>> predMatrix(G);
	for (node v : G.nodes) {
		predMatrix[v].init(G);
	}

	NodeArray<Array<ArrayBuffer<node>>> distMapArr(G);
	for (node v : G.nodes) {
		distMapArr[v].init(G.numberOfNodes());
		for (int i = 0; i < G.numberOfNodes(); i++) {
			distMapArr[v][i] = SListPure<node>();
		}
	}

	Array<ArrayBuffer<node>> MaxDistMap(G.numberOfNodes());
	for (int i = 0; i < G.numberOfNodes(); i++) {
		MaxDistMap[i] = SListPure<node>();
	}

	int maxDist = mod_bfs_SPAP(G, distMapArr, predMatrix, MaxDistMap);

#ifdef OGDF_DEBUG
	std::cout << "Max Max Distance: " << maxDist <<"; Shortest Distances in G:" << std::endl;
	for (int i = 0; i < G.numberOfNodes(); i++) {
		std::cout << "Max Distance "<< i << ": {" << std::endl << "	";
		for (node v : MaxDistMap[i]) {
			std::cout << "Source Node "<< v->index() << ": [" << std::endl << "		";
			for (int j = 0; j < G.numberOfNodes(); j++) {
				std::cout << "dest-nodes at dist "<< j << ": { ";
				for (node u : distMapArr[v][j]) {
					std::cout << u->index() << " (pre: " << predMatrix[v][u]->index() << "), ";
				}
				std::cout << "}" << std::endl << "		";
			}
			std::cout << "]" << std::endl << "	";

		}
		std::cout << "}" << std::endl;
	}
#endif
	
	return maxDist;
}

int DistanceMetric::mod_bfs_SPAP(const Graph& G, NodeArray<Array<SListPure<node>>>& distMapArr, 
		NodeArray<NodeArray<node>>& predMatrix, Array<SListPure<node>>& MaxDistMap) {
	for (node v : G.nodes) {
		mod_bfs_SPSS(v, G, distMapArr[v], predMatrix[v]);
	}

	int maxDist = 0;
	for (Array<SListPure<node>> V : distMapArr) {
		int max = -1;
		for (int i = 0; i < V.size() && !V[i].empty(); i++) {
			max++;
		}
		if (max > maxDist) {
			maxDist = max;
		}
		MaxDistMap[max].pushBack(V[0].front()); //to do: maybe make a sorted list instead, 
		//the purpose of SortedNodes is mainly for iterating through rho-centers with smallest max distance to biggest max distance
		// in the end it still has to go through all to exclude all, as a big max distance doesn't necessarily deny a rho-center 
		//(exception being when rho >= max distance of node). Another heuristic could be to count how many nodes are within a certain distance, 
		//but this'd need to be done and sorted for each possible rho, 
		//to make such an algorithm you'd just count the sum of all destination nodes within the smallest distance, 
		//then go to the next and add that ones amount + the previous, and so on. but having to sort it for each rho would still take time.
		//maybe less time than testing for rho-centers though.

		return maxDist;
	}
}

void DistanceMetric::mod_bfs_SPSS(node s, const Graph& G, 
		Array<SListPure<node>>& distMap, NodeArray<node>& predArr) const {
	NodeArray<int> mark(G, 0);
	SListPure<node> bfs;
	bfs.pushBack(s);
	// mark s and set distance to itself 0
	mark[s] = 1;
	distMap[0].pushBack(s);
	predArr[s] = s;
	while (!bfs.empty()) {
		node w = bfs.popFrontRet();
		int d = mark[w];
		for (adjEntry adj : w->adjEntries) {
			node v = adj->twinNode();
			if (mark[v] == 0) {
				mark[v] = d + 1;
				bfs.pushBack(v);
				distMap[d].pushBack(v);
				predArr[v] = w;
			}
		}
	}
}



int ComponentSet::constructComponents(const Graph& G, NodeArray<bool>& ball, node u, int rho,
		Array<Graph> compArray, NodeArray<int> compIndices, Array<NodeArray<node>> nodeMap) const {
	//Array<GraphCopySimple> Comp_maybe; //to do evaluate if Comp or Comp_maybe is better suited for Emek-Peleg
	//maybe use ConnectedSubGraph.h to make an Array of Graphs
	GraphCopySimple tempG;
	tempG.setOriginalGraph(G);

	for (edge e : G.edges) {
		if (ball[e->source()] && ball[e->target()]) { // to do: maybe already leave in edges that need to be readded later anyway when inducing G(V(G_i)).
			tempG.delEdge(e);
		}
	}

	NodeArray<int> compIndices(tempG);
	ArrayBuffer<node> repr;
	
	int compAmnt = connectedComponents(tempG, compIndices, nullptr, &repr); //to do: return compAmnt by including it as param maybe instead of just writing it into m_compAmnt
	
	Array<Graph> compSubGraphs(compAmnt);
	Array<NodeArray<node>> nodeMappings(compAmnt);


	for (int i = 0; i < compAmnt; i++) {
		nodeMappings[i].init(G);
		embedder::ConnectedSubgraph<int>::call(G, compSubGraphs[i], repr[i], nodeMappings[i]);
	}
	return compAmnt;
}

}

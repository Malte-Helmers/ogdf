/** \file
 * \brief Emek and Peleg's approximation algorithm to calculate a Min Stretch Spanning Tree; ogdf::MinimumStretchSpanningTreeEmekPeleg .
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

#include <ogdf/graphalg/MinStretchSpanningTreeEmekPeleg.h>
#include <ogdf/planarity/embedder/ConnectedSubgraph.h>
#include <ogdf/graphalg/ConnectivityTester.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/basic/GraphSets.h>

namespace ogdf {

bool MinStretchSpanningTreeEmekPeleg::preconditionsOk(const GraphAttributes& GA, std::string& error) {
	if (GA.directed()) {
		error = "The graph must be undirected";
		return false;
	}

	if (GA.has(GA.edgeIntWeight) || GA.has(GA.edgeDoubleWeight)) {
		error = "The graph must be unweighted. Try deleting the weights first";
		return false;
	}

		//test for connectivity with connectivityTester but isConnected(G) is simpler and may even be faster, idk
	//const Graph G = GA.constGraph();
	//ConnectivityTester CT = ConnectivityTester();
	//NodeArray<NodeArray<int>> matrix(G, NodeArray<int>(G));
	//if (CT.computeConnectivity(G,matrix) == 0) {
	//	error = "The graph must be connected. Try inputting each component instead";
	//	return false;
	//}

	const Graph G = GA.constGraph();
	if (!isConnected(G)) {
		error = "The graph must be connected. Try inputting each component instead";
		return false;
	}

	return true;
}

TreeSpannerModule::ReturnType MinStretchSpanningTreeEmekPeleg::execute() {
	const Graph G_hat = m_GA->constGraph();

	//		saving the solution as m_inTreeSpanner, the SolutionGraph as m_treeSpanner and the approximated min stretch as m_stretch.
	 F = constructMSST(G_hat);
	 //m_inTreeSpanner...
	//m_treeSpanner = ...
	//m_stretch = ...
	return TreeSpannerModule::ReturnType::Feasible; // it's within O(log n) of the optimal solution
}

/*
void MinStretchSpanningTreeEmekPeleg::init(const GraphAttributes& GA, GraphCopySimple& treeSpanner, EdgeArray<bool>& inTreeSpanner) {
	TreeSpannerModule::init(GA, treeSpanner, inTreeSpanner);
}
*/

EdgeArray<bool> MinStretchSpanningTreeEmekPeleg::constructMSST(const Graph& G_hat) {
	F.init(G_hat, false);
	F_alt.init(G_hat);

	P = ConnectivityPartition(G_hat);

	EdgeArray<bool> T_hat;

	DistanceMetric delta = DistanceMetric(G_hat);

	int R = G_hat.numberOfNodes();
	int L = 1;


	for (int kappa = floor((L+R)/2); L <= kappa && kappa <= R; kappa = floor((L+R)/2)) { // Binary Search

#ifdef OGDF_DEBUG
		std::cout << "L: " << L << " Kappa: " << kappa << " R: " << R << std::endl;
#endif

		assertTimeLeft();

		//	Reinitialize P and F
		F.init(G_hat, false); //uses Global var F as described by Emek & Peleg
		F_alt.clear();
		P.init(G_hat);


		if (MinStretchSpanningTreeEmekPeleg::constructTree(G_hat, 2*kappa, delta)) {
			T_hat = F;
			R = kappa - 1;

		} else {
			L = kappa + 1;
		}
	}

#ifdef OGDF_DEBUG
		OGDF_ASSERT(T_hat.valid());
#endif

	return T_hat;
}

bool MinStretchSpanningTreeEmekPeleg::constructTree(const Graph& G, int rho, const DistanceMetric& delta) {
#ifdef OGDF_DEBUG
	std::cout << " Begin of constructTree; G has " << G.numberOfNodes() << " Nodes and Rho is " << rho << ":" << std::endl;
#endif
	// Step 0: Exit if n = 1:
	if (G.numberOfNodes() <= 1) {
		return true;
	}
	// End of Step 0

	// Step 1: make a Ball(G, u, rho) for each node u in V(G) and find out if u is a rho-center, fail if none is found:
	//-----------------------------------------------------------------------------------------------------------------
	Ball B = Ball();
	if (!B.findRhoCenter(G, delta, rho)) {
		return false;
	}
	//-----------------------------------------------------------------------------------------------------------------
	// End of Step 1

	// Step 2: call constructLocalTree() and merge its output T_local with F:
	EdgeArray<bool> T_local = constructLocalTree(G, B.rhoCenter, rho, delta);

	for (edge e : G.edges) {	//merge F with T_Local. to do: make more efficient: would be redundant if i just used F in constructLocalTree
		F[e] |= T_local[e];
	}
	// End of Step 2

	//Step 3: determine if P' is a hub, i.e. with exception of the one containing rho-center,
	// if the clusters in F will remain connected when split among the components of Step 1.
		if (!P.isHub(G, B.decomposedGraph, B.rhoCenter)) {
			return false;
		}
	// End of Step 3

	// Step 4: Take Components determined through Step 1 and call constructTree on the graphs induced by their nodes.
	bool success = true;
	for (int i = 0; i < B.decomposedGraph.compAmount && success; i++) {
		Graph* Component = &B.decomposedGraph.components[i];
		if (!constructTree(*Component, rho, delta)) {
			success = false;
		}
	}
	return success;
	// End of Step 4
}

EdgeArray<bool> MinStretchSpanningTreeEmekPeleg::constructLocalTree(const Graph& G, node u, int rho, const DistanceMetric& delta) {
	EdgeArray<bool> T_local(G); // to do maybe just use F outright instead. Would require constructLocalTree to be void.

	// Does a variation of Dijkstra's Algorithm:
	for (int i = 0; i <= (int)(2/3)*rho; i++) {
		for (node v : delta.distanceMapArray[u][i]) {
			if (P.clusterIndices[v] != P.clusterIndices[u]) { // Checks if v is not in Kernel Cluster
				edge lastEdge = G.searchEdge(delta.predecessors[u][v], v); // may be optimised by saving edges in delta, idk... what even is the runtime?
				P.merge(lastEdge);
				T_local[lastEdge] = true;
				//F[lastEdge] = true;
				//F_alt.insert(lastEdge);
			}
		}
	}

	return T_local;
}
}

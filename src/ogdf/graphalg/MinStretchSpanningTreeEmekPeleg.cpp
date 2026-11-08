/** \file
 * \brief Implementation of Emek and Peleg's approximation algorithm to calculate a Min Stretch Spanning Tree; ogdf::MinimumStretchSpanningTreeEmekPeleg .
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

	//		saving the solution as m_inTreeSpanner, the SolutionGraph as m_treeSpanner and the lower Bound kappa for the min stretch is already saved as m_stretch.
	constructMSST(G_hat, *m_inTreeSpanner);

	//printEdges(*m_inTreeSpanner,"m_inTreeSpanner");

	m_treeSpanner->clear();
	m_treeSpanner->setOriginalGraph(G_hat);
	for (node n : G_hat.nodes) {
		m_treeSpanner->newNode(n);
	}

	for (edge e : G_hat.edges) {
		if ((*m_inTreeSpanner)[e]) {
			m_treeSpanner->newEdge(e);
		}
	}

#ifdef OGDF_DEBUG
	GraphAttributes GA_(G_hat);
	OGDF_ASSERT(validStretch(*m_treeSpanner, *m_inTreeSpanner));
	OGDF_ASSERT(isMultiplicativeTreeSpanner(GA_, *m_treeSpanner, calculateStretch(*m_treeSpanner, *m_inTreeSpanner)));
#endif

	//DistanceMetric delta = DistanceMetric(m_treeSpanner);
	//m_stretch = delta.maxDistance;
	return TreeSpannerModule::ReturnType::Feasible; // it's within O(log n) of the optimal solution
}

	bool MinStretchSpanningTreeEmekPeleg::validStretch(const GraphCopySimple& treeSpanner, EdgeArray<bool>& inTreeSpanner) {
		int kappa = m_stretch;
		int approxStretch = calculateStretch(treeSpanner, inTreeSpanner);
		int upperBoundary = ceil( 6 * kappa * log2(treeSpanner.numberOfNodes()) );

#ifdef OGDF_DEBUG
		std::cout << "Kappa: " << kappa << ", approxStretch: " << approxStretch << ", ceiling of 6*kappa*log2(n) = " << upperBoundary << std::endl; 
#endif

		return approxStretch <= upperBoundary;
	}


/*
void MinStretchSpanningTreeEmekPeleg::init(const GraphAttributes& GA, GraphCopySimple& treeSpanner, EdgeArray<bool>& inTreeSpanner) {
	TreeSpannerModule::init(GA, treeSpanner, inTreeSpanner);
}
*/

void MinStretchSpanningTreeEmekPeleg::constructMSST(const Graph& G_hat, EdgeArray<bool>& T_hat) {
	F.init(G_hat, false);
	F_alt.init(G_hat);

	T_hat.init();


	P = ConnectivityPartition(G_hat);

	DistanceMetric delta = DistanceMetric(G_hat);

	GraphCopySimple G; //Editable Copy of G_hat with reference to G_hat


	int R = G_hat.numberOfNodes();
	int L = 1;


	for (int kappa = floor((L+R)/2); L <= kappa && kappa <= R; kappa = floor((L+R)/2)) { // Binary Search

#ifdef OGDF_DEBUG
		//std::cout << "L: " << L << " Kappa: " << kappa << " R: " << R << std::endl;
#endif

		assertTimeLeft();

		//	Reinitialize P and F
		F.init(G_hat, false); //uses Global var F as described by Emek & Peleg
		F_alt.clear();
		P.init(G_hat);

		G.init(G_hat);


		if (MinStretchSpanningTreeEmekPeleg::constructTree(G_hat, G, 2*kappa, delta)) {
			T_hat.init(G_hat, false);

			for (edge e : G_hat.edges) {	//Write F to T_hat. to do: make more efficient
				T_hat[e] = F[e];
			}
			R = kappa - 1;

		} else {
			L = kappa + 1;
		}
		m_stretch = kappa;
	}

#ifdef OGDF_DEBUG
		OGDF_ASSERT(T_hat.valid());
#endif
}

bool MinStretchSpanningTreeEmekPeleg::constructTree(const Graph& G_hat, GraphCopySimple& G, int rho, const DistanceMetric& delta) {
#ifdef OGDF_DEBUG
	//std::cout << " Begin of constructTree; G has " << G.numberOfNodes() << " Nodes and Rho is " << rho << ":" << std::endl;
#endif
	// Step 0: Exit if n = 1:
	if (G.numberOfNodes() <= 1) {
		return true;
	}
	// End of Step 0

	// Step 1: make a Ball(G, u, rho) for each node u in V(G) and find out if u is a rho-center, fail if none is found:
	//-----------------------------------------------------------------------------------------------------------------
	Ball B = Ball();
	if (!B.findRhoCenter(G_hat, G, delta, rho)) {
#ifdef OGDF_DEBUG
		std::cout << "constructTree failed because it didn't find a rho-center!" << std::endl;
#endif
		return false;
	}
	OGDF_ASSERT(B.centerNodeIsRhoCenter);
	//-----------------------------------------------------------------------------------------------------------------
	// End of Step 1

	// Step 2: call constructLocalTree() and merge its output T_local with F:
	EdgeArray<bool> T_local = constructLocalTree(G_hat, G, B.centerNode, rho, delta);

	//version going through all e in G_hat
	for (edge e : G_hat.edges) {	//merge F with T_Local. to do: make more efficient: would be redundant if i just used F in constructLocalTree
		F[e] |= T_local[e];
	}
	//printEdges(T_local,"T_local after return");
	//printEdges(F,"F");

	// End of Step 2

	//Step 3: determine if P' is a hub, i.e. with exception of the one containing rho-center,
	// if the clusters in F will remain connected when split among the components of Step 1.
		if (!P.isHub(G_hat, B.decomposedGraph, B.centerNode)) {

#ifdef OGDF_DEBUG
			std::cout << "constructTree failed because P is not a hub!" << std::endl;
#endif
			return false;
		}
	// End of Step 3

	// Step 4: Take Components determined through Step 1 and call constructTree on the graphs induced by their nodes.
	bool success = true;
	for (int i = 0; i < B.decomposedGraph.compAmount && success; i++) {
		GraphCopySimple* Component = &B.decomposedGraph.components[i];
		if (!constructTree(G_hat, *Component, rho, delta)) {
			success = false;
		}
	}
	return success;
	// End of Step 4
}

EdgeArray<bool> MinStretchSpanningTreeEmekPeleg::constructLocalTree(const Graph& G_hat, GraphCopySimple& G, node u, int rho, const DistanceMetric& delta) {
	EdgeArray<bool> T_local(G_hat); // to do maybe just use F outright instead. Would require constructLocalTree to be void.

	// Does a variation of Dijkstra's Algorithm:
	for (int i = 0; i <= (int)(3/2)*rho && i < delta.distanceMapArray[u].size(); i++) {
		for (node v : delta.distanceMapArray[u][i]) {
			if (G.copy(v) != nullptr && P.clusterIndices[v] != P.clusterIndices[u]) { // Checks if v is not in Kernel Cluster and if v is in G (there may be a more optimal way for checking this.)
				edge lastEdge = G_hat.searchEdge(delta.predecessors[u][v], v); // may be optimised by saving edges in delta, idk... what even is the runtime?
				P.merge(lastEdge);
				T_local[lastEdge] = true;
				//F[lastEdge] = true;
				//F_alt.insert(lastEdge);
			}
		}
	}
	//P.print();
	//printEdges(T_local,"T_local");

	return T_local;
}


}

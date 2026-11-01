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
        m_inTreeSpanner = constructMSST(G_hat);
		//m_treeSpanner = ...
		//m_stretch = ...
        return TreeSpannerModule::ReturnType::Feasible; // it's within O(log n) of the optimal solution
    }

	/*
	void MinStretchSpanningTreeEmekPeleg::init(const GraphAttributes& GA, GraphCopySimple& treeSpanner, EdgeArray<bool>& inTreeSpanner) {
		TreeSpannerModule::init(GA, treeSpanner, inTreeSpanner);
	}
	*/

	EdgeArray<bool>* MinStretchSpanningTreeEmekPeleg::constructMSST(const Graph& G_hat) {
		EdgeArray<bool> F_temp(G_hat);
		Array<NodeArray<bool>> P_temp(G_hat.numberOfNodes());
		F = &F_temp;
		P = &P_temp;
		EdgeArray<bool>* T_hat = nullptr;


		int R = G_hat.numberOfNodes();
		int L = 1;

		int maxDist = MinStretchSpanningTreeEmekPeleg::calculateDist(G_hat, distance, predecessors, sortedNodes);

		for (int kappa = floor((L+R)/2); L <= kappa && kappa <= R; kappa = floor((L+R)/2)) {

#ifdef OGDF_DEBUG
			std::cout << "L: " << L << " Kappa: " << kappa << " R: " << R << std::endl;
#endif
			assertTimeLeft();


			//	Reinitialize P and F
			F->init(G_hat, false); //uses Global var F as described by Emek & Peleg

			P->init(G_hat.numberOfNodes()); //defines Partition P alongside F
			for (node v : G_hat.nodes) {
				(*P)[v->index()].init(G_hat, false);
				(*P)[v->index()][v] = true;
			}


			if (MinStretchSpanningTreeEmekPeleg::constructTree(G_hat, 2*kappa)) {
				T_hat = F;
				R = kappa - 1;

			} else {
				L = kappa + 1;
			}
		}

#ifdef OGDF_DEBUG
			OGDF_ASSERT(T_hat != nullptr);
#endif

		return T_hat;
	}

	bool MinStretchSpanningTreeEmekPeleg::constructTree(const Graph& G, int rho) {
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
		NodeArray<bool> ball;
		node rho_center;
		NodeArray<int> compIndices;

		bool found = false;
		for (int i = 0; i < G.numberOfNodes() && !found; i++) {
			for (node v : sortedNodes[i]) {
				if (!found) {
					ball.init(G,false);
					for (int j = 0; j < rho; j++) {
						for (node u : (*m_dist)[v][j]) {
							ball[u] = true;
						}
					}
					compIndices = makeComponents(G, ball, v, rho);
					if (findRhoCenter(G, compIndices)) {
						rho_center = v;
						found = true;
					}
				}

			}
		}
		if (!found) {
			return false;
		}
		m_compIndices = &compIndices; //to do maybe make more elegant
		//-----------------------------------------------------------------------------------------------------------------
		// End of Step 1

		// Step 2: call constructLocalTree() and merge its output T_local with F:

		NodeArray<bool> temp(G);
		for (int i = 0; i < P->size(); i++) { // swap Kernel Cluster with Cluster[0]
			if ((*P)[i][rho_center]) {
				temp = (*P)[0]; // are these true copies?
				(*P)[0] = (*P)[i];
				(*P)[i] = temp;
			}
		}

		delete &temp;


		EdgeArray<bool> T_local = constructLocalTree(G, rho_center, rho , *P);

		for (edge e : G.edges) {	//merge F with T_Local.
			(*F)[e] |= T_local[e];
		}
		// End of Step 2

		//Step 3: determine if P' is a hub, i.e. with exception of the one containing rho-center,
		// if the clusters in F will remain connected when split among the components of Step 1.
			if (!isHub(G, *P, compIndices)) {
				return false;
			}
		// End of Step 3

		// Step 4: Take Components determined through Step 1 and call constructTree on the graphs induced by their nodes.
		bool success = true;
		for (int i = 0; i < m_compAmnt && success; i++) {
			if (!constructTree((*m_compSubGraphs)[i],rho)) {
				success = false;
			}
		}
		return success;
		// End of Step 4
	}

	EdgeArray<bool> MinStretchSpanningTreeEmekPeleg::constructLocalTree(const Graph& G, node u, int rho, Array<NodeArray<bool>>& Clusters) {
		EdgeArray<bool> T_local(G); // to do: maybe use EdgeSet instead of EdgeArray<bool>, and NodeSet instead of NodeArray<bool>: EdgeSet T_local(G);
		int resultSize = Clusters.size();
		Array<bool> remClusters(resultSize,true);

		// Does a variation of Dijkstra's Algorithm:
		for (int i = 0; i < (int)(2/3)*rho; i++) {
			for (node v : (*m_dist)[u][i]) {
				if (!Clusters[0][v]) {
					for (int j = 1; j < P->size(); j++) {
						if (Clusters[j][v]) {
							remClusters[j] = false;
							resultSize--;
							for (node x : G.nodes) { // Merge Cluster 0 with Cluster that contains v
								Clusters[0][x] |= Clusters[j][x];
							}
						}
					}
					edge lastEdge = G.searchEdge((*m_predecessors)[u][v], v);
					T_local(lastEdge) = true;
				}
			}
		}

		//Constructs new Clusters-Array P':
		Array<NodeArray<bool>> result(resultSize);
		int newElem = 1;
		result[0] = Clusters[0]; // is this a true copy?

		for (int i = 1; i < Clusters.size() && newElem < resultSize; i++) {
			if (remClusters[i]) {
				result[newElem] = Clusters[i]; // is this a true copy?
				newElem++;
			}
		}

		P = &result; //to do maybe make more elegant

		return T_local;
	}
}

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
#include <ogdf/graphalg/ConnectivityTester.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>

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
        const Graph G = GA.constGraph();
        ConnectivityTester CT = ConnectivityTester();
        NodeArray<NodeArray<int>> matrix(G, NodeArray<int>(G));
        if (CT.computeConnectivity(G,matrix) == 0) {
			error = "The graph must be connected. Try inputting each component instead";
			return false;
		}
		return true;
	}

    TreeSpannerModule::ReturnType MinStretchSpanningTreeEmekPeleg::execute() {
        constructMSST();
        return TreeSpannerModule::ReturnType::Optimal; //Not entirely true, it's within O(log n) of the actual optimal solution
    }

	void MinStretchSpanningTreeEmekPeleg::init(const GraphAttributes& GA, GraphCopySimple& treeSpanner, EdgeArray<bool>& inTreeSpanner) {
		TreeSpannerModule::init(GA, treeSpanner, inTreeSpanner);
		m_G.clear();
		m_G.init(GA.constGraph());
	}

	void MinStretchSpanningTreeEmekPeleg::mod_bfs_SPAP(const Graph& G, NodeArray<Array<SListPure<node>>>& distance, 
			NodeArray<NodeArray<node>>& predecessors, Array<SListPure<node>>& sortedNodes, int& maxDist) {
		for (node v : G.nodes) {
			mod_bfs_SPSS(v, G, distance[v], predecessors[v]);
		}

		maxDist = 0;
		for (Array<SListPure<node>> V : distance) {
			int max = -1;
			for (int i = 0; i < V.size() && !V[i].empty(); i++) {
				max++;
			}
			if (max > maxDist) {
				maxDist = max;
			}
			sortedNodes[max].pushBack(V[0].front()); //to do: maybe make a sorted list instead, 
			//the purpose of SortedNodes is mainly for iterating through rho-centers with smallest max distance to biggest max distance
			// in the end it still has to go through all to exclude all, as a big max distance doesn't necessarily deny a rho-center 
			//(exception being when rho >= max distance of node). Another heuristic could be to count how many nodes are within a certain distance, 
			//but this'd need to be done and sorted for each possible rho, 
			//to make such an algorithm you'd just count the sum of all destination nodes within the smallest distance, 
			//then go to the next and add that ones amount + the previous, and so on. but having to sort it for each rho would still take time.
			//maybe less time than testing for rho-centers though.
		}
	}

	void MinStretchSpanningTreeEmekPeleg::mod_bfs_SPSS(node s, const Graph& G, Array<SListPure<node>>& distanceArray, NodeArray<node>& predecessorArray) {
		NodeArray<int> mark(G, 0);
		SListPure<node> bfs;
		bfs.pushBack(s);
		// mark s and set distance to itself 0
		mark[s] = 1;
		distanceArray[0].pushBack(s);
		predecessorArray[s] = s;
		while (!bfs.empty()) {
			node w = bfs.popFrontRet();
			int d = mark[w];
			for (adjEntry adj : w->adjEntries) {
				node v = adj->twinNode();
				if (mark[v] == 0) {
					mark[v] = d + 1;
					bfs.pushBack(v);
					distanceArray[d].pushBack(v);
					predecessorArray[v] = w;
				}
			}
		}
	}

	void MinStretchSpanningTreeEmekPeleg::calculateDist() {
		/* //calc dist using default SPAP:
		NodeArray<NodeArray<int>> distance(m_G);
		for (node v : m_G.nodes) {
			distance[v].init(m_G);
			distance[v][v] = 0;
		}

		bfs_SPAP(m_G, distance, 1); // To do: optimize runtime by using thorup SPSP for all vertices, currently has O(|V²|+|V||E|)
									// with Thorup's algorithm it'd be O(|E|) for single source, so O(|V|*|E|) for all pairs.

#ifdef OGDF_DEBUG
		std::cout << "shortest distances in m_G:" << std::endl;
		for (node v : m_G.nodes) {
			for (node u : m_G.nodes) {
				std::cout << distance[v][u] << " ";
			}
			std::cout << std::endl;
		}
#endif
		m_dist = distance;

		return;
		*/
		
		//calc dist using modified SPAP:
		NodeArray<Array<SListPure<node>>> distance(m_G);
		for (node v : m_G.nodes) {
			distance[v].init(m_G.numberOfNodes());
			for (int i = 0; i < m_G.numberOfNodes(); i++) {
				distance[v][i] = SListPure<node>();
			}
		}

		NodeArray<NodeArray<node>> predecessors(m_G);
		for (node v : m_G.nodes) {
			predecessors[v].init(m_G);
		}

		Array<SListPure<node>> sortedNodes(m_G.numberOfNodes());
		for (int i = 0; i < m_G.numberOfNodes(); i++) {
			sortedNodes[i] = SListPure<node>();
		}

		int maxDist = 0;

		mod_bfs_SPAP(m_G, distance, predecessors, sortedNodes, maxDist);

#ifdef OGDF_DEBUG
		std::cout << "Max Max Distance: " << maxDist <<"; Shortest Distances in m_G:" << std::endl;
		for (int i = 0; i < m_G.numberOfNodes(); i++) {
			std::cout << "Max Distance "<< i << ": {" << std::endl << "	";
			for (node v : sortedNodes[i]) {
				std::cout << "Source Node "<< v->index() << ": [" << std::endl << "		";
				for (int j = 0; j < m_G.numberOfNodes(); j++) {
					std::cout << "dest-nodes at dist "<< j << ": { ";
					for (node u : distance[v][j]) {
						std::cout << u->index() << " (pre: " << predecessors[v][u]->index() << "), ";
					}
					std::cout << "}" << std::endl << "		";
				}
				std::cout << "]" << std::endl << "	";

			}
			std::cout << "}" << std::endl;
		}
#endif
		m_dist = distance;
		m_predecessors = predecessors;
		m_sortedNodes = sortedNodes;
		m_maxDist = maxDist;

		return;
	}

	void MinStretchSpanningTreeEmekPeleg::constructMSST() {
		int R = m_G.numberOfNodes();
		int L = 1;
		GraphCopySimple T_head;

		MinStretchSpanningTreeEmekPeleg::calculateDist();

		for (m_stretch = floor((L+R)/2); L <= m_stretch && m_stretch <= R; m_stretch = floor((L+R)/2)) {
#ifdef OGDF_DEBUG
			std::cout << "L: " << L << " Kappa: " << m_stretch << " R: " << R << std::endl;
#endif
			if (MinStretchSpanningTreeEmekPeleg::constructTree(m_G, 2*m_stretch)) {
				T_head = *m_treeSpanner;
				R = m_stretch - 1;
			} else {
				L = m_stretch + 1;
			}
			MinStretchSpanningTreeEmekPeleg::init(*m_GA, *m_treeSpanner, *m_inTreeSpanner);
		}
#ifdef OGDF_DEBUG
			OGDF_ASSERT(!( T_head.empty() ));
#endif
		m_treeSpanner = &T_head;
		//m_inTreeSpanner = ...
		return;
	}

	Array<GraphCopySimple> MinStretchSpanningTreeEmekPeleg:: makeComponents(NodeArray<bool> ball, node u, unsigned int rho) {
		Array<GraphCopySimple> Comp;

		return Comp;
	}

	bool MinStretchSpanningTreeEmekPeleg::findRhoCenter(Array<GraphCopySimple> Components) {
		return true;
	}

	bool MinStretchSpanningTreeEmekPeleg::isHub(Array<NodeArray<bool>> Clusters, Array<GraphCopySimple> Components) {
		return true;	
	}

	bool MinStretchSpanningTreeEmekPeleg::constructTree(GraphCopySimple G, unsigned int p) {
#ifdef OGDF_DEBUG
		std::cout << " Begin of constructTree; G has " << G.numberOfNodes() << " Nodes and Rho is " << p << ":" << std::endl;
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
		Array<GraphCopySimple> Components;

		bool found = false;
		for (int i = 0; i < m_G.numberOfNodes() && !found; i++) {
			for (node v : m_sortedNodes[i]) {
				if (!found) {
					ball.init(G,false);
					for (int j = 0; j < p; j++) {
						for (node u : m_dist[v][j]) {
							ball[u] = true;
						}
					}
					Components = makeComponents(ball,v,p);
					if (findRhoCenter(Components)) {
						rho_center = v;
						found = true;
					}
				}

			}
		}
		if (!found) {
			return false;
		}
		//-----------------------------------------------------------------------------------------------------------------
		// End of Step 1

		// Step 2: call constructLocalTree() and merge its output with F:
		Array<NodeArray<bool>> Clusters;
		m_inTreeSpanner = &constructLocalTree(G, p, Clusters); //to do make this a true merge
		// End of Step 2

		//Step 3: determine if P' is a hub, i.e. with exception of the one containing rho-center,
		// if the clusters in m_inTreeSpanner will remain connected when split among the components of Step 1.
			if (!isHub(Clusters,Components)) {
				return false;
			}
		// End of Step 3

		// Step 4: Take Components determined through Step 1 and call constructTree on the graphs induced by their nodes.
		bool success = true;
		for (int i = 0; i < Components.size() && success; i++) {
			if (!constructTree(Components[i],p)) {
				success = false;
			}
		}
		return success;
		// End of Step 4
	}

	EdgeArray<bool> MinStretchSpanningTreeEmekPeleg::constructLocalTree(Graph G, unsigned int p, Array<NodeArray<bool>> Clusters) const {
		EdgeArray<bool> T_local(G);
		return T_local;
	}
}

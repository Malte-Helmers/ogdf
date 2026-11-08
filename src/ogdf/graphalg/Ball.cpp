/** \file
 * \brief Implementation of a naive algorithm to calculate a Ball-SubGraph; ogdf::Ball .
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
	
bool Ball::findRhoCenter(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) {

	bool found = false;

	for (int i = 0; i < G_hat.numberOfNodes() && !found; i++) {
		node u = delta.nodesByMaxDistance[i]; // sometimes a better stretch can be achieved through another rho-center. maybe it could be worth it to remember and try out other rho-centers as well for the final valid rho.
		if (G.copy(u) != nullptr) { // maybe optimisable? instead of going through G_hat's nodes and checking which are in G, maybe just going through G is possible, but maybe not in combination with using delta for optimisation.
			init(G_hat, G, delta, rho, u);

			if (validRhoCenter(G_hat, G)) {
				OGDF_ASSERT(centerNode == u);
				centerNodeIsRhoCenter = true;
				found = true;
			}
		}
	}

	return found;
}

void Ball::init(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho, node u) {
	radius = rho;
	centerNode = u;
	centerNodeIsRhoCenter = false;

	internalNodes.init(G_hat, false);
	//internalEdges.init(G_hat, false);

	intEdgeSet.init(G_hat);
	intEdgeSet.clear();

	for (int i = 0; i <= rho && i < delta.distanceMapArray[u].size(); i++) {
		for (node v : delta.distanceMapArray[u][i]) {
			if (G.copy(v) != nullptr) {
				internalNodes[v] = true;
			}
		}
	}

	for (edge e : G_hat.edges) { // to do: optimize so that it doesn't iterate through every edge for this, may need to change DistanceMetric to track edges for this... Or maybe I can use destnode as source node and search for destnode with dist 1 to find edges, maybe in practise too slow
		if (internalNodes[e->source()] && internalNodes[e->target()]) {
			//internalEdges[e] = true;
			intEdgeSet.insert(e);
		}
	}

	decomposedGraph.init(G_hat, G, intEdgeSet);
}

bool Ball::validRhoCenter(const Graph& G_hat, GraphCopySimple& G) const {
	
	for (int i = 0; i < decomposedGraph.compAmount; i++) {

		int compSize = decomposedGraph.components[i].numberOfNodes();
		
		if (compSize > G.numberOfNodes()) {
			return false;
		}
	}
	return true;
}

void Ball::print() {
#ifdef OGDF_DEBUG
	std::cout << "Ball with radius " << radius << ((centerNodeIsRhoCenter) ? " and Rho-Center " : " and center-node ") << centerNode->index() << ":" << std::endl;
	std::cout << "Internal Nodes are:" << std::endl;
	for (node v : internalNodes.graphOf()->nodes) {
		if (internalNodes[v]) {
			std::cout << "Node " << v->index() << " with Edges to ";
			for (edge e : intEdgeSet) {
				if (e->source() == v) {
					std::cout << "Target-Node " << e->target()->index() << ", ";
				}
			}
			std::cout << std::endl;
		}
	}
#endif
}

}

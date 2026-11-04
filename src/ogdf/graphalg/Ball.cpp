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
	
bool Ball::findRhoCenter(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) {

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

void Ball::init(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho, node u) {
	internalNodes.init(G, false);
	//internalEdges.init(G, false);

	intEdgeSet.init(G);
	intEdgeSet.clear();

	for (int i = 0; i <= rho && i < delta.distanceMapArray[u].size(); i++) {
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

bool Ball::validRhoCenter(const Graph& G_hat, GraphCopySimple& G) const {
	
	for (int i = 0; i < decomposedGraph.compAmount; i++) {

		int compSize = decomposedGraph.components[i].numberOfNodes();
		
		if (compSize > G.numberOfNodes()) {
			return false;
		}
	}
	return true;
}

}

/** \file
 * \brief <<DESCRIBE WHAT IS IMPLEMENTED>>
 *
 * \author <<AUTHOR NAMES (RESPONSIBLE PERSONS)>>
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
#include <ogdf/graphalg/ComponentSet.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

void ComponentSet::init(const Graph& G_hat, GraphCopySimple& G, EdgeSet removedEdges) {
	compIndices = NodeArray<int>(G_hat);

	ArrayBuffer<node> repr;

	for (edge e : removedEdges) {
		G.delEdge(G.copy(e));
	}
	
	//compAmount = connectedComponents(G, compIndices, nullptr, &repr);
	determineComponents(G_hat, G, compIndices, repr);
	
	components.init(compAmount);
	//nodeMap.init(compAmount);


	for (int i = 0; i < compAmount; i++) {
		//nodeMap[i].init(G);
		//embedder::ConnectedSubgraph<int>::call(G, components[i], repr[i], nodeMap[i]);
		constructInducedSubgraph(G_hat, G, compIndices, components[i], repr[i]);
	}
}

void ComponentSet::determineComponents(const Graph& G_hat, GraphCopySimple& G, NodeArray<int>& compIndices, ArrayBuffer<node>& repr) {
	compIndices.init(G_hat, -1);
	repr.init(G.numberOfNodes());

	int compAmnt = 0;

	for (node u : G.nodes) {
		if (compIndices[G.original(u)] == -1) {

			compIndices[G.original(u)] = compAmnt;
			repr.push(G.original(u));

			//simple and naive impl through bfs
			SListPure<node> bfs;
			bfs.pushBack(u);
			while (!bfs.empty()) {
				node w = bfs.popFrontRet();
				for (adjEntry adj : w->adjEntries) {
					node v = adj->twinNode();
					if (compIndices[G.original(v)] == -1) {
						compIndices[G.original(v)] = compAmnt;
						bfs.pushBack(v);
					}
				}
			}

			compAmnt++;
		}
	}
}

void ComponentSet::constructInducedSubgraph(const Graph& G_hat, GraphCopySimple& G, NodeArray<int>& compIndices, GraphCopySimple& subGraph, node source) {
	subGraph.clear();
	subGraph.setOriginalGraph(G_hat);

	int compIndex = compIndices[source];

	//simple and naive impl through bfs
	SListPure<node> bfs;
	NodeArray<bool> markNode(G_hat, false);
	EdgeArray<bool> markEdge(G_hat, false);

	subGraph.newNode(source);
	markNode[source] = true;
	bfs.pushBack(source);

	while (!bfs.empty()) {
		node w = bfs.popFrontRet();
		for (adjEntry adj : w->adjEntries) {
			node v = adj->twinNode();
			edge e = adj->theEdge();
			if (compIndices[v] == compIndex && !markEdge[e]) {

				if (!markNode[v]) {

					subGraph.newNode(v);
					markNode[v] = true;
					bfs.pushBack(v);
				}

				subGraph.newEdge(e);
				markEdge[e] = true;
			}
		}
	}
}

}

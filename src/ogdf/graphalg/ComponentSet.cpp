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

void ComponentSet::init(const Graph& G, EdgeSet removedEdges) {
	compIndices = NodeArray<int>(G);

	ArrayBuffer<node> repr;

	GraphCopySimple tempG;
	tempG.init(G);

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

}

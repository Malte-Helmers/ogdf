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
        if (CT.computeConnectivity(G,matrix)) {
			error = "The graph must be connected. Try inputting each component instead";
			return false;
		}
		return true;
	}

    TreeSpannerModule::ReturnType MinStretchSpanningTreeEmekPeleg::execute() {
        constructMSST();
        return TreeSpannerModule::ReturnType::Optimal; //Not entirely true, it's within O(log n) of the actual optimal solution
    }

	void MinStretchSpanningTreeEmekPeleg::init(const GraphAttributes& GA, GraphCopySimple& treeSpanner,
			EdgeArray<bool>& inTreeSpanner) {
		TreeSpannerModule::init(GA, treeSpanner, inTreeSpanner);
		m_G.clear();
		m_G.init(GA.constGraph());
	}
}

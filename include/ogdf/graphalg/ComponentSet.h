/** \file
 * \brief Declaration of a simple and naive Component-Set.
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

#pragma once

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphCopy.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
#include <ogdf/basic/GraphSets.h>

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

class OGDF_EXPORT ComponentSet {
public:
	ComponentSet() = default;

	ComponentSet(const Graph& G_hat, GraphCopySimple& G, EdgeSet removedEdges) {this->init(G_hat, G, removedEdges);}

	void init(const Graph& G_hat, GraphCopySimple& G, EdgeSet removedEdges) ;

	NodeArray<int> compIndices;
	Array<GraphCopySimple> components;

	//Array<NodeArray<node>> nodeMap;

	int compAmount;

protected:
	int determineComponents(const Graph& G_hat, GraphCopySimple& G, NodeArray<int>& compInd, ArrayBuffer<node>& repr) const ;

	void constructInducedSubgraph(const Graph& G_hat, GraphCopySimple& G, NodeArray<int>& compInd, GraphCopySimple& subGraph, node source) const ;

public:

	void print() ;

};

}

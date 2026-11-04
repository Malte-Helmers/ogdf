/** \file
 * \brief <<DESCRIBE WHAT IS DECLARED>>
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

#pragma once

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
#include <ogdf/basic/GraphSets.h>

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {

class OGDF_EXPORT DistanceMetric {
public:
	DistanceMetric() = default;

	DistanceMetric(const Graph& G) {this->init(G);}

	void init(const Graph& G) ;

	//NodeArray<NodeArray<int>> distances;
	NodeArray<NodeArray<node>> predecessors;

	NodeArray<ArrayBuffer<ArrayBuffer<node>>> distanceMapArray;
	ArrayBuffer<ArrayBuffer<node>> maxDistanceMap;

	Array<node> nodesByMaxDistance;

	int maxDistance;

protected:

	int better_bfs_SPAP
	(
		const Graph& G, 
		NodeArray<NodeArray<node>>& predMatrix, 
		NodeArray<ArrayBuffer<ArrayBuffer<node>>>& distMapArr, 
		ArrayBuffer<ArrayBuffer<node>>& maxDistMap
	) const ;

	void better_bfs_SPSS
	(
		node s, 
		const Graph& G, 
		NodeArray<node>& predArr, // Saves the predecessor in Path from s to node with id corresponding to position in matrix
		ArrayBuffer<ArrayBuffer<node>>& distMap // Saves a queue of nodes for each distance corresponding to the position in matrix
	) const ;
};

}

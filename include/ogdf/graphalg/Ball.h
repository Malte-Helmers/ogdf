/** \file
 * \brief Declaration of a naive algorithm to calculate a Ball-SubGraph; ogdf::Ball .
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
#include <ogdf/graphalg/DistanceMetric.h>
#include <ogdf/graphalg/ComponentSet.h>

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
/**
 * Constructs a vertex induced subgraph (or Ball) of a graph by defining a center node #m_u and an edge radius #m_p.
 *
 * The Ball only consists of nodes v with dist(#m_u, v) <= #m_p .
 *
 * The original Graph without the Ball's edges decomposes into multiple components that are put into a Component-List.
 * 
 * If the node u is a #m_p-center the amount of vertices of each component are at most half the amount of vertices of the original graph.
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT Ball {
public :

	//! Creates an instance of Ball.
	Ball() = default;

	Ball(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) {this->findRhoCenter(G_hat, G, delta, rho);}

	Ball(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho, node u) {this->init(G_hat, G, delta, rho, u);}


	//! Destructor.
	~Ball() = default;

	bool findRhoCenter(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) ;

	void init(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho, node u) ;

	NodeArray<bool> internalNodes;
	//EdgeArray<bool> internalEdges; //kinda redundant right now
	EdgeSet intEdgeSet;

	ComponentSet decomposedGraph;

	int radius;

	node centerNode;

	bool centerNodeIsRhoCenter;

protected:

	bool validRhoCenter(const Graph& G_hat, GraphCopySimple& G) const ;

public:

	void print() ;

};

}

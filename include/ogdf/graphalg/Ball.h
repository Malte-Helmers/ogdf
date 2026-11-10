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
 * The original Graph without the Ball's internal edges decomposes into multiple components.
 * 
 * These components are then used to induce vertec induced Subgraphs of the Graph, which are put into a Component-List.
 * 
 * If the node u is a rho-center the amount of vertices of each component are at most half the amount of vertices of the original graph.
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT Ball {
public :

	//! Creates an instance of Ball.
	Ball() = default;

	/**
	 * Creates an Instance of Ball and initializes it by looking for a Rho-Center to use as Center Node, 
	 * 
	 * If unsuccessful, it will use the last Node it looked at
	 * 
	 * Check centerNodeIsRhoCenter to see if it found a Rho-Center
	 *
	 * \param G_hat The Original Graph referenced by G
	 * \param G The Graph to construct a Ball on
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP to allow finding each internal Node in constant Time
	 * \param rho The Ball's radius
	 */
	Ball(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) {this->init(G_hat, G, delta, rho);}

	/**
	 * Creates an Instance of Ball and initializes it with the given center-Node
	 * 
	 * Will set centerNodeIsRhoCenter to false
	 *
	 * \param G_hat The Original Graph referenced by G
	 * \param G The Graph to construct a Ball on
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP to allow finding each internal Node in constant Time
	 * \param rho The Ball's radius
	 * \param u The Ball's center
	 */
	Ball(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho, node u) {this->init(G_hat, G, delta, rho, u);}


	//! Destructor.
	~Ball() = default;

	/**
	 * Re-Initializes the Ball by looking for a Rho-Center to use as Center Node, 
	 * 
	 * If unsuccessful, it will use the last Node it looked at
	 *
	 * \param G_hat The Original Graph referenced by G
	 * \param G The Graph to construct a Ball on
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP to allow finding each internal Node in constant Time
	 * \param rho The Ball's radius
	 * \returns whether it found a Rho-Center, also stored in var centerNodeIsRhoCenter 
	 */
	bool init(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) {
		return findRhoCenter(G_hat, G, delta, rho);
	}

	/**
	 * Looks for a Rho-Center to use as Center Node, 
	 * 
	 * If unsuccessful, it will use the last Node it looked at
	 *
	 * \param G_hat The Original Graph referenced by G
	 * \param G The Graph to construct a Ball on
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP to allow finding each internal Node in constant Time
	 * \param rho The Ball's radius
	 * \returns whether it found a Rho-Center, also stored in var centerNodeIsRhoCenter 
	 */
	bool findRhoCenter(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho) ;

	/**
	 * Re-Initializes the Ball with the given center-Node
	 * 
	 * Will set centerNodeIsRhoCenter to false
	 *
	 * \param G_hat The Original Graph referenced by G
	 * \param G The Graph to construct a Ball on
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP to allow finding each internal Node in constant Time
	 * \param rho The Ball's radius
	 * \param u The Ball's center
	 */
	void init(const Graph& G_hat, GraphCopySimple& G, const DistanceMetric& delta, int rho, node u) ;

	int radius; //!< The Ball's radius
	node centerNode; //!< The Ball's center node, may be a Rho-Center
	bool centerNodeIsRhoCenter; //!< Indicates if centerNode is a Rho-Center or not

	NodeArray<bool> internalNodes; //!< NodeArray that saves the Ball's internal Nodes, so all Nodes v reachable from Center Node u with Dist(u,v) <= radius
	EdgeSet intEdgeSet; //!< EdgeSet that saves the Ball's internal Edges, so all Edges between two internal Nodes

	ComponentSet decomposedGraph; //!< A ComponentSet that saves the VertexInducedSubgraphs induced by the components resulting from the removal of the Ball's internal Edges

protected:

	/**
	 * Checks if the current Center-Node is a Rho-Center
	 *
	 * \param G_hat The Original Graph referenced by G (Redundant, To Do: Remove)
	 * \param G The Graph the Ball is constructed on
	 * \returns whether it found a Rho-Center, also stored in var centerNodeIsRhoCenter 
	 */
	bool validRhoCenter(const Graph& G_hat, GraphCopySimple& G) const ;

public:

	/**
	 * Prints the internal Nodes and Edges of the Ball, its radius, center node, and whether its a Rho-Center
	 * 
	 * Does not print the Components. For that, use: <Ball-Instance>.decomposedGraph.print()
	 */
	void print() ;

};

}

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

/**
 * Constructs a ComponentSet for a given GraphCopy and it's original Graph:
 * 
 * It first removes the specified Edges and then determines the connected Components through the leftover Edges.
 * 
 * The Vertices of these Components are then used to induce a Subgraph on the given original Graph for each Component and saved in a an Array.
 * 
 * This means that any Edges within a Component that got removed through the first Step get readded.
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT ComponentSet {
public:

	//! Creates a ComponentSet
	ComponentSet() = default;

	/**
	 * Creates and initializes a ComponentSet
	 * \param G_hat The original Graph that is referenced by G and should be referenced by the components
	 * \param G The Graph to determine the connected Components from
	 * \param removedEdges The Edges in G_hat to remove from G to create the connected Components from
	 */
	ComponentSet(const Graph& G_hat, GraphCopySimple& G, EdgeSet removedEdges) {this->init(G_hat, G, removedEdges);}

	/**
	 * Re-Initializes the ComponentSet
	 * \param G_hat The original Graph that is referenced by G and should be referenced by the components
	 * \param G The Graph to determine the connected Components from
	 * \param removedEdges The Edges in G_hat to remove from G to create the connected Components from
	 */
	void init(const Graph& G_hat, GraphCopySimple& G, EdgeSet removedEdges) ;


	NodeArray<int> compIndices; //!< NodeArray saving each node's connected Component's index
	Array<GraphCopySimple> components; //!< Array saving the Subgraphs induced by the vertices of the components
	int compAmount; //!< The Amount of Components

	//Array<NodeArray<node>> nodeMap;

protected:

	/**
	 * Determines the Components of a given GraphCopy and saves index and Representative for the Original Graph's Nodes
	 * \param G_hat The original Graph that is referenced by G and should be referenced by the components
	 * \param G The Graph to determine the connected Components from
	 * \param compInd Is assigned the Indices of the components for each of G_hat's nodes
	 * \param repr Is assigned a node of G_hat contained in each of the components
	 */
	int determineComponents(const Graph& G_hat, GraphCopySimple& G, NodeArray<int>& compInd, ArrayBuffer<node>& repr) const ;

	/**
	 * Constructs the Subgraph induced by the vertices of a Component on G_hat
	 * \param G_hat The original Graph that should be referenced by the components
	 * \param G (Redundant. To Do: remove)
	 * \param compInd Contains the Indices of the components for each of G_hat's nodes
	 * \param subGraph Is assigned the vertex-induced Subgraph
	 * \param source The node within the Component to start the bfs from
	 */
	void constructInducedSubgraph(const Graph& G_hat, GraphCopySimple& G, NodeArray<int>& compInd, GraphCopySimple& subGraph, node source) const ;

public:

	/**
	 * Prints the amount of components, each Node in G with the index of its component, and the Edges of each Component
	 */
	void print() ;

};

}

/** \file
 * \brief Declaration of Emek and Peleg's approximation algorithm to calculate a Min Stretch Spanning Tree; ogdf::MinimumStretchSpanningTreeEmekPeleg .
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
#include <ogdf/graphalg/Ball.h>
#include <ogdf/graphalg/ConnectivityPartition.h>
#include <ogdf/graphalg/DistanceMetric.h>
#include <ogdf/graphalg/TreeSpannerModule.h>

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
/**
 * Calculates a spanning tree of a graph that has a maximum stretch within O(log n) of the minimum.
 *
 * Given a graph and its max distance maxDist, a k-Stretch Spanning
 * Tree is a Spanning Tree that has a max distance of k * maxDist.
 *
 * @ingroup ga-treespanner
 */
class OGDF_EXPORT MinStretchSpanningTreeEmekPeleg : public TreeSpannerModule {
public:

	//! @copydoc ogdf::TreeSpannerModule::preconditionsOk
	virtual bool preconditionsOk(const GraphAttributes& GA, std::string& error) override ;

	/**
	 * Calculates the stretch of the given Tree-Spanner and checks if it's within the allowed approximation ratio
	 *
	 * \param treeSpanner The TreeSpanner to check the stretch of
	 * \param inTreeSpanner An EdgeArray containing the Edges of the original Graph used by the TreeSpanner
	 * \returns if the calculated Stretch is within the approximation ratio of 6*kappa*log_2(n)
	 */
	bool validStretch(const GraphCopySimple& treeSpanner, EdgeArray<bool>& inTreeSpanner) ;


	/**
	 * Constructs a min-stretch-spanning-tree for the given graph
	 * 
	 * Essentially solves the optimization problem by doing a binary search on an algortihm that solves the decision problem.
	 * 
	 * Should only fail if the given graph is invalid.
	 * 
	 * \param G_hat The Graph to construct a Min-Stretch Tree-Spanner for
	 * \param T_hat Is assigned the Edges of G_hat that construct the finished TreeSpanner
	 */
	void constructMSST(const Graph& G_hat, EdgeArray<bool>& T_hat) ;

protected:

	//! @copydoc ogdf::TreeSpannerModule::execute
	TreeSpannerModule::ReturnType execute() override ;

	/**
	 * Recursive function that tries to construct an MSST for a given rho := 2 * assumedStretch
	 * 
	 * Can(!) fail if the given rho is smaller than twice the graph's Min-Stretch
	 * 
	 * Is guaranteed to solve the decision problem of finding a Tree-Spanner with a Stretch <= ceil(3*rho*log_2(n)), if rho/2 >= Min-Stretch
	 * 
	 * Sometimes solves the decision problem of finding a Tree-Spanner with a Stretch <= ceil(3*rho*log_2(n)), even if rho/2 < Min-Stretch
	 *
	 * \param G_hat The constant Original Graph referenced by G
	 * \param G A Subgraph of G_hat (can be equal) to construct a Tree-Spanner for
	 * \param rho Twice the Graph's assumed Stretch.
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP.
	 * \returns true always if rho/2 >= Min-Stretch, and false sometimes if rho/2 < Min-Stretch
	 */
	bool constructTree(const Graph& G_hat, GraphCopySimple& G, int rho, const DistanceMetric& delta) ;

	/**
	 * Function that constructs a tree via Dijkstra-Variation on the Intersection of the external Edges of P and a Ball B_delta(u, 3/2rho) on G
	 *
	 * \param G_hat The constant Original Graph referenced by G
	 * \param G A Subgraph of G_hat (can be equal) to construct a Tree on
	 * \param u The Graph G's rho-center
	 * \param rho Twice the Graph's assumed Stretch and 2/3 of the radius of the Ball around u to construct a Tree in
	 * \param delta A DistanceMetric of the original Graph. Contains efficiently sorted APSP.
	 * \returns EdgeArray containing Edges of constructed Tree
	 */
	EdgeArray<bool> constructLocalTree(const Graph& G_hat, GraphCopySimple& G, node u, int rho, const DistanceMetric& delta);

	using TreeSpannerModule::assertTimeLeft;
	using TreeSpannerModule::m_GA;
	using TreeSpannerModule::m_stretch;
	using TreeSpannerModule::m_treeSpanner;
	using TreeSpannerModule::m_inTreeSpanner;
private:

	EdgeArray<bool> F; //!< EdgeArray that saves the Solution for the current rho 
	ConnectivityPartition P; //!< Partition of Node-Clusters which represents the Connectivity of the Nodes in F
};

}
/** \file
 * \brief Declaration and Implementation of Emek and Peleg's approximation algorithm to calculate a Min Stretch Spanning Tree; ogdf::MinimumStretchSpanningTreeEmekPeleg .
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
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
//#include <ogdf/graphalg/ball.h>
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
	virtual bool preconditionsOk(const GraphAttributes& GA, std::string& error) ;

	//! Constructs a min stretch spanning tree for the graph #m_G,
	//! Essentially solves the optimization problem by doing a binary search on an algortihm that solves the decision problem.
	//! Should only fail if graph is invalid.
	void constructMSST();

	//! Returns a pointer to the associated graph.
	const GraphCopySimple* getGraph() const { return (&m_G); }

	//! Sets the associated graph.
	void setGraph(GraphCopySimple& G) {
		OGDF_ASSERT(!G.empty());
		m_G = G;
	}

protected:

	TreeSpannerModule::ReturnType execute() ;

	void init(const GraphAttributes& GA, GraphCopySimple& treeSpanner,
			EdgeArray<bool>& inTreeSpanner) ;

	//! Recursive function that tries to construct an MSST for a given p := 2 * stretch
	//! Can fail if the given p is smaller than twice the graph's #m_G stretch
	bool constructTree(GraphCopySimple G, unsigned int p);

	//! Recursive function that tries to construct an MSST for a given p := 2 * stretch
	EdgeSet constructLocalTree(Graph G, unsigned int p) const;

	//! Returns an edgeless spanning forest of #m_G .
	Graph newForest() const;

	using TreeSpannerModule::assertTimeLeft;
	using TreeSpannerModule::m_GA;
	using TreeSpannerModule::m_stretch;
	using TreeSpannerModule::m_treeSpanner;
	using TreeSpannerModule::m_inTreeSpanner;
private:
	GraphCopySimple m_G; //!< The copy of GA.constGraph() to work on.
	Graph m_F; //!< The Solution; Starts off as edgeless spanning forest of #m_g.

	EpsilonTest m_eps;

};

}
/** \file
 * \brief basic module for treespanner modules
 * 
 * heavily derived from ogdf::spannerModule.
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

#include <ogdf/basic/EpsilonTest.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/GraphCopy.h>
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/Module.h>
#include <ogdf/basic/Stopwatch.h>
#include <ogdf/basic/basic.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>

#include <cstdint>
#include <string>

namespace ogdf {
/**
 * \brief Interface for tree-spanner algorithms.
 *
 * All tree-spanner implementations must:
 * - implement TreeSpannerModule::execute and TreeSpannerModule::preconditionsOk
 * - be timelimited: Call TreeSpannerModule::assertTimeLeft e.g. in the main loop
 * - create a tree-spanner (m_treeSpanner) and a set of the n-1 used edges regarding the original graph (m_inTreeSpanner)
 *
 * @ingroup ga-treespanner
 */
class TreeSpannerModule : public Module {
public:
	//! Initializes a tree-spanner module.
	TreeSpannerModule() : m_GA(nullptr) { }

	// destruction
	virtual ~TreeSpannerModule() { }

	/**
	 * \brief Executes the algorithm.
	 *
	 * Example:
	 * \code
	 * GraphAttributes GA = ....
	 * GraphCopySimple treeSpanner;
	 * EdgeArray<bool> inTreeSpanner;
	 * std::unique_ptr<TreeSpannerModule> sm = std::make_unique<MinStretchSpanningTreeEmekPeleg>();
	 * typename TreeSpannerModule::Result result = sm->call(GA, treeSpanner, inTreeSpanner);
	 * \endcode
	 *
	 *
	 * @param GA The graph used. GraphAttributes can be used to pass in weights
	 * @param treeSpanner The resulting tree-spanner. Must be a copy of GA.constGraph()
	 * @param inTreeSpanner Maps true to an edge if the edge is in the treeSpanner
	 * @return The state in which the algorithm returns. Only for OK the treeSpanner is valid.
	 */
	virtual ReturnType call(const GraphAttributes& GA, GraphCopySimple& treeSpanner,
			EdgeArray<bool>& inTreeSpanner) {
		init(GA, treeSpanner, inTreeSpanner);
#ifdef OGDF_DEBUG
		std::string error;
		OGDF_ASSERT(preconditionsOk(GA, error));
#endif

		if (isTimelimitEnabled()) {
			m_watch.start(true);
		}
		ReturnType result;
		try {
			result = execute();
		} catch (const TimeoutException&) {
			result = ReturnType::TimeoutInfeasible;
		}
		if (isTimelimitEnabled()) {
			m_watch.stop();
		}
		return result;
	}

	/**
	 * @returns true, if the given \p GA is valid for a specific algorithm. If not,
	 * an error message is provided via \p error
	 */
	virtual bool preconditionsOk(const GraphAttributes& GA, std::string& error) = 0;

	/**
	 * Sets the timelimit for the algorithm in milliseconds. Use -1 to disable the timelimit.
	 * On timelimit, the algorithm will be aborted. Note that 0 is a valid timelimit. Do not
	 * change this during the execution of the algorithm.
	 */
	void setTimelimit(int64_t milliseconds) { m_timelimit = milliseconds; }

	/**
	 * @returns the time in milliseconds needed for executing the algorithm (see the execute method).
	 */
	int64_t getTimeNeeded() { return m_watch.milliSeconds(); }

protected:
	const GraphAttributes* m_GA;
	int m_stretch;
	GraphCopySimple* m_treeSpanner;
	EdgeArray<bool>* m_inTreeSpanner;

	/**
	 * Initializes members and create an empty tree-spanner.
	 */
	virtual void init(const GraphAttributes& GA, GraphCopySimple& treeSpanner,
			EdgeArray<bool>& inTreeSpanner) {
		m_GA = &GA;
		m_treeSpanner = &treeSpanner;
		m_inTreeSpanner = &inTreeSpanner;

		const Graph& G = GA.constGraph();
		m_treeSpanner->clear();
		m_treeSpanner->setOriginalGraph(G);
		for (node n : G.nodes) {
			m_treeSpanner->newNode(n);
		}
		m_inTreeSpanner->init(G, false);
	}

	/**
	 * Executes the core algorithm. Called after initialization. This method is used for the
	 * timelimit, so do not forget to call assertTimeLeft from time to time.
	 */
	virtual ReturnType execute() = 0;

	/**
	 * @returns true, if there should be a timelimit.
	 */
	bool isTimelimitEnabled() { return m_timelimit != -1; }

	/**
	 * @returns the amount of time in milliseconds left for execution.
	 */
	int64_t getTimeLeft() { return m_timelimit - getTimeNeeded(); }

	/**
	 * Assert, that time is left. If there is no time left (and the timelimit is active),
	 * an exception will be thrown to abort the execution.
	 */
	void assertTimeLeft() {
		if (isTimelimitEnabled() && getTimeLeft() <= 0) {
			throw TimeoutException();
		}
	}

private:
	int64_t m_timelimit = -1; //!< Timelimit in milliseconds. -1 disables the timeout.
	StopwatchCPU m_watch; //!< Used for keeping track of time.

protected:
	struct TimeoutException {
	}; //!< A simple exception used to exit from the execution, if the timelimit is reached.

public:
	/**
	 * Validates a tree-spanner
	 *
	 * \returns true if the \p treeSpanner is a k-multiplicative tree-spanner of \p GA
	 */
	static bool isMultiplicativeTreeSpanner(const GraphAttributes& GA, const GraphCopySimple& treeSpanner,
			int stretch);

	/**
	 * Calculates an all-pair shortest-path on \p treeSpanner
	 */
	static void apspTreeSpanner(const GraphCopySimple& treeSpanner,
			NodeArray<NodeArray<int>>& shortestPathMatrix);


};

void TreeSpannerModule::apspTreeSpanner(const GraphCopySimple& treeSpanner,
		NodeArray<NodeArray<int>>& shortestPathMatrix) {
	EdgeArray<int> weights(treeSpanner);
	for (edge e : treeSpanner.edges) {
		weights[e] = 1;
	}
	dijkstra_SPAP(treeSpanner, shortestPathMatrix, weights);
}

bool TreeSpannerModule::isMultiplicativeTreeSpanner(const GraphAttributes& GA,
		const GraphCopySimple& treeSpanner, int stretch) {
	const Graph& G = GA.constGraph();
	OGDF_ASSERT(&(treeSpanner.original()) == &G);

	if (G.numberOfNodes() != treeSpanner.numberOfNodes() || treeSpanner.numberOfEdges() != G.numberOfNodes() - 1 ) {
		return false;
	}

	NodeArray<NodeArray<int>> originalDistances(G);
	EdgeArray<int> weights(G);
	for (edge e : G.edges) {
		weights[e] = 1;
	}
	dijkstra_SPAP(G, originalDistances, weights);

	NodeArray<NodeArray<int>> newDistances(treeSpanner);
	apspTreeSpanner(treeSpanner, newDistances);

	EpsilonTest m_eps;
	for (edge e : G.edges) {
		node u = e->source();
		node v = e->target();
		int originalDistance = originalDistances[u][v];
		int newDistance = newDistances[treeSpanner.copy(u)][treeSpanner.copy(v)];
		if (m_eps.greater(static_cast<double>(newDistance), static_cast<double>(stretch * originalDistance))) {
			return false;
		}
	}

	return true;
}

}

/** \file
 * \brief Instance Declaration and Runtime-Testing of Min-Stretch-Spanning-Tree algorithms; ogdf::MinimumStretchSpanningTreesEmekPeleg .
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

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/GraphList.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/energybased/fmmm/FMMMOptions.h>
#include <ogdf/fileformats/GraphIO.h>
//#include <ogdf/graphalg/minStretchSpanningTreeEmekPeleg.h>
//#include <ogdf/graphalg/minStretchSpanningTreeMCF.h>
//#include <ogdf/graphalg/TreeSpannerModule.h>
//#include <ogdf/graphalg/ball.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>

#include <iostream>
#include <string>

using namespace ogdf;

int main(int argc, char *argv[]) { //Input: .gml of Graph.

	// Parsing argv and intializing G
	string inputGraph = "emek_peleg_example.gml";
	if (argc > 1) {
		inputGraph = argv[1];
	}
	Graph G;
	bool Success = GraphIO::read(G, inputGraph);
	if (!Success) {
		std::cerr << "Could not load given .gml" << std::endl;
		return 1;
	}
	// End of Input Parsing

	// Initialize GA and set as undirected
	GraphAttributes GA(G);
	GA.directed() = false;
	const Graph Gc = GA.constGraph();
	// End of GA Initialization


	// Preprocess shortest distances between all node pairs using bfs: Runtime O(n²m) 
	// To do: optimize runtime
	NodeArray<NodeArray<int>> distance(Gc);
	for (node v : Gc.nodes) {
		distance[v].init(Gc);
		distance[v][v] = 0;
	}
	std::cout << "Start of BFS-SPAP" << std::endl;
	bfs_SPAP(Gc, distance, 1);
	std::cout << "shortest distances in G:" << std::endl;
	for (node v : Gc.nodes) {
		for (node u : Gc.nodes) {
			std::cout << distance[v][u] << " ";
		}
		std::cout << std::endl;
	}
	// End of SPAP preprocessing


	// Drawing GA
	for (node v : G.nodes) {
		GA.width(v) = GA.height(v) = 5.0;
	}

	FMMMLayout fmmm;

	fmmm.useHighLevelOptions(true);
	fmmm.unitEdgeLength(15.0);
	fmmm.newInitialPlacement(true);
	fmmm.qualityVersusSpeed(FMMMOptions::QualityVsSpeed::GorgeousAndEfficient);

	fmmm.call(GA);
	GraphIO::write(GA, "output-MinStretchTreeEmekPeleg.gml", GraphIO::writeGML);
	GraphIO::write(GA, "output-MinStretchTreeEmekPeleg.svg", GraphIO::drawSVG);
	// End of Drawing GA

	std::cout << "End of Testing!" << std::endl;
	return 0;
}
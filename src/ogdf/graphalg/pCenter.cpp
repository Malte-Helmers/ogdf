/** \file
 * \brief Implementation of a naive algorithm to calculate a p-center; ogdf::pCenter .
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
#include <ogdf/basic/GraphList.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/basic.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>
#include <ogdf/graphalg/pCenter.h>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
/*
bool pCenter::checkU(node* v)  {
	OGDF_ASSERT(m_G != nullptr);
	
	Graph* Ball;
	List<Graph*>* Components;
	bool valid;

		Graph* Ball = pCenter::createBall(v);
		List<Graph*>* Components = pCenter::scanComponents(Ball);
		valid = true;
		for (Graph* c : Components->getListPure()) {
			if (c->numberOfNodes() > (m_G->numberOfNodes() / 2)) {
				valid = false;
			}
		}

		if (valid) {
			this->m_b = Ball;
			this->m_c = Components;
			this->m_u = v;
			return true;
		}
	return false;
}
*/
bool pCenter::hasValidU()  {
	OGDF_ASSERT(m_G != nullptr);

	Graph* Ball;
	List<Graph*>* Components;
	bool valid;
	for (node v : m_G->nodes) {

		//wip: maybe move to checkU() and replace with call
		Ball = pCenter::createBall(&v);
		Components = pCenter::scanComponents(Ball);
		valid = true;
		for (Graph* c : Components->getListPure()) {
			if (c->numberOfNodes() > (m_G->numberOfNodes() / 2)) {
				valid = false;
			}
		}

		if (valid) {
			this->m_b = Ball;
			this->m_c = Components;
			this->m_u = v;
			return true;
		}
	}
	return false;
}
/*
void pCenter::constructDistMatrix() {
	NodeArray<NodeArray<int>> distance(*m_G);
	for (node v : m_G->nodes) {
		distance[v].init(*m_G);
		distance[v][v] = 0;
	}
	std::cout << "Start of BFS-SPAP" << std::endl;
	bfs_SPAP(*m_G, distance, 1);
	this->m_distMatrix = &distance;
}
*/

 Graph* pCenter::createBall(node* u)  { 
	OGDF_ASSERT(this->m_G != nullptr);
	OGDF_ASSERT(this->m_p != nullptr);
	OGDF_ASSERT(this->m_u != nullptr);

	//if(m_distMatrix=nullptr) {
	//	this->constructDistMatrix();
	//}
	//(*m_distMatrix)[(*u)->index()];

	return 0; 

}


List<Graph*>* pCenter::scanComponents(Graph* b)  { 
	OGDF_ASSERT(m_G != nullptr);
	OGDF_ASSERT(m_b != nullptr);
	return 0; 

}

}

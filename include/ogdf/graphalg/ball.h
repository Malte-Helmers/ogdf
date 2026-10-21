/** \file
 * \brief Declaration of a naive algorithm to calculate a ball; ogdf::ball .
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

#include <functional>
#include <utility>
#include <vector>

namespace ogdf {
/**
 * Constructs a vertex induced subgraph (or Ball) of a graph by defining a center node #m_u and an edge radius #m_p.
 *
 * The ball only consists of nodes v with dist(#m_u, v) <= #m_p .
 *
 * The original Graph without the Ball's edges decomposes into multiple components that are put into a Component-List.
 * 
 * If the node u is a #m_p-center the amount of vertices of each component are at most half the amount of vertices of the original graph.
 * 
 * @ingroup graph-algs
 */
class OGDF_EXPORT ball {
public :
	//! Creates an instance of ball without associated graph #m_G, radius #m_p or node #m_u .
	ball() : m_G {nullptr}, m_p {nullptr}, m_u {nullptr} { }

	//! Creates an instance of ball without radius #m_p or node #m_u .
	ball( Graph& G) : m_G {&G}, m_p {nullptr}, m_u {nullptr} { }

	//! Creates an instance of ball without node #m_u .
	ball( Graph& G,  unsigned int p) : m_G {&G}, m_p {&p}, m_u {nullptr} { }
	/*, pValid {this->hasValidU()}*/

	//! Creates an instance of ball.
	ball( Graph& G,  unsigned int p, node& u) : m_G {&G}, m_p {&p}, m_u {u} {
	}


	//! Destructor.
	~ball() = default;



	//! Returns the radius #m_p.
	unsigned int* getP()  { return m_p; }

	//! Sets the radius #m_p.
	void setP( unsigned int* p) { m_p = p; }



	//! Tries to find a node #m_u such that the size of any Components connected by edges outside of the Ball is <= 0.5 * |V|.
	//! If successful, sets #m_u, #m_b and #m_c to the corresponding findings and returns true.
	//! If unsuccessful, it returns false.
	bool hasPCenter() ;

	//! Checks if the given node is a p_center such that the size of any Components connected by edges outside of the Ball is <= 0.5 * |V|.
	//! If successful, sets #m_u, #m_b and #m_c to the corresponding findings and returns true.
	//! If unsuccessful, it returns false.
	//bool checkU(node* u) ;

	//! Returns the center node #m_u.
	node getU()  { return m_u; }

	//! Sets the center node #m_u.
	void setU(node u) { m_u = u; }



	//! Returns a pointer to the associated graph.
	 Graph* getGraph()  { return m_G; }

	//! Sets the associated graph.
	void setGraph( Graph& G) {
		OGDF_ASSERT(!G.empty());
		m_G = &G;
	}

	//! Constructs the distance Matrix #distMatrix.
	//void constructDistMatrix() ;

	//! Sets the distance Matrix #distMatrix.
	//void setDistMatrix(NodeArray<NodeArray<int>>* distMatrix) { m_distMatrix = distMatrix;}

	//! Returns a pointer to the distance Matrix #distMatrix.
	//NodeArray<NodeArray<int>>* getDistmatrix()  { return m_distMatrix; }

	

	//! Returns a pointer to the ball.
	Graph* getBall()  { return m_b; }

	//! Returns a pointer to a list of the components the graph with the Ball Edges decomposes into.
	 List<Graph*>* getComponents()  { return m_c; }




protected :

	//! Creates an induced Subgraph with all vertices v in #m_G that fulfill dist(#m_u, v) <= #m_p.
	Graph* createBall(node* u) ;

	//! Puts together a List of the components of #m_G that are connected through Edges outside of the Ball.
	List<Graph*>* scanComponents(Graph* b) ;

private : 
	Graph* m_G; //!< The associated graph.
	unsigned int* m_p; //!< The Ball's radius.
	bool pValid = false; //!< A Bool that checks if #m_p constructs a valid p_center on #m_G.
	node m_u; //!< The Center Node of the ball.

	//NodeArray<NodeArray<int>>* m_distMatrix = nullptr;
	Graph* m_b; //!< The induced subgraph, or ball.
	List<Graph*>* m_c; //!< A List of the components that are connected through Edges outside of the Ball.
};

}

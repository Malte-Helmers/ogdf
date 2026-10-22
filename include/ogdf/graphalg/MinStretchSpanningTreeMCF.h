/** \file
 * \brief Declaration and Implementation of an exact algorithm calculating a Minimum Stretch Spanning Tree by using Multicommodity Flow.
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

#include <ogdf/graphalg/TreeSpannerModule.h>

namespace ogdf {
/**
 * Calculates a spanning tree of a graph that has an optimal maximum stretch.
 *
 * Given a graph and its max distance maxDist, a k-Stretch Spanning
 * Tree is a Spanning Tree that has a max distance of k * maxDist.
 *
 * @ingroup ga-treespanner
 */
template<typename TWeight>
class MinimumStretchSpanningTreeMCF : public TreeSpannerModule<TWeight> {
};
}

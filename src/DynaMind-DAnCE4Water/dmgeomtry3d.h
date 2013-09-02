/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef DMGEOMTRY3D_H
#define DMGEOMTRY3D_H

#include <cgalgeometry.h>
#include <vector>

namespace DM {
	class System;
	class Face;
	class View;
	class Subsystem;
}



namespace DM {
	class DMGeomtry3D
	{
	public:

		static std::vector<DM::Face *> BoolOperation(DM::System * sys, std::vector<DM::Face *> facesA, std::vector<DM::Face *> facesB, CGALGeometry::BoolOperation op_dm);

		static std::vector<DM::Face *> SurfaceTriangulation(DM::System * sys, std::vector<DM::Face *> faces);
	};
}


#endif // DMGEOMTRY3D_H

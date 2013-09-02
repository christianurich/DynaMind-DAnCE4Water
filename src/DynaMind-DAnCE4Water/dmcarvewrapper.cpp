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

#include "dmcarvewrapper.h"

#include <dmsystem.h>
#include <dm.h>

#include <geometry.hpp>
#include <carve/csg.hpp>
#include <carve/carve.hpp>
#include <carve/mesh_ops.hpp>
#include <carve/triangulator.hpp>
#include <carve/input.hpp>
#include <cgalgeometry.h>

using namespace DM;

carve::mesh::MeshSet<3> * DMCarveWrapper::FacesToMesh(std::vector<DM::Face*> faces) {

	carve::input::PolyhedronData data;
	std::map<DM::Node *, int> dm_index;
	foreach (DM::Face * f, faces) {
		foreach (DM::Node * n, f->getNodePointers()) {
			if (dm_index.find(n) == dm_index.end()) {
				int index = data.addVertex(carve::geom::VECTOR(n->getX(), n->getY(), n->getZ()));
				dm_index[n] = index;
			}
		}
	}
	//Logger(Debug) << "Object";
	foreach (DM::Face * f, faces) {
		std::vector<int> face;
		//Logger(Debug) << "Face";
		foreach (DM::Node * n, f->getNodePointers()) {
			face.push_back(dm_index[n]);
		}
		data.addFace(face.begin(), face.end());
	}
	return  new carve::mesh::MeshSet<3>(data.points, data.getFaceCount(), data.faceIndices);

}

std::vector<DM::Face*> DMCarveWrapper::MeshToFace(DM::System * sys, carve::mesh::MeshSet<3> * mesh) {
	std::vector<DM::Face*> faces;
	for (carve::mesh::MeshSet<3>::face_iter i = mesh->faceBegin(); i != mesh->faceEnd(); ++i) {
		carve::mesh::MeshSet<3>::face_t *f = *i;
		std::vector<DM::Node*> nodes;
		for (carve::mesh::MeshSet<3>::face_t::edge_iter_t e = f->begin(); e != f->end(); ++e) {
			double x = (e->vert->v.x );
			double y = (e->vert->v.y );
			double z = (e->vert->v.z );
			nodes.push_back(sys->addNode(x,y,z));
		}
		DM::Face * f_dm = sys->addFace(nodes);
		std::vector<double> colors;
		colors.push_back(1);
		colors.push_back(1);
		colors.push_back(0);
		f_dm->getAttribute("color")->setDoubleVector(colors);
		faces.push_back(f_dm);

	}
	return faces;
}

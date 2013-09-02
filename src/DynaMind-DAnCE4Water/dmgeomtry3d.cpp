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

#include "dmgeomtry3d.h"

#include <dmsystem.h>
#include <dm.h>

#include <geometry.hpp>
#include <carve/csg.hpp>
#include <carve/carve.hpp>
#include <carve/mesh_ops.hpp>
#include <carve/triangulator.hpp>
#include <carve/input.hpp>
#include <cgalgeometry.h>
#include <dmcarvewrapper.h>

using namespace DM;

std::vector<DM::Face*> DMGeomtry3D::SurfaceTriangulation(DM::System * sys,std::vector<DM::Face *>faces) {
	std::vector<DM::Face*> mesh;
	carve::mesh::MeshSet<3> * result =  DMCarveWrapper::FacesToMesh(faces);
	for (carve::mesh::MeshSet<3>::face_iter i = result->faceBegin(); i != result->faceEnd(); ++i) {
		carve::mesh::MeshSet<3>::face_t *f = *i;

		std::vector<carve::mesh::Edge<3> *> triangles;
		carve::mesh::triangulate(f->edge, f->project, std::back_inserter(triangles));

		foreach (carve::mesh::Edge<3> * t, triangles) {
			std::vector<DM::Node*> nodes;
			carve::mesh::Edge<3> *e = t;
			do {

				double x = (e->vert->v.x +12);
				double y = (e->vert->v.y +12);
				double z = (e->vert->v.z +12);
				nodes.push_back(sys->addNode(x,y,z));
				e = e->next;
			} while (e != t);

			DM::Face * f_dm = sys->addFace(nodes);
			std::vector<double> colors;


			colors.push_back(1);
			colors.push_back(0);
			colors.push_back(0);
			f_dm->getAttribute("color")->setDoubleVector(colors);
			mesh.push_back(f_dm);
		}
	}
	delete result;
	return mesh;
}



std::vector<DM::Face*> DMGeomtry3D::BoolOperation(DM::System * sys, std::vector<DM::Face*> facesA, std::vector<DM::Face*> facesB, CGALGeometry::BoolOperation dm_op) {

	carve::mesh::MeshSet<3> * a = DMCarveWrapper::FacesToMesh(facesA);
	carve::mesh::MeshSet<3> * b = DMCarveWrapper::FacesToMesh(facesB);


	carve::csg::CSG::OP op;
	switch(dm_op){
	case CGALGeometry::OP_INTERSECT:
		op  = carve::csg::CSG::INTERSECTION;
		break;
	case CGALGeometry::OP_UNION:
		op  = carve::csg::CSG::UNION;
		break;
	case CGALGeometry::OP_DIFFERENCE:
		op  = carve::csg::CSG::A_MINUS_B;
		break;
	}

	carve::csg::CSG csg;

	carve::mesh::MeshSet<3> *result  = csg.compute(a, b, op);
	int face_counter = 0;
	for (carve::mesh::MeshSet<3>::face_iter i = result->faceBegin(); i != result->faceEnd(); ++i) {
		face_counter++;
	}
	Logger(Debug) << "Faces " << face_counter;
	std::vector<DM::Face*> return_faces =  DMCarveWrapper::MeshToFace(sys, result);

	delete a;
	delete b;

	return return_faces;

}

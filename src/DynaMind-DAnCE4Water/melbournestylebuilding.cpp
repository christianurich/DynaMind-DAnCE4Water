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

#include "melbournestylebuilding.h"

#include <dmsystem.h>
#include <dmface.h>
#include <QTransform>
#include <cgalgeometry.h>
#include <tbvectordata.h>
#include <spatialsearchnearestnodes.h>
#include <dmgeomtry3d.h>


DM_DECLARE_NODE_NAME(MelbourneStyleBuilding, DAnCE4Water)



MelbourneStyleBuilding::MelbourneStyleBuilding()
{
	parcels = DM::View("PARCEL", DM::FACE, DM::READ);
	buildings = DM::View("BUILDING", DM::EDGE, DM::WRITE);
	splits = DM::View("SPLITS", DM::EDGE, DM::WRITE);
	stuff = DM::View("STUFF", DM::FACE, DM::WRITE);

	construction_side = DM::View("construction_side", DM::FACE, DM::WRITE);

	std::vector<DM::View> datastream;
	datastream.push_back(parcels);
	datastream.push_back(buildings);
	datastream.push_back(splits);
	datastream.push_back(stuff);
	datastream.push_back(construction_side);

	this->addData("city", datastream);
}


void MelbourneStyleBuilding::run()
{
	DM::System * city = this->getData("city");

	mforeach (DM::Component * cmp, city->getAllComponentsInView(parcels)) {
		DM::Face * parcel = dynamic_cast<DM::Face*>(cmp);

		typedef std::pair<std::string, split_val> splitpair;

		std::vector<splitpair> splits;
		splits.push_back(splitpair("front",split_val("5", true)));
		splits.push_back(splitpair("front",split_val("5", true)));
		splits.push_back(splitpair("construction_side",split_val("x", false)));
		splits.push_back(splitpair("back",split_val("3", true)));

		std::vector<DM::Face*> results = this->spiltFace(city, parcel, splits);

		std::vector<splitpair> splits_1;
		splits_1.push_back(splitpair("side",split_val("3", true)));
		splits_1.push_back(splitpair("drive_way",split_val("5", true)));
		splits_1.push_back(splitpair("construction_side",split_val("x", false)));
		splits_1.push_back(splitpair("side",split_val("3", false)));


		foreach (DM::Face * f_r, this->spiltFace(city, parcel, splits_1,false)) {
			results.push_back(f_r);
		}
		foreach (DM::Face * f_r, results){
			for (uint i = 1; i < f_r->getNodePointers().size(); i++) {
				city->addEdge(f_r->getNodePointers()[i-1], f_r->getNodePointers()[i], this->splits);
			}
			city->addEdge(f_r->getNodePointers()[f_r->getNodePointers().size()-1], f_r->getNodePointers()[0], this->splits);
		}

		//Now find all construction sites and intersect them
		std::vector<DM::Face*> construction_sides;

		foreach (DM::Face * f, results) {
			if (f->getAttribute("type")->getString() == "construction_side")
				construction_sides.push_back(f);
		}

		//intersect faces
		std::vector<DM::Face*> intersectedFaces;
		intersectedFaces.push_back(construction_sides[0]);
		for (int i = 1; i < construction_sides.size(); i++) {
			std::vector<DM::Face*> intersectedFaces_tmp;
			foreach (DM::Face * i_f, intersectedFaces) {
				foreach (DM::Face * r_f, DM::CGALGeometry::BoolOperationFace(city, i_f, construction_sides[i], DM::CGALGeometry::OP_INTERSECT)) {
					intersectedFaces_tmp.push_back(r_f);
				}
			}
			intersectedFaces = intersectedFaces_tmp;
		}
		foreach (DM::Face * f, intersectedFaces) {
			city->addComponentToView(f, construction_side);
		}
	}

}


bool MelbourneStyleBuilding::isStreetNode(DM::Node * n) {
	if(n->getAttribute("street_side")->getDouble() > 0)
		return true;
	return false;
}

std::vector<std::pair<std::string, double> > MelbourneStyleBuilding::translateSwitch(double l, std::vector<std::pair<std::string, split_val> > splits)
{
	std::vector<std::pair<std::string, double> > tranlasted_splits;
	int x_counter = 0;
	double length_used = 0;
	typedef std::pair<std::string, split_val>  type_val;
	foreach (type_val vals, splits) {
		split_val s_val = vals.second;
		double val = -1;
		if (s_val.second) {
			val = QString::fromStdString(s_val.first).toDouble();
			length_used+=val;
		} else {
			x_counter++;
		}
		tranlasted_splits.push_back(std::pair<std::string, double>(vals.first, val));
	}
	double dx = (l - length_used)/(double)x_counter;

	for (int i = 0; i < tranlasted_splits.size(); i++) {
		std::pair<std::string, double> val = tranlasted_splits[i];
		if (val.second < 0) {
			val.second = dx;
		}
		tranlasted_splits[i] = val;
	}

	return tranlasted_splits;
}

std::vector<DM::Face *> MelbourneStyleBuilding::spiltFace(DM::System *sys, DM::Face *f, std::vector<std::pair<std::string, split_val> > split_rules, bool street_side)
{
	std::vector<DM::Face * > results;
	std::vector<DM::Node> box;
	std::vector<double> size;

	std::vector<DM::Edge*> streetEdges;

	std::vector<DM::Node *> nodesToCheck = f->getNodePointers();
	nodesToCheck.push_back(nodesToCheck[0]);
	for (int i = 1; i < nodesToCheck.size(); i++){
		if (isStreetNode(nodesToCheck[i-1]) && isStreetNode(nodesToCheck[i])){
			streetEdges.push_back(sys->addEdge(nodesToCheck[i-1], nodesToCheck[i]));
		}
	}


	double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, f), box, size);
	alpha = this->whereIsStreet(box, streetEdges);


	DM::Face * bb;
	std::vector<DM::Node*> l_bb;
	foreach (DM::Node  n, box) {
		l_bb.push_back(sys->addNode(n));
	}
	l_bb.push_back(l_bb[0]);

	bb = sys->addFace(l_bb);

	DM::Node center = DM::CGALGeometry::CaclulateCentroid2D(bb);

	double x_c = center.getX();
	double y_c = center.getY();

	double l = size[0];
	double w = size[1];

	double current_x = 0;

	double r_l = l;
	if (!street_side)
		r_l = w;
	std::vector<std::pair<std::string, double> > splits = this->translateSwitch(r_l, split_rules);

	for (unsigned i = 0; i < splits.size(); i++) {
		std::string type = splits[i].first;
		double dx = splits[i].second;
		QRectF r1;
		if (street_side)
			r1 = QRectF(-l/2.+ current_x,  -w/2-10, dx ,w+10);
		else // from left
			r1 = QRectF(-l/2. - 10, -w/2  + current_x, l + 10 ,+ dx);

		//Rotate Intersection Rect
		QTransform t;
		t.rotate(alpha);

		DM::Logger(DM::Debug) <<  "alpha " << alpha;

		QPolygonF intersection_tmp = t.map(r1);
		QTransform t1;

		//Move Intersection Rect
		t1.translate(x_c, y_c);
		QPolygonF intersection = t1.map(intersection_tmp);

		std::vector<DM::Node* > intersection_p;
		for (int i = 0; i < intersection.size()-1; i++) {
			QPointF & p = intersection[i];
			intersection_p.push_back(sys->addNode(DM::Node(p.x(), p.y(), 0)));
		}

		DM::Face * bb = sys->addFace(intersection_p);

		std::vector<DM::Face *> intersected_faces = DM::CGALGeometry::IntersectFace(sys, f, bb);

		if (intersected_faces.size() == 0) {
			DM::Logger(DM::Warning) << "Advanced parceling createSubdevision interseciton failed";
			continue;
		}

		current_x +=  dx;

		foreach (DM::Face * f, intersected_faces) {
			results.push_back(f);
			f->addAttribute("type", type);
		}
	}

	return results;
}

double MelbourneStyleBuilding::whereIsStreet(std::vector<DM::Node> bb, std::vector<DM::Edge*> streetEdges)
{
	if (streetEdges.size() == 0) {
		DM::Logger(DM::Error) << "No Streets found";
		return 0;
	}

	std::vector<std::pair<int,int> > indizes;
	foreach (DM::Edge * e, streetEdges) {
		std::pair<int, int> bb_nodes;
		std::vector<DM::Node*> nodes;
		nodes.push_back(e->getStartNode());
		nodes.push_back(e->getEndNode());

		for (int k = 0; k < 2; k++) {
			int index = 0;
			double current_l = TBVectorData::calculateDistance(nodes[k], &bb[0]);
			for (uint i = 1; i < bb.size(); i++) {
				double l = TBVectorData::calculateDistance(nodes[k], &bb[i]);
				if (l < current_l) {
					index = i;
					current_l = l;
				}
			}
			if (k == 0)
				bb_nodes.first = index;
			if (k == 1)
				bb_nodes.second = index;
		}
		indizes.push_back(bb_nodes);
	}
	std::pair<int,int> access_street;
	access_street = indizes[0];
	double l_current = TBVectorData::calculateDistance( &bb[indizes[0].first], &bb[indizes[0].second]);
	for (int i = 1; i < indizes.size(); i++) {
		double l = TBVectorData::calculateDistance( &bb[indizes[i].first],  &bb[indizes[i].second]);
		if (l < l_current) {
			access_street = indizes[i];
			l_current = l;
		}
	}

	DM::Logger(DM::Debug) << bb[access_street.first].getX() << " " << bb[access_street.first].getY();
	DM::Logger(DM::Debug) << bb[access_street.second].getX() << " " << bb[access_street.second].getY();
	double angel = TBVectorData::AngelBetweenVectors(DM::Node(0,-1,0), bb[access_street.second]-bb[access_street.first])*180./M_PI;

	DM::Node n = TBVectorData::NormalVector(DM::Node(0,-1,0), bb[access_street.second]-bb[access_street.first]); //directcos is 0 in + and - y direction.
	if (n.getZ() < 0) {
		angel+=180;
	}

	DM::Logger(DM::Debug) << "direct angel " << angel;

	return angel;
}


#include "melbournestylebuilding.h"



#include <dmsystem.h>
#include <dmface.h>
#include <QTransform>
#include <cgalgeometry.h>
#include <tbvectordata.h>
#include <spatialsearchnearestnodes.h>

DM_DECLARE_NODE_NAME(MelbourneStyleBuilding, DAnCE4Water)



MelbourneStyleBuilding::MelbourneStyleBuilding()
{
	parcels = DM::View("PARCEL", DM::FACE, DM::READ);
	buildings = DM::View("BUILDING", DM::EDGE, DM::WRITE);


	std::vector<DM::View> datastream;
	datastream.push_back(parcels);
	datastream.push_back(buildings);

	this->addData("city", datastream);
}


void MelbourneStyleBuilding::run()
{
	DM::System * city = this->getData("city");

	mforeach (DM::Component * cmp, city->getAllComponentsInView(parcels)) {
		DM::Face * parcel = dynamic_cast<DM::Face*>(cmp);
	}
}

std::vector<DM::Face *> MelbourneStyleBuilding::spiltFace(DM::System *sys, DM::Face *f, std::vector<std::pair<std::string, double> > splits, bool street_side)
{

	std::vector<DM::Face * > results;
	std::vector<DM::Node> box;
	std::vector<double> size;

	std::vector<DM::Node *> street_nodes;
	foreach(DM::Node * n, f->getNodePointers()) {
		if (n->getAttribute("street_side")->getDouble() > 0){
			street_nodes.push_back(n);
		}
	}

	double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, f), box, size);
	this->whereIsStreet(box, street_nodes);

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

		DM::Logger(DM::Debug) <<  alpha;

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

double MelbourneStyleBuilding::whereIsStreet(std::vector<DM::Node> bb, std::vector<DM::Node*> streetNodes)
{
	std::vector<uint> indizes;
	foreach (DM::Node * n, streetNodes) {
		int index = 0;
		double current_l = TBVectorData::calculateDistance(n, &bb[0]);
		for (uint i = 1; i < bb.size(); i++) {
			double l = TBVectorData::calculateDistance(n, &bb[i]);
			if (l < current_l) {
				index = i;
				current_l = l;
			}
		}
		indizes.push_back(index);
	}

	foreach (int i, indizes) {
		DM::Logger(DM::Debug) << i;
	}
	DM::Logger(DM::Debug) << bb[indizes[0]].getX() << " " << bb[indizes[0]].getY();
	DM::Logger(DM::Debug) << bb[indizes[1]].getX() << " " << bb[indizes[1]].getY();
	double angel = TBVectorData::AngelBetweenVectors(DM::Node(1,0,0), bb[indizes[1]]-bb[indizes[0]])*180./M_PI;
	DM::Logger(DM::Debug) << "angel " << angel;

	return angel;
}


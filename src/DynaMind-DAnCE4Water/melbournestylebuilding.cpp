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
	splits = DM::View("SPITS", DM::EDGE, DM::WRITE);



	std::vector<DM::View> datastream;
	datastream.push_back(parcels);
	datastream.push_back(buildings);
	datastream.push_back(splits);

	this->addData("city", datastream);
}


void MelbourneStyleBuilding::run()
{
	DM::System * city = this->getData("city");

	mforeach (DM::Component * cmp, city->getAllComponentsInView(parcels)) {
		DM::Face * parcel = dynamic_cast<DM::Face*>(cmp);

		typedef std::pair<std::string, double> splitpair;
		std::vector<splitpair> splits;
		splits.push_back(splitpair("front",2));
		splits.push_back(splitpair("front",3));
		std::vector<DM::Face*> results = this->spiltFace(city, parcel, splits);


		foreach (DM::Face * f_r, results) {
			for (uint i = 1; i < f_r->getNodePointers().size(); i++) {
				city->addEdge(f_r->getNodePointers()[i-1], f_r->getNodePointers()[i], this->splits);
			}
			city->addEdge(f_r->getNodePointers()[f_r->getNodePointers().size()-1], f_r->getNodePointers()[0], this->splits);
		}

		std::vector<splitpair> splits_1;
		splits_1.push_back(splitpair("front",5));
		results = this->spiltFace(city, parcel, splits_1,false);


		foreach (DM::Face * f_r, results) {
			for (uint i = 1; i < f_r->getNodePointers().size(); i++) {
				city->addEdge(f_r->getNodePointers()[i-1], f_r->getNodePointers()[i], this->splits);
			}
			city->addEdge(f_r->getNodePointers()[f_r->getNodePointers().size()-1], f_r->getNodePointers()[0], this->splits);
		}

	}
}

bool MelbourneStyleBuilding::isStreetNode(DM::Node * n) {
	if(n->getAttribute("street_side")->getDouble() > 0)
		return true;
	return false;
}

std::vector<DM::Face *> MelbourneStyleBuilding::spiltFace(DM::System *sys, DM::Face *f, std::vector<std::pair<std::string, double> > splits, bool street_side)
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


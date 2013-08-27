#include "unittestsdance4water.h"

#include <dm.h>

#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>

#include <cgalgeometry_p.h>
#include <cgalgeometry.h>

DM::Face * addParcel(DM::System* sys, DM::View v)
{
	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(25,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(25,50,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,50,0));

	n1->addAttribute("street_side", 1);
	n2->addAttribute("street_side", 1);

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);

	DM::Face * f1 = sys->addFace(nodes, v);

	return f1;
}

TEST_F(UnitTestsDAnCE4Water, TestOffset) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);

	DM::System * sys = new DM::System();

	DM::View parcel("PARCEL", DM::FACE, DM::WRITE);

	DM::Face * p = addParcel(sys, parcel);

	typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
	typedef K::Point_2                                          Point_2;
	typedef CGAL::Polygon_2<K>                                  Polygon_2;

	Polygon_2 poly1;

	foreach(DM::Node *n,  p->getNodePointers()){
		poly1.push_back(Point_2(n->getX(), n->getY()));
	}

	DM::Logger(DM::Debug) << poly1.area();

	//Intersection Polygon

	std::vector<DM::Node * > offsetPoly;
	foreach(DM::Node *n,  p->getNodePointers()){
		if (n->getAttribute("street_side")->getDouble() > 0 ) {
			offsetPoly.push_back(n);
		}
	}
	DM::Logger(DM::Debug) << offsetPoly.size();

	//Create intersection Polygon



	//DM::CGALGeometry::CalculateMinBoundingBox()






}


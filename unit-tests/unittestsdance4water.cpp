#include "unittestsdance4water.h"

#include <dm.h>

#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>

#include <tbvectordata.h>

#include <cgalgeometry_p.h>
#include <cgalgeometry.h>

#include <melbournestylebuilding.h>



DM::Face * addParcel(DM::System* sys, DM::View v)
{
	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(25,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(25,50,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,50,0));

	n1->addAttribute("street_side", 1);
	n4->addAttribute("street_side", 1);

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);

	DM::Face * f1 = sys->addFace(nodes, v);

	return f1;
}

TEST_F(UnitTestsDAnCE4Water, TestOffset) {
	typedef std::pair<std::string, double> split_type;
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);

	DM::System * sys = new DM::System();

	DM::View parcel("PARCEL", DM::FACE, DM::WRITE);

	DM::Face * p = addParcel(sys, parcel);

	MelbourneStyleBuilding mb;

	std::vector<split_type> splits;
	splits.push_back(split_type("street",1));
	splits.push_back(split_type("street1",2));
	splits.push_back(split_type("street2",3));

	std::vector<DM::Face*> ress = mb.spiltFace(sys, p, splits);
	EXPECT_EQ(3,ress.size());


	ress = mb.spiltFace(sys, p, splits, false); //90 degree
	EXPECT_EQ(3,ress.size());


	//mb.test();
}




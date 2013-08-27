#include "unittestsdance4water.h"

#include <dm.h>

#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>

DM::Face * addRectangle(DM::System* sys, DM::View v)
{
	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

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
	DM::Logger(DM::Debug) << "Hello test";


}


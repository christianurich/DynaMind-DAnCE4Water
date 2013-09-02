#include "unittestsdance4water.h"

#include <dm.h>

#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>

#include <tbvectordata.h>

#include <cgalgeometry_p.h>
#include <cgalgeometry.h>
#include <dmgeomtry3d.h>
#include <dmcarvewrapper.h>
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

TEST_F(UnitTestsDAnCE4Water,createMesh){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();

	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);

	DM::Face * f = sys->addFace(nodes);
	std::vector<DM::Face*> faces;
	faces.push_back(f);
	std::vector<DM::Face*> faces_return = DM::DMCarveWrapper::MeshToFace(sys, DM::DMCarveWrapper::FacesToMesh(faces));

	EXPECT_EQ(faces.size(), faces_return.size());
	EXPECT_DOUBLE_EQ(DM::CGALGeometry::CalculateArea2D(faces[0]), DM::CGALGeometry::CalculateArea2D(faces_return[0]));
}

TEST_F(UnitTestsDAnCE4Water,createMesh3D){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();

	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

	std::vector<DM::Node * > nodesA;
	nodesA.push_back(n1);
	nodesA.push_back(n2);
	nodesA.push_back(n3);
	nodesA.push_back(n4);

	std::vector<DM::Face*> facesA = TBVectorData::ExtrudeFace(sys, DM::View("", DM::FACE, DM::WRITE), nodesA, 1);
	facesA.push_back(sys->addFace(nodesA));

	std::vector<DM::Face* > results_face = DM::DMCarveWrapper::MeshToFace(sys, DM::DMCarveWrapper::FacesToMesh(facesA));
	EXPECT_EQ(results_face.size(), 6);

}

TEST_F(UnitTestsDAnCE4Water,IntersectMesh3D){


	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::System * sys = new DM::System();

	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

	std::vector<DM::Node * > nodesA;
	nodesA.push_back(n1);
	nodesA.push_back(n2);
	nodesA.push_back(n3);
	nodesA.push_back(n4);

	std::vector<DM::Face*> facesA = TBVectorData::ExtrudeFace(sys, DM::View("", DM::FACE, DM::WRITE), nodesA, 1);

	std::reverse(nodesA.begin(),nodesA.end());
	facesA.push_back(sys->addFace(nodesA));
	DM::Logger(DM::Debug) << "faces" << facesA.size();

	n1 = sys->addNode(DM::Node(0,0,0));
	n2 = sys->addNode(DM::Node(0.5,0,0));
	n3 = sys->addNode(DM::Node(0.5,0.5,0));
	n4 = sys->addNode(DM::Node(0,0.5,0));

	std::vector<DM::Node * > nodesB;
	nodesB.push_back(n1);
	nodesB.push_back(n2);
	nodesB.push_back(n3);
	nodesB.push_back(n4);

	std::vector<DM::Face*> facesB = TBVectorData::ExtrudeFace(sys, DM::View("", DM::FACE, DM::WRITE), nodesB, 0.5);

	std::reverse(nodesB.begin(),nodesB.end());
	facesB.push_back(sys->addFace(nodesB));

	std::vector<DM::Face* > results_face = DM::DMGeomtry3D::BoolOperation(sys, facesA, facesB, DM::CGALGeometry::OP_INTERSECT);
	EXPECT_EQ(results_face.size(), 6);

	std::vector<DM::Face* > results_face2= DM::DMGeomtry3D::BoolOperation(sys, facesA, facesB, DM::CGALGeometry::OP_DIFFERENCE);
	EXPECT_EQ(results_face2.size(), 9);
}





#include "unittestsdance4water.h"

#include <dm.h>
#include <dmedge.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>

#include <tbvectordata.h>

#include <cgalgeometry_p.h>
#include <cgalgeometry.h>
#include <dmgeomtry3d.h>
#include <dmcarvewrapper.h>
#include <melbournestylebuilding.h>

#include <QTransform>

DM::Face * addParcel(DM::System* sys, double alpha, DM::View v)
{


	QRectF r1 (0,  0, 25, 50);

	//Rotate Intersection Rect
	QTransform t;
	t.rotate(alpha);
	QPolygonF r2 = t.map(r1);

	std::vector<DM::Node* > parcel_nodes;
	for (int i = 0; i < r2.size()-1; i++) {
		QPointF & p = r2[i];
		parcel_nodes.push_back(sys->addNode(DM::Node(p.x(), p.y(), 0)));
	}

	DM::Face * parcel = sys->addFace(parcel_nodes, v);

	parcel->getNodePointers()[0]->addAttribute("street_side", 1);
	parcel->getNodePointers()[1]->addAttribute("street_side", 1);


	return parcel;
}

TEST_F(UnitTestsDAnCE4Water, crave_test_tri) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();
	DM::Node * n0 = sys->addNode(336794.977670,5806893.361093,0.000000);
	DM::Node * n1 = sys->addNode(336794.977670,5806893.361093,3.000000);
	DM::Node * n2 = sys->addNode(336795.382326,5806896.474930,3.000000);
	DM::Node * n3 = sys->addNode(336795.382326,5806896.474930,0.000000);
	DM::Node * n4 = sys->addNode(336806.877608,5806891.814649,0.000000);
	DM::Node * n5 = sys->addNode(336806.877608,5806891.814649,3.000000);
	DM::Node * n6 = sys->addNode(336807.149281,5806893.905181,0.000000);
	DM::Node * n7 = sys->addNode(336807.149281,5806893.905181,3.000000);
	DM::Node * n8 = sys->addNode(336807.282263,5806894.928486,0.000000);
	DM::Node * n9 = sys->addNode(336807.282263,5806894.928486,3.000000);

	std::vector<DM::Node*> f1;
	f1.push_back(n0);
	f1.push_back(n1);
	f1.push_back(n2);
	f1.push_back(n3);

	std::vector<DM::Node*> f2;
	f2.push_back(n4);
	f2.push_back(n5);
	f2.push_back(n1);
	f2.push_back(n0);

	std::vector<DM::Node*> f3;
	f3.push_back(n6);
	f3.push_back(n7);
	f3.push_back(n5);
	f3.push_back(n4);

	std::vector<DM::Node*> f4;
	f4.push_back(n8);
	f4.push_back(n9);
	f4.push_back(n7);
	f4.push_back(n6);

	std::vector<DM::Node*> f5;
	f5.push_back(n3);
	f5.push_back(n2);
	f5.push_back(n9);
	f5.push_back(n8);

	std::vector<DM::Node*> f6;
	f6.push_back(n2);
	f6.push_back(n1);
	f6.push_back(n5);
	f6.push_back(n7);
	f6.push_back(n9);

	std::vector<DM::Node*> f7;
	f7.push_back(n8);
	f7.push_back(n6);
	f7.push_back(n4);
	f7.push_back(n0);
	f7.push_back(n3);

	std::vector<DM::Face*> faces;
	faces.push_back( sys->addFace(f1));
	faces.push_back( sys->addFace(f2));
	faces.push_back( sys->addFace(f3));
	faces.push_back( sys->addFace(f4));
	faces.push_back( sys->addFace(f5));
	faces.push_back( sys->addFace(f6));
	faces.push_back( sys->addFace(f7));

	for (int i = 0; i < 1; i++)
	DM::DMGeomtry3D::SurfaceTriangulation(sys,faces);

}

//TEST_F(UnitTestsDAnCE4Water, testStreetSide) {
//	ostream *out = &cout;
//	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
//	DM::System * sys = new DM::System();

//	for (double alpha = 0; alpha < 360; alpha += 30) {
//		DM::Face * parcel = addParcel(sys, alpha, DM::View("PARCEL", DM::FACE, DM::WRITE));
//		MelbourneStyleBuilding mb;

//		std::vector<DM::Node>  bb;
//		std::vector<double> size;
//		std::vector<DM::Edge*> streets =  mb.findStreets(sys, parcel);
//		EXPECT_EQ(streets.size(), 1);
//		double length = TBVectorData::calculateDistance(streets[0]->getStartNode(), streets[0]->getEndNode());
//		EXPECT_DOUBLE_EQ(length, 25);
//		double a = DM::CGALGeometry::CalculateMinBoundingBox(parcel->getNodePointers(), bb, size);

//		DM::Logger(DM::Standard) << "alpha " << alpha <<"/" << a << "/" <<mb.whereIsStreet(bb, streets);

//		EXPECT_TRUE(fabs(alpha - mb.whereIsStreet(bb, streets)) < 0.001);

//	}


//}

//TEST_F(UnitTestsDAnCE4Water,createMesh){
//	ostream *out = &cout;
//	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
//	DM::System * sys = new DM::System();

//	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
//	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
//	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
//	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

//	std::vector<DM::Node * > nodes;
//	nodes.push_back(n1);
//	nodes.push_back(n2);
//	nodes.push_back(n3);
//	nodes.push_back(n4);

//	DM::Face * f = sys->addFace(nodes);
//	std::vector<DM::Face*> faces;
//	faces.push_back(f);
//	std::vector<DM::Face*> faces_return = DM::DMCarveWrapper::MeshToFace(sys, DM::DMCarveWrapper::FacesToMesh(faces));

//	EXPECT_EQ(faces.size(), faces_return.size());
//	EXPECT_DOUBLE_EQ(DM::CGALGeometry::CalculateArea2D(faces[0]), DM::CGALGeometry::CalculateArea2D(faces_return[0]));
//}

//TEST_F(UnitTestsDAnCE4Water,createMesh3D){
//	ostream *out = &cout;
//	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
//	DM::System * sys = new DM::System();

//	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
//	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
//	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
//	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

//	std::vector<DM::Node * > nodesA;
//	nodesA.push_back(n1);
//	nodesA.push_back(n2);
//	nodesA.push_back(n3);
//	nodesA.push_back(n4);

//	std::vector<DM::Face*> facesA = TBVectorData::ExtrudeFace(sys, DM::View("", DM::FACE, DM::WRITE), nodesA, 1);
//	facesA.push_back(sys->addFace(nodesA));

//	std::vector<DM::Face* > results_face = DM::DMCarveWrapper::MeshToFace(sys, DM::DMCarveWrapper::FacesToMesh(facesA));
//	EXPECT_EQ(results_face.size(), 6);

//}

//TEST_F(UnitTestsDAnCE4Water,IntersectMesh3D){


//	ostream *out = &cout;
//	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
//	DM::System * sys = new DM::System();

//	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
//	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
//	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
//	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

//	std::vector<DM::Node * > nodesA;
//	nodesA.push_back(n1);
//	nodesA.push_back(n2);
//	nodesA.push_back(n3);
//	nodesA.push_back(n4);

//	std::vector<DM::Face*> facesA = TBVectorData::ExtrudeFace(sys, DM::View("", DM::FACE, DM::WRITE), nodesA, 1);

//	std::reverse(nodesA.begin(),nodesA.end());
//	facesA.push_back(sys->addFace(nodesA));
//	DM::Logger(DM::Debug) << "faces" << facesA.size();

//	n1 = sys->addNode(DM::Node(0,0,0));
//	n2 = sys->addNode(DM::Node(0.5,0,0));
//	n3 = sys->addNode(DM::Node(0.5,0.5,0));
//	n4 = sys->addNode(DM::Node(0,0.5,0));

//	std::vector<DM::Node * > nodesB;
//	nodesB.push_back(n1);
//	nodesB.push_back(n2);
//	nodesB.push_back(n3);
//	nodesB.push_back(n4);

//	std::vector<DM::Face*> facesB = TBVectorData::ExtrudeFace(sys, DM::View("", DM::FACE, DM::WRITE), nodesB, 0.5);

//	std::reverse(nodesB.begin(),nodesB.end());
//	facesB.push_back(sys->addFace(nodesB));

//	std::vector<DM::Face* > results_face = DM::DMGeomtry3D::BoolOperation(sys, facesA, facesB, DM::CGALGeometry::OP_INTERSECT);
//	EXPECT_EQ(results_face.size(), 6);

//	std::vector<DM::Face* > results_face2= DM::DMGeomtry3D::BoolOperation(sys, facesA, facesB, DM::CGALGeometry::OP_DIFFERENCE);
//	EXPECT_EQ(results_face2.size(), 9);
//}





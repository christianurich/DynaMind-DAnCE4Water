#include "melbournestylebuilding.h"



#include <dmsystem.h>
#include <dmface.h>

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

std::vector<DM::Face *> MelbourneStyleBuilding::spiltFace(DM::System *sys, DM::Face *f, std::vector<double> splits)
{
	/**std::vector<DM::Node> box;
	std::vector<double> size;

	double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, f), box, size);

	DM::Face * bb;
	std::vector<DM::Node*> l_bb;
	foreach (DM::Node  n, box) {
		l_bb.push_back(sys->addNode(n));
	}
	l_bb.push_back(l_bb[0]);

	bb = sys->addFace(l_bb);

	DM::Node center = DM::CGALGeometry::CalculateCentroid(sys, bb);

	double x_c = center.getX();
	double y_c = center.getY();


	double l = size[0];
	double w = size[1];

	double current_x = 0;
	double current_y = 0;

	for (int i = 0; i < splits.size(); i++) {

		QRectF r1 (-l/2.+ i*l/(double)elements,  -w/2-10,splits,w+10);

		//Rotate Intersection Rect
		QTransform t;
		t.rotate(alpha);

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

		DM::Face * bb = sys->addFace(intersection_p, bbs);

		bb->addAttribute("generation", gen);
		std::vector<DM::Face *> intersected_faces = DM::CGALGeometry::IntersectFace(sys, f, bb);

		if (intersected_faces.size() == 0) {
			DM::Logger(DM::Warning) << "Advanced parceling createSubdevision interseciton failed";
			continue;
		}

	}**/
}


#ifndef MELBOURNESTYLEBUILDING_H
#define MELBOURNESTYLEBUILDING_H

#include <cgalgeometry_p.h>
#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT MelbourneStyleBuilding : public DM::Module
{
	DM_DECLARE_NODE(MelbourneStyleBuilding)
private:
		DM::View parcels;
		DM::View buildings;
		DM::View splits;
		DM::View stuff;

		bool isStreetNode(DM::Node *n);
public:
		MelbourneStyleBuilding();
		void run();

		void test(DM::System *sys);

		std::vector<DM::Face*> spiltFace(DM::System * sys, DM::Face * f, std::vector<std::pair<string, double> > splits, bool street_side = true);

		double whereIsStreet(std::vector<DM::Node> bb, std::vector<DM::Edge *> streetEdges);



};

#endif // MELBOURNESTYLEBUILDING_H

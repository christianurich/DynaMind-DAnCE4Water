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

public:
	MelbourneStyleBuilding();
	void run();

	std::vector<DM::Face*> spiltFace( DM::System * sys, DM::Face * f, std::vector<double> splits);



};

#endif // MELBOURNESTYLEBUILDING_H

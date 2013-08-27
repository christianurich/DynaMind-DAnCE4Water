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


#ifndef MELBOURNESTYLEBUILDING_H
#define MELBOURNESTYLEBUILDING_H


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
};

#endif // MELBOURNESTYLEBUILDING_H

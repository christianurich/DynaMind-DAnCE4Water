/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#ifndef MELBOURNESTYLEBUILDING_H
#define MELBOURNESTYLEBUILDING_H

#include <cgalgeometry_p.h>
#include <dmmodule.h>

typedef std::pair<std::string, bool> split_val;

class DM_HELPER_DLL_EXPORT MelbourneStyleBuilding : public DM::Module
{
	DM_DECLARE_NODE(MelbourneStyleBuilding)
private:
		DM::View parcels;
		DM::View buildings;
		DM::View splits;
		DM::View stuff;

		DM::View construction_side;

		bool isStreetNode(DM::Node *n);
public:
		MelbourneStyleBuilding();
		void run();

		void test(DM::System *sys);

		std::vector<DM::Face*> spiltFace(DM::System * sys, DM::Face * f, std::vector<std::pair<string, split_val> > splits, std::vector<DM::Edge *> street_edges, bool street_side = true);

		double whereIsStreet(std::vector<DM::Node> bb, std::vector<DM::Edge *> streetEdges);



		std::vector<std::pair<std::string, double> > translateSwitch(double l, std::vector<std::pair<std::string, split_val> > splits);
		void createBuilding(DM::System *sys, DM::Face *f, DM::View v);
		std::vector<DM::Face *> intersectFaces(DM::System * sys, std::vector<DM::Face *> faces, std::string type);
		std::vector<DM::Edge *> findStreets(DM::System* city, DM::Face* parcel);
};

#endif // MELBOURNESTYLEBUILDING_H

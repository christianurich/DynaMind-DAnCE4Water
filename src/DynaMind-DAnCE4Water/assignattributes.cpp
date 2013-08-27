/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#include "assignattributes.h"

DM_DECLARE_NODE_NAME(AssignAttributes, DAnCE4Water)

AssignAttributes::AssignAttributes()
{
	n_attriubte = "";
	this->addParameter("Attribute", DM::STRING, &this->n_attriubte);
	base = DM::View("Base", DM::FACE, DM::READ);
	base.getAttribute("area");
	intersects = DM::View("Intersect", DM::FACE, DM::READ);
	intersects.getAttribute("Percentage");
	intersects.getAttribute("Percentage_UUIDs");
	std::vector<DM::View> data;
	data.push_back(intersects);
	data.push_back(base);
	this->addData("City", data);
}
void AssignAttributes::init() {
	if (n_attriubte.empty())
		return;
	intersects.addAttribute(n_attriubte);
	base.getAttribute(n_attriubte);
	std::vector<DM::View> data;
	data.push_back(intersects);
	data.push_back(base);
	this->addData("City", data);
}

void AssignAttributes::run() {
	DM::System * city = this->getData("City");
	std::vector<std::string> uuids = city->getUUIDsOfComponentsInView(intersects);

	foreach (std::string uuid, uuids) {
		double val_tot = 0;
		DM::Face * f = city->getFace(uuid);
		std::vector<std::string> b_uuids = f->getAttribute("Percentage_UUIDs")->getStringVector();
		std::vector<double> b_pers = f->getAttribute("Percentage")->getDoubleVector();
		for (int i = 0; i < b_uuids.size(); i++) {
			DM::Face * bf = city->getFace(b_uuids[i]);
			double attr_val = bf->getAttribute(n_attriubte)->getDouble();
			val_tot += attr_val * b_pers[i];
		}
		f->addAttribute(n_attriubte,val_tot);
	}
}

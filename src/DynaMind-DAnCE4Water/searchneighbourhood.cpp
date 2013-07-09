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


#include "searchneighbourhood.h"

DM_DECLARE_NODE_NAME(SearchNeighbourhood, DAnCE4Water)

SearchNeighbourhood::SearchNeighbourhood()
{
    blockNames = "BLOCK";
    edgeNames = "BLOCK_EDGES";
    isEdgeBased = true;

    this->addParameter("BlockNames", DM::STRING, &this->blockNames);
    this->addParameter("EdgeNames", DM::STRING, &this->edgeNames);
    this->addParameter("EdgeBased", DM::BOOL, &this->isEdgeBased);


    std::vector<DM::View> datastream;
    datastream.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::MODIFY));

    this->addData("city", datastream);
}

void SearchNeighbourhood::init()
{
    std::vector<DM::View> datastream;

    vBlock = DM::View(blockNames, DM::COMPONENT, DM::READ);
    vBlock.addLinks("neighbourhood", vBlock);
    vBlock.addAttribute("neighbours");
    vEdge = DM::View(edgeNames, DM::EDGE, DM::READ);
    vEdge.getAttribute(vBlock.getName());

    datastream.push_back(vBlock);
    datastream.push_back(vEdge);

    this->addData("city", datastream);
}

void SearchNeighbourhood::run()
{
    DM::System * sys = this->getData("city");

    std::vector<std::string> edge_uuids = sys->getUUIDs(vEdge);
    foreach(std::string e_uuid, edge_uuids) {
        DM::Component * e = sys->getComponent(e_uuid);
        std::vector<DM::LinkAttribute> links =  e->getAttribute(vBlock.getName())->getLinks();
        foreach(DM::LinkAttribute f, links) {
            DM::Component * b = sys->getComponent(f.uuid);
            foreach(DM::LinkAttribute l, links) {
                if (b->getUUID() == l.uuid) continue;
                b->getAttribute("neighbourhood")->setLink(vBlock.getName(), l.uuid);
                b->addAttribute("neighbours", b->getAttribute("neighbours")->getDouble() + 1);
            }
        }
    }

}



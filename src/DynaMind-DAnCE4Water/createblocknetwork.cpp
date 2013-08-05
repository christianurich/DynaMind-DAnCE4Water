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

#include "createblocknetwork.h"

DM_DECLARE_NODE_NAME(CreateBlockNetwork, DAnCE4Water)

CreateBlockNetwork::CreateBlockNetwork()
{
    blockNames = "BLOCK";
    centerNames = "BLOCK_CENTER";
    edgeNames = "BLOCK_NETWORK";

    this->addParameter("BlockNames", DM::STRING, &this->blockNames);
    this->addParameter("CenterNames", DM::STRING, &this->centerNames);
    this->addParameter("EdgeNames", DM::STRING, &this->edgeNames);


    std::vector<DM::View> datastream;
    datastream.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::MODIFY));

    this->addData("city", datastream);
}

void CreateBlockNetwork::init()
{

    std::vector<DM::View> datastream;

    vBlock = DM::View(blockNames, DM::COMPONENT, DM::READ);
    vBlock.getAttribute("neighbourhood");
    vCenter = DM::View(centerNames, DM::NODE, DM::READ);

    vEdge = DM::View(edgeNames, DM::EDGE, DM::WRITE);

    datastream.push_back(vBlock);
    datastream.push_back(vCenter);
    datastream.push_back(vEdge);

    this->addData("city", datastream);

}

string CreateBlockNetwork::getHelpUrl()
{
    return "https://github.com/christianurich/DynaMind-DAnCE4Water/blob/master/doc/CreateBlockNetwork.md";
}

void CreateBlockNetwork::run()
{
    DM::System * city = this->getData("city");
    std::vector<std::string> uuids = city->getUUIDs(vBlock);
    std::set<std::pair<std::string, std::string > > connectedBlocks;

    foreach (std::string uuid, uuids) {
        DM::Component * block = city->getComponent(uuid);

        DM::LinkAttribute lCenter = block->getAttribute(vCenter.getName())->getLink();
        DM::Node * startNode = city->getNode(lCenter.uuid);

        //Link Center of current block with all other blocks linked to this baby
        foreach (DM::LinkAttribute lBlock, block->getAttribute("neighbourhood")->getLinks()) {
            DM::Component * nblock = city->getComponent(lBlock.uuid);
            DM::LinkAttribute lNeigh = nblock->getAttribute(vCenter.getName())->getLink();
            DM::Node * endNode = city->getNode(lNeigh.uuid);
            //Check if connected from other side
            std::pair<std::string, std::string> otherSide (endNode->getUUID(),startNode->getUUID());

            if (connectedBlocks.find(otherSide) != connectedBlocks.end()) continue;

            city->addEdge(startNode, endNode, vEdge);

            std::pair<std::string, std::string> thisSide (startNode->getUUID(),endNode->getUUID());
            connectedBlocks.insert(thisSide);
        }
    }

}





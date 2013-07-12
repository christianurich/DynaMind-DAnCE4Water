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

#include "gravitydrivennetwork.h"


DM_DECLARE_NODE_NAME(GravityDrivenNetwork, DAnCE4Water)

GravityDrivenNetwork::GravityDrivenNetwork()
{
    NameInputEdges = "BLOCK_NETWORK";
    NameInputNodes = "BLOCK_CENTER";
    NameOutputNetwork = "BLOCK_DRAINAGE_NETWORK";
    extendedSearch = 1;

    this->addParameter("NameInputEdges", DM::STRING, &this->NameInputEdges);
    this->addParameter("NameInputNodes", DM::STRING, &this->NameInputNodes);
    this->addParameter("NameOutputNetwork", DM::STRING, &this->NameOutputNetwork);
    this->addParameter("ExtendSerachRadius", DM::INT, &this->extendedSearch);

    std::vector<DM::View> datastream;
    datastream.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::MODIFY));
    this->addData("city", datastream);

}

void GravityDrivenNetwork::init()
{
    std::vector<DM::View> datastream;

    vInputEdges = DM::View(NameInputEdges, DM::EDGE, DM::READ);

    vInputNodes = DM::View(NameInputNodes, DM::NODE, DM::READ);
    vInputNodes.getAttribute("attractor");

    vOutputNetwork = DM::View(NameOutputNetwork, DM::EDGE, DM::WRITE);


    datastream.push_back(vInputEdges);
    datastream.push_back(vInputNodes);
    datastream.push_back(vOutputNetwork);

    this->addData("city", datastream);
}



void GravityDrivenNetwork::run()
{
    DM::System * city = this->getData("city");
    std::vector<std::string> n_uuids = city->getUUIDs(vInputNodes);
    std::vector<std::string> e_uuids = city->getUUIDs(vInputEdges);
    std::set<std::pair<DM::Node*, DM::Node* > > newconnections;

    /**
     * go downstream from all nodes as long as there is either
     * (a) no attrictive node left
     * (b) there is already an existing path
     */

    std::map<DM::Node * , std::set<DM::Node *> > startNodeList;

    foreach (std::string e_uuid, e_uuids) {
        DM::Edge * e = city->getEdge(e_uuid);
        std::set<DM::Node*> endPoints = startNodeList[e->getStartNode()];
        endPoints.insert(e->getEndNode());
        startNodeList[e->getStartNode()] = endPoints;

        //Since Graph is not directed
        endPoints = startNodeList[e->getEndNode()];
        endPoints.insert(e->getStartNode());
        startNodeList[e->getEndNode()] = endPoints;

    }


    foreach (std::string n_uuid, n_uuids) {
        //start node
        DM::Node * n = city->getNode(n_uuid);
        double currentAttr = n->getAttribute("attractor")->getDouble();
        while (n) {
            //get all connected nodes
            std::set<DM::Node *> connectedNodes = startNodeList[n];
            if (connectedNodes.size() == 0) {
                n = 0;
                continue;
            }
            DM::Node * nextNode = identifyNode(currentAttr, connectedNodes);

            if (!nextNode) {
                //extended search
                nextNode = extendSearchRadius(startNodeList,extendedSearch,  connectedNodes , currentAttr);
                if (!nextNode) {
                    n = 0;
                    continue;
                }
            }
            std::pair<DM::Node*, DM::Node*> con(n, nextNode);
            if (newconnections.find(con) != newconnections.end()) {
                n = 0;
                continue;
            }

            city->addEdge(n, nextNode, vOutputNetwork);
            newconnections.insert(con);


            n = nextNode;
            currentAttr = n->getAttribute("attractor")->getDouble();
        }


    }


}

string GravityDrivenNetwork::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-ToolBox/wiki/Gravitydrivennetwork";
}

DM::Node *  GravityDrivenNetwork::extendSearchRadius(std::map<DM::Node * , std::set<DM::Node *> >&  startNodeList, int radius, std::set<DM::Node * > & candidateNodes,  double currentAttr)
{

    for (int i = 0; i < radius; i++) {
        std::set<DM::Node * > newCandidates;
        foreach (DM::Node * n, candidateNodes) {
            foreach (DM::Node * c, startNodeList[n]) {
                newCandidates.insert(c);
            }
        }
        foreach (DM::Node * c, newCandidates) {
            candidateNodes.insert(c);
        }
        DM::Node * n = identifyNode(currentAttr, candidateNodes);
        if (n) {
            return n;
        }

    }
    return 0;
}

DM::Node * GravityDrivenNetwork::identifyNode( double currentAttr, const std::set<DM::Node *> & connectedNodes)
{
    DM::Node * newNode = 0;
    foreach (DM::Node * candidateNode, connectedNodes) {
        double candAttr = candidateNode->getAttribute("attractor")->getDouble();
        if (candAttr > currentAttr) {
            currentAttr = candAttr;
            newNode = candidateNode;
        }
    }
    return newNode;
}

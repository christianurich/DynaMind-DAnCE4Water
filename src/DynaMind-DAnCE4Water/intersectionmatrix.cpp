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

#include "intersectionmatrix.h"
#include "tbvectordata.h"
#include "math.h"
#include "QPolygonF"

DM_DECLARE_NODE_NAME(IntersectionMatrix, DAnCE4Water)

IntersectionMatrix::IntersectionMatrix()
{
    base = DM::View("Base", DM::FACE, DM::READ);
    base.getAttribute("area");
    intersects = DM::View("Intersect", DM::FACE, DM::READ);
    intersects.addLinks("Base", base);
    intersects.addAttribute("Percentage");
    intersects.addAttribute("Percentage_UUIDs");
    intersects.addAttribute("NumberOfInterSections");
    intersects.addAttribute("AreaIntersected");
    std::vector<DM::View> data;
    data.push_back(intersects);
    data.push_back(base);
    this->addData("City", data);
    gridSize = 1000;
}


QString IntersectionMatrix::createHash(int ix, int iy)
{
    QString key = QString::number(ix) + "|" +  QString::number(iy);

    return key;
}
void IntersectionMatrix::initSpatialMap() {
    std::vector<std::string> uuids = city->getUUIDsOfComponentsInView(base);
    foreach (std::string uuid, uuids) {
        Face * dmf = city->getFace(uuid);
        QPolygonF f(TBVectorData::FaceAsQPolgonF(city, dmf));

        double x1 = f.boundingRect().x();
        double x2 = f.boundingRect().x() +  f.boundingRect().width();
        int ix1 = (int) x1 / gridSize;
        int ix2 = (int) x2 / gridSize;

        double y1 = f.boundingRect().y();
        double y2 = f.boundingRect().y() + f.boundingRect().height();

        int iy1 = (int) y1 / gridSize;
        int iy2 = (int) y2 / gridSize;

        for (int i = ix1; i <= ix2; i++) {
            for (int j = iy1; j <= iy2; j++) {
                QString hash = this->createHash(i, j);
                if (!SpatialMap[hash])
                    SpatialMap[hash] = new std::vector<Face*>();
                SpatialMap[hash]->push_back(dmf);
            }
        }
    }
}

std::set<DM::Face*> IntersectionMatrix::getFacesInArea(DM::Face * dmf) {
    std::set<DM::Face*> ress;
    QPolygonF f(TBVectorData::FaceAsQPolgonF(city, dmf));

    double x1 = f.boundingRect().x();
    double x2 = f.boundingRect().x() +  f.boundingRect().width();
    int ix1 = (int) x1 / gridSize;
    int ix2 = (int) x2 / gridSize;

    double y1 = f.boundingRect().y();
    double y2 = f.boundingRect().y() + f.boundingRect().height();

    int iy1 = (int) y1 / gridSize;
    int iy2 = (int) y2 / gridSize;

    for (int i = ix1; i <= ix2; i++) {
        for (int j = iy1; j <= iy2; j++) {
            QString hash = this->createHash(i, j);
            if (!SpatialMap[hash])
                continue;
            std::vector<DM::Face*> * faces = SpatialMap[hash];
            foreach(DM::Face * f1, (*faces)) {
                ress.insert(f1);
            }
        }
    }

    return ress;
}

void IntersectionMatrix::run() {
    city = this->getData("City");
    initSpatialMap();
    std::vector<std::string> uuids = city->getUUIDsOfComponentsInView(intersects);

    foreach (std::string uuid, uuids) {
        Face * dmf = city->getFace(uuid);
        std::set<DM::Face*> neighbours = this->getFacesInArea(dmf);

        QPolygonF f = TBVectorData::FaceAsQPolgonF(city, dmf);

        int TrueIntersects = 0;
        double area_tot = 0;
        std::vector<std::string> linksnames;
        std::vector<double> percentages;
        foreach (Face * idmf , neighbours) {
            QPolygonF f_i = TBVectorData::FaceAsQPolgonF(city, idmf);
            QPolygonF f_delta = f.intersected(f_i);
            if (f_delta.size() < 3)
                continue;
            TrueIntersects++;
            DM::System tmp_sys;
            //Calculate Area
            std::vector<DM::Node*> nodes;
            foreach (QPointF p, f_delta) {
                Node * n =tmp_sys.addNode(p.x(), p.y(), 0);
                nodes.push_back(n);
            }
            Face * f_intersected = tmp_sys.addFace(nodes);

            double a_tmp = fabs(TBVectorData::calculateArea(&tmp_sys, f_intersected));
            area_tot += a_tmp;
            linksnames.push_back(idmf->getUUID());
            percentages.push_back(a_tmp/idmf->getAttribute("area")->getDouble());
        }



        dmf->addAttribute("NumberOfInterSections", TrueIntersects);
        dmf->addAttribute("AreaIntersected", area_tot);
        Attribute attr("Base");
        std::vector<LinkAttribute> linkAttributes;
        foreach (std::string l, linksnames) {
            LinkAttribute la;
            la.uuid = l;
            la.viewname = base.getName();
            linkAttributes.push_back(la);
        }
        attr.setLinks(linkAttributes);

        dmf->addAttribute(attr);


        Attribute attr_vals("Percentage");
        attr_vals.setDoubleVector(percentages);

        dmf->addAttribute(attr_vals);

        Attribute attr_names("Percentage_UUIDs");
        attr_names.setStringVector(linksnames);

        dmf->addAttribute(attr_names);
    }


    foreach (QString key, SpatialMap.keys()) {
        delete SpatialMap[key];
    }
    SpatialMap.clear();

}

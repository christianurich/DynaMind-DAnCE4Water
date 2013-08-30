#include <iostream>
#include <geometry.hpp>
#include <carve/csg.hpp>

int main(int argc, char *argv[])
{
	std::cout << "Hello World" << std::endl;

	carve::mesh::MeshSet<3> * a = makeCube(carve::math::Matrix::SCALE(2.0, 2.0, 2.0));
	carve::mesh::MeshSet<3> * b = makeCube(carve::math::Matrix::SCALE(2.0, 2.0, 2.0) *
										   carve::math::Matrix::TRANS(1.0, 1.0, 1.0));

	carve::csg::CSG::OP op = carve::csg::CSG::INTERSECTION;
	carve::csg::CSG csg;
	carve::mesh::MeshSet<3> *result  = csg.compute(a, b, op);

	for (carve::mesh::MeshSet<3>::face_iter i = result->faceBegin(); i != result->faceEnd(); ++i) {
		carve::mesh::MeshSet<3>::face_t *f = *i;

		for (carve::mesh::MeshSet<3>::face_t::edge_iter_t e = f->begin(); e != f->end(); ++e) {
			double x = (e->vert->v.x );
			double y = (e->vert->v.y );
			double z = (e->vert->v.z );

			std::cout << "\t" << x << "\t" << y << "\t" << z<< std::endl;
		}
	}
	delete a;
	delete b;

}

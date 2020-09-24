#include <vector>
#include <array>
#include <tuple>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "marchingcubes.h"

#include <pybind11/eigen.h>
#include <Eigen/Dense>

namespace py = pybind11;
using namespace std;
using namespace Eigen;

tuple<MatrixXd, MatrixXi>
marching_cubes(const py::EigenDRef<VectorXd> arr_flatten, const array<int, 3>& shape, double isovalue)
{
    // here we use EigenDRef as it does not make a copy caues it's just a refernce 
    // pybind11 seems not support pass by reference for std::vector
    array<int, 3> lower{0, 0, 0};
    array<int, 3> upper{shape[0]-1, shape[1]-1, shape[2]-1};
    int numx = upper[0] - lower[0] + 1;
    int numy = upper[1] - lower[1] + 1;
    int numz = upper[2] - lower[2] + 1;

    auto access_3d_arr = [&](int i, int j, int k){
        auto idx = i * shape[1] * shape[2] + j * shape[2] + k;
        return arr_flatten(idx);
    };

    vector<double> vertices;
    vector<int> polygons;

    mc::marching_cubes(lower, upper, numx, numy, numz, access_3d_arr, isovalue, vertices, polygons);
    MatrixXd V = Map<Matrix<double, Dynamic, Dynamic, RowMajor>>(vertices.data(), vertices.size()/3, 3);
    MatrixXi P = Map<Matrix<int, Dynamic, Dynamic, RowMajor>>(polygons.data(), polygons.size()/3, 3);
    return std::make_tuple(V, P);
}


PYBIND11_MODULE(mcube, m) {
    m.doc() = "python wrapper"; 
    m.def("marching_cube", &marching_cubes, "marching cube method");
}

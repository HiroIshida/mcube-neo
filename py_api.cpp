#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "marchingcubes.h"

namespace py = pybind11;
void marching_cube()
{
}

PYBIND11_MODULE(example, m) {
    m.doc() = "python wrapper"; 
    m.def("marching_cube", &marching_cube, "marching cube method");
}

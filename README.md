### mcube-neo
This implementation is based on [PyMCubes](https://github.com/pmneila/PyMCubes). The original implementation outputs only list of vertices and facets (V, F). mcube-neo additionary implements algorithm to find connected components of the mesh. So, in addtion to `(V, F)`, we can obtain the information of which facet to belong to which groups. The implementation that I added/modified is mainly be found in `marchingcubes.h`. Another difference from the original is that, we use [pybind11](https://github.com/pybind/pybind11) rather than pure python wrapper, which enables us to easily write a fast wrapper.

### installation and usage example
Installation will be done by the following commands:
```sh
git clone https://github.com/HiroIshida/mcube-neo
cd mcube-neo
git clone https://github.com/pybind/pybind11
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python)
# cmake .. -DPYTHON_EXECUTABLE=$(which python) -PYTHON_GLOBAL_SITE_PKG=ON # if want to install to global site package
make
make install
```

A simple usage example can be found in `test.py`. The following image is the output of the script. You can see that component is decomposed into different connected polygons, and plotted with different colors.
![imagetest](/figs/test.png)


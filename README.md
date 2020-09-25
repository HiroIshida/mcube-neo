### mcube-neo
This implementation is based on [PyMCubes](https://github.com/pmneila/PyMCubes). The original implementation outputs only list of vertices and facets (V, F). mcube-neo additionary implements algorithm to find connected components of the mesh. So, in addtion to `(V, F)`, we can obtain the information of which facet to belong to which groups. The implementation that I added/modified is mainly be found in `marchingcubes.h`. Another difference from the original is that, we use [pybind11](https://github.com/pybind/pybind11) rather than pure python wrapper, which enables us to easily write a fast wrapper.


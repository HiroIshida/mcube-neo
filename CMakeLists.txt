cmake_minimum_required(VERSION 3.4)
set(CMAKE_BUILD_TYPE Release)
project(pybind_test VERSION 0.1.0)

find_package(Eigen3 REQUIRED)
ADD_DEFINITIONS(-DEIGEN_NO_DEBUG)

include_directories(${EIGEN3_INCLUDE_DIR})
add_subdirectory(pybind11)
set(LIBRARY_NAME mcube)
pybind11_add_module(${LIBRARY_NAME} py_api.cpp marchingcubes.cpp)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c 
    "import site; import sys;sys.stdout.write(site.getsitepackages()[0])"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_INSTALL_DIR
    )
install(TARGETS ${LIBRARY_NAME} DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR})

message("destination site-packages:" ${PYTHON_SITE_PACKAGES_INSTALL_DIR})

// Copyright (c) 2012-2015, P. M. Neila

#ifndef _MARCHING_CUBES_H
#define _MARCHING_CUBES_H

#include <stddef.h>
#include <array>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <stack>
#include <algorithm>    

namespace mc
{


using namespace Eigen;
using namespace std;
extern int edge_table[256];
extern int triangle_table[256][16];

namespace private_
{

double mc_isovalue_interpolation(double isovalue, double f1, double f2,
    double x1, double x2);
size_t mc_add_vertex(double x1, double y1, double z1, double c2,
    int axis, double f1, double f2, double isovalue, std::vector<double>* vertices);
}

struct TableManager
{
    public:
        std::vector<std::vector<uint>> neighbor_table;
        std::vector<uint> neighbor_num_table;
        TableManager(int n1, int n2, int n3){
            int n_vert_max = n1*n2*n3;
            int n_neighbor_max = 8; // due to marching cube's property (don't proved yet, but emperically)

            std::vector<uint> neighbor_lst(n_neighbor_max, 0);
            neighbor_table = std::vector<std::vector<uint>>(n_vert_max, neighbor_lst);
            neighbor_num_table = std::vector<uint>(n_vert_max, 0);
        }

        void add_element(uint vert_idx, uint elem){
            int idx = neighbor_num_table[vert_idx];
            neighbor_table[vert_idx][idx] = elem;
            neighbor_num_table[vert_idx]++;
        }

        // just for debugging in python side
        void copy_elements(std::vector<std::vector<uint>>& neighbor){
            int vert_idx = 0;
            while(true){
                int neighbor_num = neighbor_num_table[vert_idx];
                if(neighbor_num == 0){
                    break;
                }
                auto start = neighbor_table[vert_idx].begin();
                auto last = start + neighbor_num;
                std::vector<uint> vec_new(start, last); 
                neighbor.push_back(vec_new);
                vert_idx++;
            }
        }

        std::vector<std::vector<uint>> connected_components(int n_vert, vector<int> polygons){
            std::vector<bool> isVisited(n_vert, false);

            auto make_group = [&](int idx_init){
                std::vector<uint> group;
                group.reserve(n_vert); // we know group will have n_vert elems at most
                std::stack<uint> Q;

                // initialize
                isVisited[idx_init] = true;
                group.push_back(idx_init);
                Q.push(idx_init);

                while(!Q.empty()){
                    auto idx_here = Q.top();
                    Q.pop();
                    for(int i=0; i<neighbor_num_table[idx_here]; i++){
                        auto face_idx = neighbor_table[idx_here][i];
                        for(int j=0; j<3; j++){
                            uint idx_next = polygons[face_idx * 3 + j];
                            if(!isVisited[idx_next]){
                                isVisited[idx_next] = true;
                                group.push_back(idx_next);
                                Q.push(idx_next);
                            }
                        }
                    }
                }
                return group;
            };

            auto first_false_idx = [&]() -> int{// return -1 if not found
                for(int idx=0; idx<n_vert; idx++){
                    if(!isVisited[idx])
                        return idx;
                }
                return -1;
            };

            std::vector<std::vector<uint>> groups;
            while(true){
                auto idx_start = first_false_idx();
                if(idx_start == -1){return groups;}
                groups.push_back(make_group(idx_start));
            }

        }
};


template<typename vector3, typename formula>
tuple<MatrixXd, MatrixXi, vector<vector<unsigned int>> > marching_cubes(const vector3& lower, const vector3& upper, int numx, int numy, int numz, formula f, double isovalue
    )
{

    using coord_type = typename vector3::value_type;
    using size_type = typename vector3::size_type;
    using namespace private_;

    /*
    // Some initial checks
    if(numx < 2 || numy < 2 || numz < 2)
        return;

    if(!std::equal(std::begin(lower), std::end(lower), std::begin(upper),
                   [](double a, double b)->bool {return a <= b;}))
        return;
        */

    TableManager tm(numx, numy, numz);

    // numx, numy and numz are the numbers of evaluations in each direction
    --numx; --numy; --numz;

    coord_type dx = (upper[0] - lower[0]) / static_cast<coord_type>(numx);
    coord_type dy = (upper[1] - lower[1]) / static_cast<coord_type>(numy);
    coord_type dz = (upper[2] - lower[2]) / static_cast<coord_type>(numz);

    const int num_shared_indices = 2 * (numy + 1) * (numz + 1);
    std::vector<size_type> shared_indices_x(num_shared_indices);
    std::vector<size_type> shared_indices_y(num_shared_indices);
    std::vector<size_type> shared_indices_z(num_shared_indices);
    auto _offset = [&](size_t i, size_t j, size_t k){return i*(numy+1)*(numz+1) + j*(numz+1) + k;};

    vector<double> vertices;
    vector<int> polygons;
    vector<vector<unsigned int>> neighbor_faces;

    for(int i=0; i<numx; ++i)
    {
        coord_type x = lower[0] + dx*i;
        coord_type x_dx = lower[0] + dx*(i+1);
        const int i_mod_2 = i % 2;
        const int i_mod_2_inv = (i_mod_2 ? 0 : 1);

        for(int j=0; j<numy; ++j)
        {
            coord_type y = lower[1] + dy*j;
            coord_type y_dy = lower[1] + dy*(j+1);

            double v[8];
            v[4] = f(x, y, lower[2]); v[5] = f(x_dx, y, lower[2]);
            v[6] = f(x_dx, y_dy, lower[2]); v[7] = f(x, y_dy, lower[2]);

            for(int k=0; k<numz; ++k)
            {
                coord_type z = lower[2] + dz*k;
                coord_type z_dz = lower[2] + dz*(k+1);

                v[0] = v[4]; v[1] = v[5];
                v[2] = v[6]; v[3] = v[7];
                v[4] = f(x, y, z_dz); v[5] = f(x_dx, y, z_dz);
                v[6] = f(x_dx, y_dy, z_dz); v[7] = f(x, y_dy, z_dz);

                unsigned int cubeindex = 0;
                for(int m=0; m<8; ++m)
                    if(v[m] <= isovalue)
                        cubeindex |= 1<<m;

                // Generate vertices AVOIDING DUPLICATES.

                int edges = edge_table[cubeindex];
                std::array<size_type, 12> indices;
                if(edges & 0x040)
                {
                    size_t index = mc_add_vertex(x_dx, y_dy, z_dz, x, 0, v[6], v[7], isovalue, &vertices);
                    indices[6] = index;
                    shared_indices_x[_offset(i_mod_2_inv, j+1, k+1)] = index;
                }
                if(edges & 0x020)
                {
                    size_t index = mc_add_vertex(x_dx, y, z_dz, y_dy, 1, v[5], v[6], isovalue, &vertices);
                    indices[5] = index;
                    shared_indices_y[_offset(i_mod_2_inv, j+1, k+1)] = index;
                }
                if(edges & 0x400)
                {
                    size_t index = mc_add_vertex(x_dx, y_dy, z, z_dz, 2, v[2], v[6], isovalue, &vertices);
                    indices[10] = index;
                    shared_indices_z[_offset(i_mod_2_inv, j+1, k+1)] = index;
                }

                if(edges & 0x001)
                {
                    if(j == 0 && k == 0)
                    {
                        size_t index = mc_add_vertex(x, y, z, x_dx, 0, v[0], v[1], isovalue, &vertices);
                        indices[0] = index;
                    }
                    else
                        indices[0] = shared_indices_x[_offset(i_mod_2_inv, j, k)];
                }
                if(edges & 0x002)
                {
                    if(k == 0)
                    {
                        size_t index = mc_add_vertex(x_dx, y, z, y_dy, 1, v[1], v[2], isovalue, &vertices);
                        indices[1] = index;
                        shared_indices_y[_offset(i_mod_2_inv, j+1, k)] = index;
                    }
                    else
                        indices[1] = shared_indices_y[_offset(i_mod_2_inv, j+1, k)];
                }
                if(edges & 0x004)
                {
                    if(k == 0)
                    {
                        size_t index = mc_add_vertex(x_dx, y_dy, z, x, 0, v[2], v[3], isovalue, &vertices);
                        indices[2] = index;
                        shared_indices_x[_offset(i_mod_2_inv, j+1, k)] = index;
                    }
                    else
                        indices[2] = shared_indices_x[_offset(i_mod_2_inv, j+1, k)];
                }
                if(edges & 0x008)
                {
                    if(i == 0 && k == 0)
                    {
                        size_t index = mc_add_vertex(x, y_dy, z, y, 1, v[3], v[0], isovalue, &vertices);
                        indices[3] = index;
                    }
                    else
                        indices[3] = shared_indices_y[_offset(i_mod_2, j+1, k)];
                }
                if(edges & 0x010)
                {
                    if(j == 0)
                    {
                        size_t index = mc_add_vertex(x, y, z_dz, x_dx, 0, v[4], v[5], isovalue, &vertices);
                        indices[4] = index;
                        shared_indices_x[_offset(i_mod_2_inv, j, k+1)] = index;
                    }
                    else
                        indices[4] = shared_indices_x[_offset(i_mod_2_inv, j, k+1)];
                }
                if(edges & 0x080)
                {
                    if(i == 0)
                    {
                        size_t index = mc_add_vertex(x, y_dy, z_dz, y, 1, v[7], v[4], isovalue, &vertices);
                        indices[7] = index;
                        shared_indices_y[_offset(i_mod_2, j+1, k+1)] = index;
                    }
                    else
                        indices[7] = shared_indices_y[_offset(i_mod_2, j+1, k+1)];
                }
                if(edges & 0x100)
                {
                    if(i == 0 && j == 0)
                    {
                        size_t index = mc_add_vertex(x, y, z, z_dz, 2, v[0], v[4], isovalue, &vertices);
                        indices[8] = index;
                    }
                    else
                        indices[8] = shared_indices_z[_offset(i_mod_2, j, k+1)];
                }
                if(edges & 0x200)
                {
                    if(j == 0)
                    {
                        size_t index = mc_add_vertex(x_dx, y, z, z_dz, 2, v[1], v[5], isovalue, &vertices);
                        indices[9] = index;
                        shared_indices_z[_offset(i_mod_2_inv, j, k+1)] = index;
                    }
                    else
                        indices[9] = shared_indices_z[_offset(i_mod_2_inv, j, k+1)];
                }
                if(edges & 0x800)
                {
                    if(i == 0)
                    {
                        size_t index = mc_add_vertex(x, y_dy, z, z_dz, 2, v[3], v[7], isovalue, &vertices);
                        indices[11] = index;
                        shared_indices_z[_offset(i_mod_2, j+1, k+1)] = index;
                    }
                    else
                        indices[11] = shared_indices_z[_offset(i_mod_2, j+1, k+1)];
                }

                int tri;
                int* triangle_table_ptr = triangle_table[cubeindex];
                for(int m=0; tri = triangle_table_ptr[m], tri != -1; ++m){
                    int idx_vertex = indices[tri];
                    polygons.push_back(idx_vertex);

                    int idx_polygon = (polygons.size() - 1)/ 3;
                    tm.add_element(idx_vertex, idx_polygon);
                }
            }
        }
    }
    auto groups = tm.connected_components(vertices.size()/3, polygons);
    MatrixXd V = Map<Matrix<double, Dynamic, Dynamic, RowMajor>>(vertices.data(), vertices.size()/3, 3);
    MatrixXi P = Map<Matrix<int, Dynamic, Dynamic, RowMajor>>(polygons.data(), polygons.size()/3, 3);

    return std::make_tuple(V, P, groups);
}

}

#endif // _MARCHING_CUBES_H

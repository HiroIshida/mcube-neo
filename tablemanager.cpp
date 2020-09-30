#include "tablemanager.h"
#include<iostream>

TableManager::TableManager(int n1, int n2, int n3)
{
    int n_vert_max = n1*n2*n3;
    int n_neighbor_max = 8; // due to marching cube's property (don't proved yet, but emperically)
    std::vector<uint> neighbor_lst(n_neighbor_max, 0);
    neighbor_table = std::vector<std::vector<uint>>(n_vert_max, neighbor_lst);
    neighbor_num_table = std::vector<uint>(n_vert_max, 0);
}

void TableManager::add_element(uint vert_idx, uint elem)
{
    int idx = neighbor_num_table[vert_idx];
    neighbor_table[vert_idx][idx] = elem;
    neighbor_num_table[vert_idx]++;
}

void TableManager::reset()
{
    // note, we don't have to reset neighbor_table. As the neighbor_table is the huge chunk
    // resetting all of them taking a huge cost. That's why I let the class have neighbor_num_table
    // to manage the date, which enables neighbor_table to keep nasty
    std::fill(neighbor_num_table.begin(), neighbor_num_table.end(), 0);
}

std::tuple<std::vector<uint>, std::vector<uint>, std::vector<bool>>
TableManager::connected_components(
        const std::vector<double>& vertices,
        const std::vector<int>& polygons){
    int n_facet = polygons.size() / 3;
    int n_vertex = vertices.size() / 3;
    std::vector<bool> isVisited(n_facet, false);

    std::vector<uint> facet_color_vector(n_facet);
    std::vector<uint> vertex_color_vector(n_vertex, -1);

    auto store_edge = [&](int idx_facet, std::unordered_map<std::pair<uint, uint>, uint>& edge_visit_count){
        auto a = polygons[idx_facet*3 + 0];
        auto b = polygons[idx_facet*3 + 1];
        auto c = polygons[idx_facet*3 + 2];

        auto add_key = [&](int idx1, int idx2){
            std::pair<uint, uint> key;
            if(idx1 < idx2){
                key.first = idx1; key.second = idx2;
            }else{
                key.first = idx2; key.second = idx1;
            }
            edge_visit_count[key]++;
        };
        add_key(a, b);
        add_key(b, c);
        add_key(c, a);
    };

    // iterate over facets using dfs
    auto fill_facet_color = [&](int idx_init, int color)->bool{
        std::stack<uint> Q;
        std::unordered_map<std::pair<uint, uint>, uint> edge_visit_count;

        // initialize
        isVisited[idx_init] = true;
        Q.push(idx_init);
        facet_color_vector[idx_init] = color;
        store_edge(idx_init, edge_visit_count);

        while(!Q.empty()){
            auto idx_here = Q.top();
            Q.pop();
            for(int i=0; i<3; i++){
                uint vert_idx = polygons[3 * idx_here + i];
                vertex_color_vector[vert_idx] = color;
                for(int j=0; j< neighbor_num_table[vert_idx]; j++){
                    uint near_facet_idx = neighbor_table[vert_idx][j];
                    if(!isVisited[near_facet_idx]){
                        isVisited[near_facet_idx] = true;
                        Q.push(near_facet_idx);
                        store_edge(near_facet_idx, edge_visit_count);
                        facet_color_vector[near_facet_idx] = color;
                    }
                }
            }
        }

        // check isclosed
        bool isclosed = true;
        for(auto& key_val : edge_visit_count){
            if(key_val.second != 2){
                isclosed = false;
                break;
            }
        }
        return isclosed;
    };

    auto first_false_idx = [&]() -> int{// return -1 if not found
        for(int idx=0; idx<n_facet; idx++){
            if(!isVisited[idx])
                return idx;
        }
        return -1;
    };

    int color = 0;
    std::vector<bool> isclosed_vec;
    while(true){
        auto idx_start = first_false_idx();
        if(idx_start == -1){break;}
        isclosed_vec.push_back(fill_facet_color(idx_start, color));
        color++;
    }
    return std::make_tuple(
            std::move(vertex_color_vector), std::move(facet_color_vector), std::move(isclosed_vec));
}

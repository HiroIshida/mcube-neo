#include <vector>
#include <stack>
#include <algorithm>    

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

        void reset(){
            // note, we don't have to reset neighbor_table. As the neighbor_table is the huge chunk
            // resetting all of them taking a huge cost. That's why I let the class have neighbor_num_table
            // to manage the date, which enables neighbor_table to keep nasty
            std::fill(neighbor_num_table.begin(), neighbor_num_table.end(), 0);
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

        std::vector<std::vector<uint>> connected_components(const std::vector<int>& polygons){
            int n_facet = polygons.size() / 3;
            std::vector<bool> isVisited(n_facet, false);

            auto make_group = [&](int idx_init){
                std::vector<uint> group;
                group.reserve(n_facet); // we know group will have n_vert elems at most
                std::stack<uint> Q;

                // initialize
                isVisited[idx_init] = true;
                group.push_back(idx_init);
                Q.push(idx_init);

                while(!Q.empty()){
                    auto idx_here = Q.top();
                    Q.pop();
                    for(int i=0; i<3; i++){
                        uint vert_idx = polygons[3 * idx_here + i];
                        for(int j=0; j< neighbor_num_table[vert_idx]; j++){
                            uint near_facet_idx = neighbor_table[vert_idx][j];
                            if(!isVisited[near_facet_idx]){
                                isVisited[near_facet_idx] = true;
                                group.push_back(near_facet_idx);
                                Q.push(near_facet_idx);
                            }
                        }
                    }
                }
                return group;
            };

            auto first_false_idx = [&]() -> int{// return -1 if not found
                for(int idx=0; idx<n_facet; idx++){
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

#include <vector>
#include <stack>
#include <algorithm>    
#include <unordered_map>    
#include "custom_hash.h"

struct TableManager
{
    public:
        std::vector<std::vector<uint>> neighbor_table;
        std::vector<uint> neighbor_num_table;
        TableManager(int n1, int n2, int n3);
        void add_element(uint vert_idx, uint elem);
        void reset();
        std::array<std::vector<uint>, 2> connected_components(
                const std::vector<double>& vertices,
                const std::vector<int>& polygons);
};

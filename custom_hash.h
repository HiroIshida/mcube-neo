#include<tuple>
#include<unordered_map>

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std{
    // example for std::tuple 
    // hash is std's type. By usign template specilization, we can add a new hashable type here!
    template<typename S, typename T>
    struct hash<std::pair<S, T>>
    {
        inline size_t operator()(const std::pair<S, T>& val) const
        {
            size_t seed = 0;
            hash_combine(seed, val.first);
            hash_combine(seed, val.second);
            return seed;
        }
    };
}

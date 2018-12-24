#include "embed.h"
#include <map>


namespace simple_embed {
    using resource_map_t = std::map<std::string, resource>;

    static resource_map_t& map()
    {
        static resource_map_t resources;
        return resources;
    }

    void store(std::string name, resource r)
    {
        resource_map_t& resources = map();
        resources.insert(std::make_pair(std::move(name), r));
    }

    boost::optional<resource> get(std::string name)
    {
        boost::optional<resource> result;
        resource_map_t& resources = map();
        auto it = resources.find(name);
        if (it != resources.end())
        {
            result = it->second;
        }
        return result;
    }
}

//
// Created by jonathan on 12/7/18.
//
#ifndef SIMPLE_EMBED_H
#define SIMPLE_EMBED_H

#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

#include <boost/optional.hpp>

namespace simple_embed {
    using resource = std::tuple<std::uint8_t const*, std::size_t>;
    inline resource make_resource(std::uint8_t const* data, std::size_t size)
    {
        return std::make_tuple(data, size);
    }

    extern void store(std::string name, resource r);
    extern boost::optional<resource> get(std::string name);

    class Register {
    public:
        Register(std::string name, resource r)
        {
            store(std::move(name), r);

        }
    };

}

#endif // SIMPLE_EMBED_H
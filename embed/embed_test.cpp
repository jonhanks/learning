#include <iostream>
#include <string>

#include "embed.h"

int main()
{
    auto resource = simple_embed::get("resource");
    if (resource)
    {
        simple_embed::resource& r = *resource;
        std::string s(reinterpret_cast<char const *>(std::get<std::uint8_t const*>(r)), std::get<std::size_t>(r));
        std::cout << s;
    }
    return 0;
}
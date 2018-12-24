#include "clara.hpp"

#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

class ByteStreamIterator {
public:
    using value_type = std::uint8_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const *;
    using reference = value_type const &;
    using iterator_category = std::input_iterator_tag;

    ByteStreamIterator(): stream_{nullptr}, entry_{0} {}

    explicit ByteStreamIterator(std::istream* stream): stream_{stream}, entry_{read_byte(stream)}
    {
    }

    bool operator==(const ByteStreamIterator& other)
    {
        return stream_ == other.stream_;
    }
    bool operator!=(const ByteStreamIterator& other)
    {
        return !operator==(other);
    }
    reference operator*() const
    {
        return entry_;
    }

    pointer operator->() const
    {
        return &entry_;
    }

    ByteStreamIterator& operator++()
    {
        entry_ = read_byte(stream_);
        stream_ = update_pointer(stream_);
        return *this;
    }
    ByteStreamIterator operator++(int)
    {
        ByteStreamIterator result{*this};

        entry_ = read_byte(stream_);
        stream_ = update_pointer(stream_);

        return result;
    }
private:
    static std::uint8_t read_byte(std::istream* stream)
    {
        std::array<char,1> buf{};
        stream->read(buf.data(), 1);
        if (stream->gcount())
        {
            return static_cast<std::uint8_t >(buf[0]);
        }
        return 0;
    }
    static std::istream* update_pointer(std::istream* stream)
    {
        return (stream ? (stream->good() ? stream : nullptr) : nullptr);
    }

    std::istream* stream_ = nullptr;
    value_type entry_ = 0;
};


class Converter {
public:
    std::string operator()(char ch)
    {
        bool newline = false;
        char buffer[25];

        if (count_ % 25 == 0 && count_ > 1)
        {
            newline = true;
        }
        std::sprintf(buffer, "%s0x%x%s", (count_ ? ", ": ""), (int)ch, (newline ? "\n" : ""));
        ++count_;
        return std::string(buffer);
    }
private:
    int count_ = 0;
};

int main(int argc, char* argv[])
{
    bool showHelp = false;
    std::string inputName;
    std::string outputName;
    std::string resName = "resource";

    auto cli = clara::Help( showHelp )
            | clara::Opt( inputName, "path" )
            ["-i"]["--input"]
            ("Input file")
            | clara::Opt( outputName, "path")
            ["-o"]["--output"]
            ("Output file")
            | clara::Opt( resName, "name")
            ["-n"]["--name"]
            ("Name of the resource");
    cli.parse( clara::Args( argc, argv ));
    if (showHelp || inputName.empty() || outputName.empty())
    {
        std::cout << cli << std::endl;
        return 1;
    }

    std::ifstream in_f{inputName};
    std::ofstream out_f{outputName};

    out_f << "#include \"embed.h\"\n\nstatic const std::uint8_t resource[] = {\n";

    Converter conv;
    std::transform(ByteStreamIterator(&in_f), ByteStreamIterator(),
            std::ostream_iterator<std::string>(out_f, ""), conv);

    out_f << "};\n\nstatic simple_embed::Register r_(\"" << resName << "\", simple_embed::make_resource(resource, sizeof(resource)));\n";

    return 0;
}
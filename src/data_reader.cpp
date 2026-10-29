#include "data_reader.h"

#include "program_error.h"

#include <limits>
#include <utility>

DataReader::DataReader(const std::string& path)
    : path(path)
    , stream(path)
{
    if (!stream.is_open())
        throw IOError("DataReader::DataReader",
                      "file '" + path + "' cannot be opened");
}

VectorList DataReader::read()
{
    const uint32_t count = nextLength("vector count", 0);
    VectorList vectors;
    while (vectors.size() < count) {
        const std::string name =
            "vector #" + std::to_string(vectors.size() + 1);
        const uint32_t length = nextLength("length of " + name, 1);
        NumberVector vector;
        while (vector.size() < length)
            vector.push_back(next("element #" +
                                  std::to_string(vector.size() + 1) + " of " +
                                  name));
        vectors.push_back(std::move(vector));
    }
    stream >> std::ws;
    if (!stream.eof())
        throw IOError("DataReader::read",
                      "file '" + path + "': extra data after " +
                          std::to_string(count) + " vectors");
    return vectors;
}

Number DataReader::next(const std::string& item)
{
    Number number;
    if (!(stream >> number))
        throw IOError("DataReader::read",
                      "file '" + path + "': " + item +
                          " is missing or is not an int64_t number");
    return number;
}

uint32_t DataReader::nextLength(const std::string& item, uint32_t minimum)
{
    const Number length = next(item);
    if (length < minimum || length > std::numeric_limits<uint32_t>::max())
        throw IOError("DataReader::read",
                      "file '" + path + "': " + item + " " +
                          std::to_string(length) + " is out of range " +
                          std::to_string(minimum) + "..4294967295");
    return static_cast<uint32_t>(length);
}

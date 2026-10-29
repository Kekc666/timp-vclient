#include "data_writer.h"

#include "program_error.h"

#include <cstdint>

DataWriter::DataWriter(const std::string& path)
    : path(path)
    , stream(path, std::ios::binary)
{
    if (!stream.is_open())
        throw IOError("DataWriter::DataWriter",
                      "file '" + path + "' cannot be created");
}

void DataWriter::write(const NumberVector& results)
{
    const uint32_t count = results.size();
    stream.write(reinterpret_cast<const char*>(&count), sizeof count);
    stream.write(reinterpret_cast<const char*>(results.data()),
                 results.size() * sizeof(Number));
    stream.flush();
    if (!stream)
        throw IOError("DataWriter::write",
                      "writing " + std::to_string(count) + " results to '" +
                          path + "' failed");
}

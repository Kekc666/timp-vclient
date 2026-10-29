#pragma once

#include "vectors.h"

#include <cstdint>
#include <fstream>
#include <string>

class DataReader {
public:
    explicit DataReader(const std::string& path);
    VectorList read();

private:
    std::string path;
    std::ifstream stream;

    Number next(const std::string& item);
    uint32_t nextLength(const std::string& item, uint32_t minimum);
};

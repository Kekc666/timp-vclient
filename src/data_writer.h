#pragma once

#include "vectors.h"

#include <fstream>
#include <string>

class DataWriter {
public:
    explicit DataWriter(const std::string& path);
    void write(const NumberVector& results);

private:
    std::string path;
    std::ofstream stream;
};

#pragma once

#include <cstdint>
#include <string>

struct Options {
    std::string address;
    uint16_t port = 33333;
    std::string inputPath;
    std::string outputPath;
    std::string credentialsPath = "~/.config/vclient.conf";
};

class UserInterface {
public:
    bool parse(int argc, char* argv[]);
    const Options& options() const;
    static std::string usage();

private:
    Options opts;
};

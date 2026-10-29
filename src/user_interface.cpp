#include "user_interface.h"

#include "program_error.h"

#include <getopt.h>

#include <cstdlib>

namespace {

const char shortOptions[] = ":ha:p:i:o:c:";

const option longOptions[] = {
    {"help", no_argument, nullptr, 'h'},
    {"address", required_argument, nullptr, 'a'},
    {"port", required_argument, nullptr, 'p'},
    {"input", required_argument, nullptr, 'i'},
    {"output", required_argument, nullptr, 'o'},
    {"config", required_argument, nullptr, 'c'},
    {nullptr, 0, nullptr, 0},
};

uint16_t toPort(const std::string& text)
{
    const bool digitsOnly = !text.empty() && text.size() <= 5 &&
        text.find_first_not_of("0123456789") == std::string::npos;
    const unsigned long port = digitsOnly ? std::stoul(text) : 0;
    if (port == 0 || port > 65535)
        throw InterfaceError("UserInterface::parse",
                             "invalid port '" + text + "', expected 1..65535");
    return static_cast<uint16_t>(port);
}

void require(const std::string& value, const std::string& option)
{
    if (value.empty())
        throw InterfaceError("UserInterface::parse",
                             "required option " + option + " is missing");
}

std::string expandHome(const std::string& path)
{
    if (path.rfind("~/", 0) != 0)
        return path;
    const char* home = std::getenv("HOME");
    if (home == nullptr)
        throw InterfaceError("UserInterface::parse",
                             "cannot expand '" + path + "': HOME is not set");
    return std::string(home) + path.substr(1);
}

}

bool UserInterface::parse(int argc, char* argv[])
{
    if (argc < 2)
        return false;
    optind = 0;
    int option;
    while ((option = getopt_long(argc, argv, shortOptions, longOptions,
                                 nullptr)) != -1) {
        switch (option) {
        case 'h':
            return false;
        case 'a':
            opts.address = optarg;
            break;
        case 'p':
            opts.port = toPort(optarg);
            break;
        case 'i':
            opts.inputPath = optarg;
            break;
        case 'o':
            opts.outputPath = optarg;
            break;
        case 'c':
            opts.credentialsPath = optarg;
            break;
        case ':':
            throw InterfaceError("UserInterface::parse",
                                 "option '" + std::string(argv[optind - 1]) +
                                     "' requires a value");
        default:
            throw InterfaceError("UserInterface::parse",
                                 "unknown option '" +
                                     std::string(argv[optind - 1]) + "'");
        }
    }
    if (optind < argc)
        throw InterfaceError("UserInterface::parse",
                             "unexpected argument '" +
                                 std::string(argv[optind]) + "'");
    require(opts.address, "-a/--address");
    require(opts.inputPath, "-i/--input");
    require(opts.outputPath, "-o/--output");
    opts.credentialsPath = expandHome(opts.credentialsPath);
    return true;
}

const Options& UserInterface::options() const
{
    return opts;
}

std::string UserInterface::usage()
{
    return "Usage: vclient -a ADDRESS -i INPUT -o OUTPUT [-p PORT] [-c FILE]\n"
           "  -h, --help            show this help\n"
           "  -a, --address ADDRESS server IPv4 address (required)\n"
           "  -p, --port PORT       server port (default 33333)\n"
           "  -i, --input FILE      text file with vectors (required)\n"
           "  -o, --output FILE     binary file for results (required)\n"
           "  -c, --config FILE     login and password file\n"
           "                        (default ~/.config/vclient.conf)\n";
}

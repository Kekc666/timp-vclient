#include "authenticator.h"
#include "communicator.h"
#include "data_reader.h"
#include "data_writer.h"
#include "user_interface.h"

#include <exception>
#include <iostream>

namespace {

NumberVector process(const Options& options, const VectorList& vectors)
{
    const Authenticator authenticator =
        Authenticator::fromFile(options.credentialsPath);
    Communicator communicator(options.address, options.port);
    communicator.connect();
    communicator.authenticate(authenticator);
    return communicator.calculate(vectors);
}

}

int main(int argc, char* argv[])
{
    try {
        UserInterface ui;
        if (!ui.parse(argc, argv)) {
            std::cout << UserInterface::usage();
            return 0;
        }
        const Options& options = ui.options();
        const VectorList vectors = DataReader(options.inputPath).read();
        const NumberVector results = process(options, vectors);
        DataWriter(options.outputPath).write(results);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}

#pragma once

#include "authenticator.h"
#include "vectors.h"

#include <cstddef>
#include <cstdint>
#include <string>

class Communicator {
public:
    Communicator(const std::string& address, uint16_t port);
    ~Communicator();
    Communicator(const Communicator&) = delete;
    Communicator& operator=(const Communicator&) = delete;

    void connect();
    void authenticate(const Authenticator& authenticator);
    NumberVector calculate(const VectorList& vectors);

private:
    std::string address;
    uint16_t port;
    int sock = -1;

    void transmit(const void* data, std::size_t size,
                  const std::string& function);
    void receive(void* data, std::size_t size, const std::string& function);
    std::string endpoint() const;
};

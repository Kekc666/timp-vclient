#include "communicator.h"

#include "program_error.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

namespace {

const timeval receiveTimeout{30, 0};
const std::size_t replyCapacity = 16;

std::string lastError()
{
    return std::strerror(errno);
}

}

Communicator::Communicator(const std::string& address, uint16_t port)
    : address(address)
    , port(port)
{
}

Communicator::~Communicator()
{
    if (sock >= 0)
        close(sock);
}

void Communicator::connect()
{
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_aton(address.c_str(), &server.sin_addr) == 0)
        throw NetworkError("Communicator::connect",
                           "'" + address + "' is not a valid IPv4 address");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        throw NetworkError("Communicator::connect",
                           "socket() failed: " + lastError());
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &receiveTimeout,
                   sizeof receiveTimeout) != 0)
        throw NetworkError("Communicator::connect",
                           "setsockopt() failed: " + lastError());
    if (::connect(sock, reinterpret_cast<const sockaddr*>(&server),
                  sizeof server) != 0)
        throw NetworkError("Communicator::connect",
                           "connection to " + endpoint() +
                               " failed: " + lastError());
}

void Communicator::authenticate(const Authenticator& authenticator)
{
    const std::string request = authenticator.message();
    transmit(request.data(), request.size(), "Communicator::authenticate");
    char reply[replyCapacity];
    const ssize_t length = recv(sock, reply, replyCapacity, 0);
    if (length < 0)
        throw NetworkError("Communicator::authenticate",
                           "no reply from " + endpoint() + ": " + lastError());
    if (length == 0)
        throw NetworkError("Communicator::authenticate",
                           endpoint() + " closed the connection");
    const std::string answer(reply, length);
    if (answer != "OK")
        throw AuthError("Communicator::authenticate",
                        "login '" + authenticator.login() + "' rejected by " +
                            endpoint() + ", reply '" + answer + "'");
}

NumberVector Communicator::calculate(const VectorList& vectors)
{
    const uint32_t count = vectors.size();
    transmit(&count, sizeof count, "Communicator::calculate");
    NumberVector results(vectors.size());
    for (std::size_t i = 0; i < vectors.size(); ++i) {
        const uint32_t length = vectors[i].size();
        transmit(&length, sizeof length, "Communicator::calculate");
        transmit(vectors[i].data(), length * sizeof(Number),
                 "Communicator::calculate");
        receive(&results[i], sizeof(Number), "Communicator::calculate");
    }
    return results;
}

void Communicator::transmit(const void* data, std::size_t size,
                            const std::string& function)
{
    if (send(sock, data, size, MSG_NOSIGNAL) != static_cast<ssize_t>(size))
        throw NetworkError(function,
                           "sending " + std::to_string(size) + " bytes to " +
                               endpoint() + " failed: " + lastError());
}

void Communicator::receive(void* data, std::size_t size,
                           const std::string& function)
{
    const ssize_t length = recv(sock, data, size, MSG_WAITALL);
    if (length < 0)
        throw NetworkError(function,
                           "receiving from " + endpoint() +
                               " failed: " + lastError());
    if (length != static_cast<ssize_t>(size))
        throw NetworkError(function,
                           "received " + std::to_string(length) + " of " +
                               std::to_string(size) + " bytes from " +
                               endpoint());
}

std::string Communicator::endpoint() const
{
    return address + ":" + std::to_string(port);
}

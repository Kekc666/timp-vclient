#include "authenticator.h"

#include "program_error.h"

#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

#include <fstream>

namespace {

const std::size_t saltBytes = 8;

std::string toHex(const CryptoPP::byte* data, std::size_t size)
{
    std::string hex;
    CryptoPP::ArraySource(
        data, size, true,
        new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex)));
    return hex;
}

}

Authenticator::Authenticator(const std::string& login,
                             const std::string& password)
    : userLogin(login)
    , userPassword(password)
{
}

Authenticator Authenticator::fromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw IOError("Authenticator::fromFile",
                      "credentials file '" + path + "' cannot be opened");
    std::string login;
    std::string password;
    if (!std::getline(file, login) || !std::getline(file, password) ||
        login.empty() || password.empty())
        throw IOError(
            "Authenticator::fromFile",
            "credentials file '" + path +
                "' must contain login and password on separate lines");
    return Authenticator(login, password);
}

std::string Authenticator::generateSalt()
{
    CryptoPP::byte salt[saltBytes];
    CryptoPP::AutoSeededRandomPool().GenerateBlock(salt, saltBytes);
    return toHex(salt, saltBytes);
}

std::string Authenticator::digest(const std::string& salt,
                                  const std::string& password)
{
    const std::string data = salt + password;
    CryptoPP::byte hash[CryptoPP::SHA224::DIGESTSIZE];
    CryptoPP::SHA224().CalculateDigest(
        hash, reinterpret_cast<const CryptoPP::byte*>(data.data()),
        data.size());
    return toHex(hash, sizeof hash);
}

std::string Authenticator::message() const
{
    return message(generateSalt());
}

std::string Authenticator::message(const std::string& salt) const
{
    return userLogin + salt + digest(salt, userPassword);
}

const std::string& Authenticator::login() const
{
    return userLogin;
}

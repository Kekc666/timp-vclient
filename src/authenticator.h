#pragma once

#include <string>

class Authenticator {
public:
    Authenticator(const std::string& login, const std::string& password);
    static Authenticator fromFile(const std::string& path);
    static std::string generateSalt();
    static std::string digest(const std::string& salt,
                              const std::string& password);

    std::string message() const;
    std::string message(const std::string& salt) const;
    const std::string& login() const;

private:
    std::string userLogin;
    std::string userPassword;
};

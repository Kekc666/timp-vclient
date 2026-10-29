#pragma once

#include <stdexcept>
#include <string>

class ProgramError : public std::runtime_error {
public:
    ProgramError(const std::string& type, const std::string& function,
                 const std::string& message)
        : std::runtime_error(type + " [" + function + "]: " + message)
    {
    }
};

class InterfaceError : public ProgramError {
public:
    InterfaceError(const std::string& function, const std::string& message)
        : ProgramError("InterfaceError", function, message)
    {
    }
};

class IOError : public ProgramError {
public:
    IOError(const std::string& function, const std::string& message)
        : ProgramError("IOError", function, message)
    {
    }
};

class NetworkError : public ProgramError {
public:
    NetworkError(const std::string& function, const std::string& message)
        : ProgramError("NetworkError", function, message)
    {
    }
};

class AuthError : public ProgramError {
public:
    AuthError(const std::string& function, const std::string& message)
        : ProgramError("AuthError", function, message)
    {
    }
};

#ifndef BIS_ENVIRONMENT_HPP
#define BIS_ENVIRONMENT_HPP

#include "RuntimeError.hpp"
#include "Token.hpp"
#include <algorithm>
#include <any>

class Environment
{
private:
    std::unordered_map<std::string_view, std::any> values;

public:
    Environment() = default;
    void define(std::string_view identifier, const std::any& value);
    const std::any& lookup(const Token& identifier);
};

#endif // BIS_ENVIRONMENT_HPP
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "RuntimeError.hpp"
#include "Token.hpp"
#include <algorithm>
#include <any>
#include <cassert>
#include <memory>

class Environment
{
public:
    explicit Environment(std::shared_ptr<Environment> parent_env);

    Environment();

    void define(const std::string& identifier, const std::any& value);

    void assign(const Token& identifier, const std::any& value);

    const std::any& lookup(const Token& identifier);

private:
    std::shared_ptr<Environment> parent_env;
    std::unordered_map<std::string, std::any> values;
};

#endif // ENVIRONMENT_HPP
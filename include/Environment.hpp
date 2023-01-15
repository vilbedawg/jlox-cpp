#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "../include/Typedef.hpp"
#include "ListType.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"
#include <any>
#include <cassert>
#include <memory>
#include <unordered_map>

class Environment
{
public:
    explicit Environment(std::shared_ptr<Environment> parent_env);

    Environment();

    void define(const std::string& identifier, const std::any& value);

    void define(const std::string& identifier, shared_ptr_any ptr_to_val);

    void assign(const Token& identifier, const std::any& value);

    void assignAt(size_t distance, const Token& identifier, const std::any& value);

    shared_ptr_any lookup(const Token& identifier);

    shared_ptr_any getAt(size_t distance, const std::string& identifier);

    Environment* ancestor(size_t distance);

private:
    std::shared_ptr<Environment> parent_env;
    std::unordered_map<std::string, shared_ptr_any> values;
};

#endif // ENVIRONMENT_HPP
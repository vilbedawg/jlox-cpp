#ifndef BUILT_IN_HPP
#define BUILT_IN_HPP

#include "Callable.hpp"
#include "FunctionType.hpp"
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

class ClockCallable : public Callable
{
public:
    size_t getArity() const override;

    std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;

    std::string toString() const override;

private:
    size_t arity = 0u;
};

class PrintCallable : public Callable
{
public:
    explicit PrintCallable(size_t arity = 0u) : arity{arity} {}

    size_t getArity() const override;

    std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;

    std::string toString() const override;

private:
    size_t arity;
};

std::string stringify(const std::any& item);

#endif // BUILT_IN_HPP
#ifndef BUILT_IN_HPP
#define BUILT_IN_HPP

#include "Callable.hpp"
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

namespace bis
{
    class ClockCallable : public Callable
    {
    private:
        size_t arity = 0u;

    public:
        size_t getArity() const override;
        std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;
        std::string toString() const override;
    };

    class PrintCallable : public Callable
    {
    private:
        size_t arity;

    public:
        explicit PrintCallable(size_t arity = 0u) : arity{arity} {}
        size_t getArity() const override;
        std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;
        std::string toString() const override;
    };
}

#endif // BUILT_IN_HPP
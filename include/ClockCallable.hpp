#ifndef BIS_CLOCK_CALLABLE_HPP
#define BIS_CLOCK_CALLABLE_HPP

#include "Callable.hpp"
#include <chrono>
#include <string>

class ClockCallable : public Callable
{
private:
    int arity = 0;

public:
    int getArity() const override;
    std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;
    std::string toString() const override;
};

#endif // BIS_CLOCK_CALLABLE_HPP
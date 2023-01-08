#ifndef BIS_CALLABLE_HPP
#define BIS_CALLABLE_HPP

#include <any>
#include <string>
#include <vector>

class Interpreter;

class Callable
{
public:
    virtual size_t getArity() const = 0;
    virtual std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const = 0;
    virtual std::string toString() const = 0;
    virtual ~Callable() = default;
};

#endif // BIS_CALLABLE_HPP
#ifndef BIS_CALLABLE_HPP
#define BIS_CALLABLE_HPP

#include <any>
#include <functional>
#include <string>
#include <vector>

class Interpreter;

using FuncType = std::function<std::any(Interpreter&, const std::vector<std::any>&)>;

class Callable
{
public:
    virtual int getArity() const = 0;
    virtual std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const = 0;
    virtual std::string toString() const = 0;
    virtual ~Callable() = default;
};

#endif // BIS_CALLABLE_HPP
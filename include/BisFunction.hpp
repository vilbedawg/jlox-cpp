#ifndef BIS_FUNCTION_HPP
#define BIS_FUNCTION_HPP

#include "Callable.hpp"
#include "Interpreter.hpp"
#include <memory>

struct FnStmt;

class BisFunction : public Callable
{
private:
    size_t arity = 0u;
    const FnStmt* declaration;
    std::shared_ptr<Environment> closure;

public:
    BisFunction(const FnStmt* declaration, std::shared_ptr<Environment> closure);

    size_t getArity() const override;
    std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;
    std::string toString() const override;
};

#endif // BIS_FUNCTION_HPP
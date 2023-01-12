#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Callable.hpp"
#include "Interpreter.hpp"
#include <memory>

struct FnStmt;

class FunctionType : public Callable
{
public:
    FunctionType(const FnStmt* declaration, std::shared_ptr<Environment> closure);

    size_t getArity() const override;

    std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;

    std::string toString() const override;

private:
    size_t arity = 0u;
    const FnStmt* declaration;
    std::shared_ptr<Environment> closure;
};

#endif // FUNCTION_TYPE_HPP
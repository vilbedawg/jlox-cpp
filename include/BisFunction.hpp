#ifndef BIS_FUNCTION_HPP
#define BIS_FUNCTION_HPP

#include "Callable.hpp"
#include "Interpreter.hpp"
#include <memory>

struct FnStmt;

class BisFunction : public Callable
{
private:
    int arity = 0;
    FnStmt* funcDeclaration;

public:
    explicit BisFunction(FnStmt* declaration);

    int getArity() const override;
    std::any call(Interpreter& interpreter, const std::vector<std::any>& args) const override;
    std::string toString() const override;
};

#endif // BIS_FUNCTION_HPP
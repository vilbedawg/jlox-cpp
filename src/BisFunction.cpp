#include "../include/BisFunction.hpp"
#include "../include/Environment.hpp"

BisFunction::BisFunction(FnStmt* declaration) : funcDeclaration{declaration}
{
}

int BisFunction::getArity() const
{
    return arity;
}

std::any BisFunction::call(Interpreter& interpreter, const std::vector<std::any>& args) const
{
    auto environment = std::make_unique<Environment>(interpreter.getGlobalEnvironment());
    for (int i = 0; i < funcDeclaration->params.size(); i++)
    {
        environment->define(funcDeclaration->params.at(i).lexeme, args.at(i));
    }

    interpreter.executeBlock(funcDeclaration->body, std::move(environment));
    return {};
}

std::string BisFunction::toString() const
{
    return "<function>";
}
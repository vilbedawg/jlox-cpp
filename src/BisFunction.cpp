#include "../include/BisFunction.hpp"
#include "../include/Environment.hpp"
#include "../include/RuntimeException.hpp"

BisFunction::BisFunction(const FnStmt* declaration) : declaration{declaration}
{
}

int BisFunction::getArity() const
{
    return static_cast<int>(declaration->params.size());
}

std::any BisFunction::call(Interpreter& interpreter, const std::vector<std::any>& args) const
{
    auto environment = std::make_unique<Environment>(interpreter.getGlobalEnvironment());
    for (size_t i = 0u; i < declaration->params.size(); i++)
    {
        environment->define(declaration->params.at(i).lexeme, args.at(i));
    }
    try
    {
        interpreter.executeBlock(declaration->body, std::move(environment));
    }
    catch (const ReturnException& return_exception)
    {
        return return_exception.getReturnValue();
    }

    return {};
}

std::string BisFunction::toString() const
{
    return "<fn " + declaration->identifier.lexeme + ">";
}
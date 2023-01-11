#include "../include/FunctionType.hpp"
#include "../include/Environment.hpp"
#include "../include/RuntimeException.hpp"

FunctionType::FunctionType(const FnStmt* declaration, std::shared_ptr<Environment> closure)
    : declaration{declaration}, closure{closure}
{
}

size_t FunctionType::getArity() const
{
    return declaration->params.size();
}

std::any FunctionType::call(Interpreter& interpreter, const std::vector<std::any>& args) const
{
    auto environment = std::make_shared<Environment>(closure);

    for (size_t i = 0u; i < declaration->params.size(); ++i)
    {
        environment->define(declaration->params[i].lexeme, args[i]);
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

std::string FunctionType::toString() const
{
    return "<fn " + declaration->identifier.lexeme + ">";
}
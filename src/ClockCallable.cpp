#include "../include/ClockCallable.hpp"

int ClockCallable::getArity() const
{
    return 0;
}

std::any ClockCallable::call(Interpreter& interpreter, const std::vector<std::any>& args) const
{
    static_assert(std::is_integral_v<std::chrono::system_clock::rep>,
                  "Representation of ticks isn't an integral value.");

    auto now = std::chrono::system_clock::now().time_since_epoch();
    return static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(now).count());
}

std::string ClockCallable::toString() const
{
    return "<native fn>";
}
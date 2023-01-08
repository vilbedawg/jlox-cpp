#include "../include/BuiltIn.hpp"

namespace bis
{
    // Native clock
    size_t ClockCallable::getArity() const
    {
        return 0u;
    }

    std::any ClockCallable::call(Interpreter& interpreter, const std::vector<std::any>& args) const
    {
        static_assert(std::is_integral_v<std::chrono::system_clock::rep>,
                      "Representation of ticks isn't an integral value.");

        // Returns Unix time in seconds.
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(now).count());
    }

    std::string ClockCallable::toString() const
    {
        return "<native fn>";
    }

    // Native print
    size_t PrintCallable::getArity() const
    {
        return arity;
    }

    std::any PrintCallable::call(Interpreter& interpreter, const std::vector<std::any>& args) const
    {
        std::stringstream stream;

        for (auto& arg : args)
        {
            if (!arg.has_value())
            {
                stream << "nil";
            }
            else if (arg.type() == typeid(bool))
            {
                stream << (std::any_cast<bool>(arg) ? "true" : "false");
            }
            else if (arg.type() == typeid(double))
            {
                auto num_as_string = std::to_string(std::any_cast<double>(arg));
                // remove trailing zeroes.
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                stream << num_as_string;
            }
            else if (arg.type() == typeid(std::string))
            {
                auto str = std::any_cast<std::string>(arg);
                if (str == "\\n")
                    stream << '\n';
                else if (str == "\\t")
                    stream << '\t';
                else
                    stream << str;
            }

            stream << ' ';
        }
        std::cout << stream.str();
        return {};
    }

    std::string PrintCallable::toString() const
    {
        return "native print";
    }
}

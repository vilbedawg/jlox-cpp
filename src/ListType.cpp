#include "../include/ListType.hpp"
#include "../include/RuntimeError.hpp"

List::List(std::vector<std::any> values)
    : values{std::move(values)}, len{static_cast<int>(this->values.size())}
{
}

int List::length() const
{
    return len;
}

std::any& List::at(int index)
{
    return index < 0 ? values.at(len + index) : values.at(index);
}

void List::append(const std::any& value)
{
    values.push_back(value);
    len += 1;
}

std::any List::pop()
{
    const auto value = values.back();
    values.pop_back();
    len -= 1;

    return value;
}

void List::remove(int index)
{
    if (index < 0)
    {
        values.erase(values.end() - 1 + index);
    }
    else
    {
        values.erase(values.begin() + index);
    }

    len -= 1;
}

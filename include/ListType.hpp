#include <algorithm>
#include <any>
#include <stdexcept>
#include <vector>

class List
{
public:
    List() = default;

    explicit List(std::vector<std::any> values);

    size_t length() const noexcept;

    std::any& at(int index);

    void append(const std::any& value);

    std::any pop() noexcept;

    void remove(int index);

private:
    std::vector<std::any> values;
    size_t len = 0u;
};

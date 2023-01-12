#include <algorithm>
#include <any>
#include <stdexcept>
#include <vector>

class List
{
private:
    std::vector<std::any> values;
    int len = 0;

public:
    List() = default;

    explicit List(std::vector<std::any> values);

    int length() const noexcept;

    std::any& at(int index);

    void append(const std::any& value);

    std::any pop() noexcept;

    void remove(int index);
};

#pragma once
#include <stdexcept>
#include <string>
namespace demo_stubs {
[[noreturn]] inline void unavailable(const char *operation) {
    throw std::logic_error(std::string("Demo stub: ") + operation +
        "; connect the original library to use this operation");
}
}

#ifndef RAPID_CORE_STACKDUMP_H
#define RAPID_CORE_STACKDUMP_H

#include <string>
#include <array>

namespace rapid { namespace core {

class StackTrace {
public:
    StackTrace();

    std::string toString() const;

    StackTrace(const StackTrace &) = delete;
    StackTrace& operator=(const StackTrace &) = delete;

private:
    int count_;
    void* stack_[62];
    std::string trace_;
};

} }

#endif // RAPID_CORE_STACKDUMP_H

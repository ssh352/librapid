#include <rapid/core/utility.h>
#include <rapid/core/stacktrace.h>
#include <rapid/core/exception.h>

namespace rapid { namespace core {

AssertException::AssertException(const std::string &message,
                                 const std::string &fileName,
                                 int line,
                                 const std::string &what)
    : message_(message)
    , fileName_(fileName)
    , line_(line)
    , what_(what.empty() ? getSystemErrorString() : "") {
    StackTrace trace;
    trace_ = trace.toString();
}

} }

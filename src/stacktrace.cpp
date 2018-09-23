#include <sstream>
#include <memory>
#include <cstring>

#include <signal.h>

#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

#include <rapid/core/stacktrace.h>

namespace rapid { namespace core {

struct FreeDeleter {
  void operator()(void* ptr) const {
      free(ptr);
  }
};

using SymbolHandle = std::unique_ptr<char*, FreeDeleter>;
using DemangleHandle = std::unique_ptr<char, FreeDeleter>;

static std::string demangleSymbols(std::string &text) {
    std::string result;

    auto begin = text.find('(');
    auto end = text.find('+', begin);

    if (begin == std::string::npos || end == std::string::npos) {
        result += text;
        return result;
    }

    ++begin;

    int status = 0;
    DemangleHandle demangled_symbol(
                abi::__cxa_demangle(text.substr(begin, end - begin).c_str(),
                nullptr,
                nullptr,
                &status));
    if (status != 0) {
        result += text;
        return result;
    }

    result.append(text.substr(0, begin));
    result.append(demangled_symbol.get());
    result.append(text.substr(end));

    return result;
}

static std::string formatStack(void* const* trace, int size) {
    SymbolHandle trace_symbols(backtrace_symbols(trace, static_cast<int>(size)));
    std::ostringstream ostr;

    ostr << "\r\n";

    if (trace_symbols.get()) {
        for (int i = 0; i < size; ++i) {
            std::string trace_symbol(trace_symbols.get()[i]);
            ostr << std::string("    ")
                 << demangleSymbols(trace_symbol);
            Dl_info info {};
            if (dladdr(trace_symbols.get()[i], &info) && info.dli_sname) {
                auto offset = uint64_t(reinterpret_cast<const char *>(trace_symbols.get()[i])
                                       - reinterpret_cast<const char *>(info.dli_saddr));
                ostr << "("
                     << info.dli_sname
                     << ")+"
                     << offset;
            }
            ostr << "\r\n";
        }
    }
    return ostr.str();
}

static void stackDumpSignalHandler(int signal, siginfo_t* info, void* context) {

}

void resigterInProcessStackTrace() {
    struct sigaction sigpipe_action;
    memset(&sigpipe_action, 0, sizeof(sigpipe_action));
    sigpipe_action.sa_handler = SIG_IGN;
    sigemptyset(&sigpipe_action.sa_mask);
    sigaction(SIGPIPE, &sigpipe_action, nullptr);

    // pre-load symbols
    StackTrace load;

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_RESETHAND | SA_SIGINFO;
    action.sa_sigaction = &stackDumpSignalHandler;
    sigemptyset(&action.sa_mask);

    sigaction(SIGILL, &action, nullptr);
    sigaction(SIGABRT, &action, nullptr);
    sigaction(SIGFPE, &action, nullptr);
    sigaction(SIGBUS, &action, nullptr);
    sigaction(SIGSEGV, &action, nullptr);
    sigaction(SIGSYS, &action, nullptr);
}

StackTrace::StackTrace() {
    count_ = backtrace(stack_, 62);
    trace_ = formatStack(stack_, count_);
}

std::string StackTrace::toString() const {
    return trace_;
}

} }

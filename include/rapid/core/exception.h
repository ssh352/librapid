//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_EXCEPTION_H
#define RAPID_CORE_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace rapid { namespace core {

class AssertException : public std::exception {
public:
    AssertException(const std::string &message = "",
                    const std::string &fileName = "",
                    int line = 0,
                    const std::string &what = "");

    virtual const char * what() const noexcept {
        return what_.c_str();
    }

    std::string message() const {
        return message_;
    }

    int throwFileline() const noexcept {
        return line_;
    }

    std::string throwedFileName() const noexcept {
        return fileName_;
    }

    std::string trace() const {
        return trace_;
    }
private:
    std::string message_;
    std::string fileName_;
    int line_;
    std::string what_;
    std::string trace_;
};

class UuidErrorException : public std::runtime_error {
public:
   explicit UuidErrorException(std::string_view message)
      : std::runtime_error(message.data()) {
   }

   explicit UuidErrorException(const char * message)
      : std::runtime_error(message) {
   }
};

#define ensureOrThrow(expr, message) if (!(expr)) \
    throw AssertException(message, __FILE__, __LINE__); \
    else (void)0;

} }

#endif // RAPID_CORE_EXCEPTION_H

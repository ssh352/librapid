//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_LOGGER_H
#define RAPID_CORE_LOGGER_H

#include <memory>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/fmt/ostr.h>

namespace rapid { namespace log {

class Logger {
public:
    static Logger & get() {
        static Logger instance;
        return instance;
    }

    ~Logger();

    Logger(const Logger &logger) = delete;
    Logger& operator=(const Logger &logger) = delete;

    Logger& addFileLogger(const std::string &fileName);

    Logger& addConsoleLogger();

    std::shared_ptr<spdlog::logger> getLogger(const std::string &name, bool async_logging = true);

private:
    Logger();

    std::vector<spdlog::sink_ptr> sinks_;
};

} }

#endif // RAPID_CORE_LOGGER_H

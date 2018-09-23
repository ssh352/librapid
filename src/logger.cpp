#include <experimental/filesystem>

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>

#include <rapid/core/logger.h>

namespace rapid { namespace log {

namespace fs = std::experimental::filesystem;
using spdlog::sinks::sink;

static void createLogsDirIfNotExists() {
    const fs::path log_path("logs");
    if (!fs::exists(log_path)) {
        fs::create_directory(log_path);
    }
}

Logger::Logger() {
    spdlog::init_thread_pool(8192, 2);
}

Logger::~Logger() {
    spdlog::shutdown();
}

std::shared_ptr<spdlog::logger> Logger::getLogger(const std::string &name, bool async_logging) {
    auto logger = spdlog::get(name);
    if (logger != nullptr) {
        return logger;
    }

    if (async_logging) {
        logger = std::make_shared<spdlog::async_logger>(name,
            std::begin(sinks_),
            std::end(sinks_),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
    }
    else {
        logger = std::make_shared<spdlog::logger>(name,
            std::begin(sinks_),
            std::end(sinks_));
    }
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%H:%M:%S.%e][%l][%n] %v");
    logger->flush_on(spdlog::level::debug);

    spdlog::register_logger(logger);
    return logger;
}

Logger& Logger::addConsoleLogger() {
    sinks_.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    return *this;
}

Logger& Logger::addFileLogger(const std::string &fileName) {
    createLogsDirIfNotExists();

    std::ostringstream ostr;
    ostr << "logs/" << fileName;
    sinks_.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(ostr.str(), 1024 * 1024, 0));
    return *this;
}

} }

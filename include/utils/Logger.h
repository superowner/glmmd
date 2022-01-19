#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class Logger
{
private:
    static std::shared_ptr<spdlog::logger> m_logger;

public:
    static void init();
    static inline std::shared_ptr<spdlog::logger> &getLogger() { return m_logger; }
};

#define GLMMD_LOG_ERROR(...) ::Logger::getLogger()->error(__VA_ARGS__)
#define GLMMD_LOG_WARN(...) ::Logger::getLogger()->warn(__VA_ARGS__)
#define GLMMD_LOG_INFO(...) ::Logger::getLogger()->info(__VA_ARGS__)

#endif
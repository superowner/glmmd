#include <utils/Logger.h>
std::shared_ptr<spdlog::logger> Logger::m_logger;

void Logger::init()
{
    spdlog::set_pattern("%^[%T][%n][%l] %v%$");
    m_logger = spdlog::stdout_color_mt("glmmd");
    m_logger->set_level(spdlog::level::trace);
}

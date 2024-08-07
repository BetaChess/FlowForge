#include "logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace flwfrg
{
std::shared_ptr<spdlog::logger> Logger::core_logger_s;

void Logger::init()
{
	// TODO: Make better custom pattern
	spdlog::set_pattern("%^[%T](%s:%#) %n[%l]: %v%$");
	core_logger_s = spdlog::stdout_color_mt("FLOWFORGE");
	core_logger_s->set_level(spdlog::level::trace);

	FLOWFORGE_INFO("Logger initialized successfully");
}

}// namespace flwfrg
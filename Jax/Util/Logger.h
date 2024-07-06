#pragma once

#include <spdlog/spdlog.h>

#include <string>
#include <memory>
#include <vector>
#include <sstream>

namespace JAX
{
	class Logger
	{
	public:
		static void Init(const std::string& name = "JaxLogger");
		static void Destroy();
		inline static std::shared_ptr<spdlog::logger> GetLogger() { return s_Logger; }

		static void SaveLastLog();
		static void Clear();
		static const std::vector<std::string>& GetLogs();

	private:
		inline static std::shared_ptr<spdlog::logger> s_Logger;
		inline static std::vector<std::string> s_Logs;
		inline static std::ostringstream s_Stream;
	};
}

#define JAX_LOG_INFO(...)		(JAX::Logger::GetLogger()->info(__VA_ARGS__), JAX::Logger::SaveLastLog())
#define JAX_LOG_WARN(...)		(JAX::Logger::GetLogger()->warn(__VA_ARGS__), JAX::Logger::SaveLastLog())
#ifdef JAX_DEBUG
	#define JAX_LOG_DEBUG(...)		(JAX::Logger::GetLogger()->debug(__VA_ARGS__), JAX::Logger::SaveLastLog())
#else
	#define JAX_LOG_DEBUG(...)
#endif
#define JAX_LOG_ERROR(...)		(JAX::Logger::GetLogger()->error(__VA_ARGS__), JAX::Logger::SaveLastLog())
#define JAX_LOG_CRITICAL(...)	(JAX::Logger::GetLogger()->critical(__VA_ARGS__), JAX::Logger::SaveLastLog(), throw)
#define JAX_LOG_TRACE(...)		(JAX::Logger::GetLogger()->trace(__VA_ARGS__), JAX::Logger::SaveLastLog(), __VA_ARGS__))

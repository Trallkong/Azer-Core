//
// Created by Trallkong on 2026/5/1.
//

#ifndef AZER_LOGGER_H
#define AZER_LOGGER_H

#include "Base.h"
#include "spdlog/spdlog.h"

namespace azer
{
    class Logger
    {
    public:
        static void Init();

        static Ref<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
    private:
        static Ref<spdlog::logger> m_CoreLogger;
        static Ref<spdlog::logger> m_ClientLogger;
    };
}

// Engine
#define AZ_CORE_TRACE(...) ::azer::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define AZ_CORE_DEBUG(...) ::azer::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define AZ_CORE_INFO(...) ::azer::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define AZ_CORE_WARN(...) ::azer::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define AZ_CORE_ERROR(...) ::azer::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define AZ_CORE_CRITICAL(...) ::azer::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client
#define AZ_TRACE(...) ::azer::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define AZ_DEBUG(...) ::azer::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define AZ_INFO(...) ::azer::Logger::GetClientLogger()->info(__VA_ARGS__)
#define AZ_WARN(...) ::azer::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define AZ_ERROR(...) ::azer::Logger::GetClientLogger()->error(__VA_ARGS__)
#define AZ_CRITICAL(...) ::azer::Logger::GetClientLogger()->critical(__VA_ARGS__)


#endif //AZER_LOGGER_H

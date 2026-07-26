//
// Created by Trallkong on 2026/5/1.
//

#pragma once

#include "Base.h"
#include "spdlog/spdlog.h"
#include "ConsoleSink.h"

namespace Azer
{
    // Returns the shared console sink (available after Logger::Init)
    Ref<ConsoleSink> GetConsoleSink();

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
#define AZ_CORE_TRACE(...) ::Azer::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define AZ_CORE_DEBUG(...) ::Azer::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define AZ_CORE_INFO(...) ::Azer::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define AZ_CORE_WARN(...) ::Azer::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define AZ_CORE_ERROR(...) ::Azer::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define AZ_CORE_CRITICAL(...) ::Azer::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client
#define AZ_TRACE(...) ::Azer::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define AZ_DEBUG(...) ::Azer::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define AZ_INFO(...) ::Azer::Logger::GetClientLogger()->info(__VA_ARGS__)
#define AZ_WARN(...) ::Azer::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define AZ_ERROR(...) ::Azer::Logger::GetClientLogger()->error(__VA_ARGS__)
#define AZ_CRITICAL(...) ::Azer::Logger::GetClientLogger()->critical(__VA_ARGS__)



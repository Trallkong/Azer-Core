//
// Created by Trallkong on 2026/5/1.
//

#include "azpch.h"
#include "Logger.h"

#include "spdlog/sinks/stdout_color_sinks-inl.h"

namespace azer
{
    Ref<spdlog::logger> Logger::m_CoreLogger = nullptr;
    Ref<spdlog::logger> Logger::m_ClientLogger = nullptr;

    void Logger::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        m_CoreLogger = spdlog::stdout_color_mt("AZER");
        m_CoreLogger->set_level(spdlog::level::trace);

        m_ClientLogger = spdlog::stdout_color_mt("client");
        m_ClientLogger->set_level(spdlog::level::trace);
    }
};

//
// Created by Trallkong on 2026/5/1.
//

#include "azpch.h"
#include "Logger.h"

#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"

namespace azer
{
    Ref<spdlog::logger> Logger::m_CoreLogger = nullptr;
    Ref<spdlog::logger> Logger::m_ClientLogger = nullptr;

    void Logger::Init()
    {
        auto formatter = std::make_unique<spdlog::pattern_formatter>(
            "%^[%T] %n: %v%$",
            spdlog::pattern_time_type::local
        );

        m_CoreLogger = spdlog::stdout_color_mt("AZER");
        m_CoreLogger->set_level(spdlog::level::trace);
        m_CoreLogger->set_formatter(std::move(formatter));

        auto client_formatter = std::make_unique<spdlog::pattern_formatter>(
            "%^[%T] %n: %v%$",
            spdlog::pattern_time_type::local
        );

        m_ClientLogger = spdlog::stdout_color_mt("client");
        m_ClientLogger->set_level(spdlog::level::trace);
        m_ClientLogger->set_formatter(std::move(client_formatter));
    }
};

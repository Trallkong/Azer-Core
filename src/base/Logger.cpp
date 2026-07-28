//
// Created by Trallkong on 2026/5/1.
//

#include "azpch.h"
#include "Logger.h"
#include "ConsoleSink.h"

#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"

namespace Azer
{
    Ref<spdlog::logger> Logger::m_CoreLogger = nullptr;
    Ref<spdlog::logger> Logger::m_ClientLogger = nullptr;

    static Ref<ConsoleSink> s_ConsoleSink = nullptr;

    Ref<ConsoleSink> GetConsoleSink() { return s_ConsoleSink; }

    void Logger::Init()
    {
        s_ConsoleSink = std::make_shared<ConsoleSink>();

        auto formatter = std::make_unique<spdlog::pattern_formatter>(
            "%^[%T] %n: %v%$",
            spdlog::pattern_time_type::local
        );

        m_CoreLogger = std::make_shared<spdlog::logger>(
            "AZER",
            spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>(), s_ConsoleSink}
        );

        #ifdef AZ_ENABLE_DEBUG
        m_CoreLogger->set_level(spdlog::level::trace);
        #else
        m_CoreLogger->set_level(spdlog::level::info);
        #endif

        m_CoreLogger->set_formatter(std::move(formatter));

        auto client_formatter = std::make_unique<spdlog::pattern_formatter>(
            "%^[%T] %n: %v%$",
            spdlog::pattern_time_type::local
        );

        m_ClientLogger = std::make_shared<spdlog::logger>(
            "client",
            spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>(), s_ConsoleSink}
        );
        m_ClientLogger->set_level(spdlog::level::trace);
        m_ClientLogger->set_formatter(std::move(client_formatter));
    }
};

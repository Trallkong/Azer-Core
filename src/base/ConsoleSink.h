//
// Created by Aier on 2026/5/31.
//

#pragma once
#include "Base.h"
#include "spdlog/sinks/base_sink.h"
#include <mutex>
#include <vector>
#include <string>

namespace Azer
{
    struct LogEntry
    {
        spdlog::level::level_enum level;
        std::string message;
        std::string loggerName;
    };

    // A spdlog sink that stores log entries in a ring buffer for UI consumption.
    // Thread-safe.
    template<typename Mutex>
    class ConsoleSinkImpl : public spdlog::sinks::base_sink<Mutex>
    {
    public:
        explicit ConsoleSinkImpl(size_t maxEntries = 2000)
            : m_MaxEntries(maxEntries)
        {
        }

        const std::vector<LogEntry>& GetEntries() const { return m_Entries; }

        void Clear() { m_Entries.clear(); }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);

            LogEntry entry;
            entry.level = msg.level;
            entry.message = fmt::to_string(formatted);
            entry.loggerName = std::string(msg.logger_name.data(), msg.logger_name.size());

            if (m_Entries.size() >= m_MaxEntries)
                m_Entries.erase(m_Entries.begin());
            m_Entries.push_back(std::move(entry));
        }

        void flush_() override {}

    private:
        std::vector<LogEntry> m_Entries;
        size_t m_MaxEntries;
    };

    using ConsoleSink = ConsoleSinkImpl<std::mutex>;
}


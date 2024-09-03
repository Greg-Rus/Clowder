#pragma once
#include <string>
#include <vector>

enum LogType
{
    LOG_INFO,
    Log_ERROR
};

struct LogEntry
{
    LogType type;
    std::string message;
};

class Logger
{
    public:
    static std::vector<LogEntry> messages;
    static void Log(const std::string& message);
    static void Error(const std::string& message);

    private:
    static const std::string GetTimeStamp();
};
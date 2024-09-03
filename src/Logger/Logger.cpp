#include "Logger.h"
#include <iostream>
#include <ctime>

std::vector<LogEntry> Logger::messages;

void Logger::Log(const std::string &message)
{
    LogEntry logEntry;
    logEntry.type = LogType::LOG_INFO;
    logEntry.message = message;
    messages.push_back(logEntry);

    std::cout
        << "\033[1;32m"
        << "LOG: [ "
        << GetTimeStamp()
        << " ] - "
        << message
        << "\033[0m"
        << std::endl;
}

void Logger::Error(const std::string &message)
{
    LogEntry logEntry;
    logEntry.type = LogType::Log_ERROR;
    logEntry.message = message;
    messages.push_back(logEntry);

    std::cout
        << "\033[1;31m"
        << "ERR: [ "
        << GetTimeStamp()
        << " ] - "
        << message
        << "\033[0m"
        << std::endl;
}

const std::string Logger::GetTimeStamp()
{
    char s[1000];

    time_t t = time(NULL);
    struct tm *p = localtime(&t);

    strftime(s, sizeof s, "%d/%B/%Y %H:%M:%S", p);
    std::string result = std::string(s);

    return result;
}
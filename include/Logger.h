#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum class LogLevel { DEBUG, INFO, ERROR };

void setLogLevel(LogLevel level);
LogLevel getLogLevel();
void logMessage(LogLevel level, const char* file, int line, const std::string& msg);

#define LOG_DEBUG(msg) logMessage(LogLevel::DEBUG, __FILE__, __LINE__, msg)
#define LOG_INFO(msg)  logMessage(LogLevel::INFO,  __FILE__, __LINE__, msg)
#define LOG_ERROR(msg) logMessage(LogLevel::ERROR, __FILE__, __LINE__, msg)

#endif

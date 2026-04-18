#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

static LogLevel g_level = LogLevel::INFO;

void setLogLevel(LogLevel level) { g_level = level; }
LogLevel getLogLevel() { return g_level; }

void logMessage(LogLevel level, const char* file, int line, const std::string& msg) {
    if (level < g_level) return;

    const char* prefix = "";
    switch (level) {
        case LogLevel::DEBUG: prefix = "[DEBUG]"; break;
        case LogLevel::INFO:  prefix = "[INFO]";  break;
        case LogLevel::ERROR: prefix = "[ERROR]"; break;
    }

    std::cerr << prefix << " [" << file << ":" << line << "] " << msg << std::endl;
}

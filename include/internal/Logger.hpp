//============================================================================
// Copyright (c) 2024, runnindrk
//
// This file is part of Quantum Abysmal.
//
// Quantum Abysmal is free software: you can redistribute it and/or modify it
// as needed, with the intent of making it freely available to everyone.
//
// This project is in its early stages and is provided without any warranties,
// expressed or implied, including but not limited to the warranties of
// merchantability, fitness for a particular purpose, or non-infringement.
//
// Use it at your own risk, and feel free to contribute as the project evolves!
//============================================================================

#ifndef QUANTUM_ABYSMAL_INTERNAL_LOGGER_HPP
#define QUANTUM_ABYSMAL_INTERNAL_LOGGER_HPP

#include <iostream>
#include <sstream>
#include <string>

class Logger
{
  public:
    enum LogLevel
    {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

  private:
    std::ostringstream stream;
    LogLevel currentLevel;

  public:
    Logger() : currentLevel(INFO) {}

    void setLogLevel(LogLevel level)
    {
        currentLevel = level;
    }

    LogLevel getLogLevel() const
    {
        return currentLevel;
    }

    template <typename T>
    Logger& operator<<(const T& msg)
    {
        stream << msg;
        return *this;
    }

    void logMessage(const char* file, int line)
    {
        std::string logPrefix = getLogLevelPrefix(currentLevel);
        std::string logEntry = logPrefix + file + " : " + std::to_string(line) + " | " + stream.str();

        std::cout << logEntry << std::endl;

        stream.str("");
        stream.clear();
    }

  private:
    std::string getLogLevelPrefix(LogLevel level) const
    {
        switch (level)
        {
        case DEBUG:
            return "[DEBUG] ";
        case INFO:
            return "[INFO] ";
        case WARN:
            return "[WARN] ";
        case ERROR:
            return "[ERROR] ";
        default:
            return "[LOG] ";
        }
    }
};

extern Logger LOGGER;

class LogLevelStream
{
    Logger& logger;
    Logger::LogLevel level;
    const char* file;
    int line;

  public:
    LogLevelStream(Logger& log, Logger::LogLevel lvl, const char* file, int line)
        : logger(log), level(lvl), file(file), line(line)
    {
        logger.setLogLevel(level);
    }

    template <typename T>
    LogLevelStream& operator<<(const T& msg)
    {
        logger << msg;
        return *this;
    }

    ~LogLevelStream()
    {
        // Log file and line number where the log is generated
        logger.logMessage(file, line);
    }
};

#ifdef DEBUG_LOGGING_ENABLED
    #define LOG_DEBUG LogLevelStream(LOGGER, Logger::DEBUG, __FILE__, __LINE__)
#else
    #define LOG_DEBUG if(false) LogLevelStream(LOGGER, Logger::DEBUG, __FILE__, __LINE__)
#endif

#ifdef INFO_LOGGING_ENABLED
    #define LOG_INFO LogLevelStream(LOGGER, Logger::INFO, __FILE__, __LINE__)
#else
    #define LOG_INFO if(false) LogLevelStream(LOGGER, Logger::INFO, __FILE__, __LINE__)
#endif

#ifdef WARN_LOGGING_ENABLED
    #define LOG_WARN LogLevelStream(LOGGER, Logger::WARN, __FILE__, __LINE__)
#else
    #define LOG_WARN if(false) LogLevelStream(LOGGER, Logger::WARN, __FILE__, __LINE__)
#endif

#ifdef ERROR_LOGGING_ENABLED
    #define LOG_ERROR LogLevelStream(LOGGER, Logger::ERROR, __FILE__, __LINE__)
#else
    #define LOG_ERROR if(false) LogLevelStream(LOGGER, Logger::ERROR, __FILE__, __LINE__)
#endif

#endif
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

#include <fstream>
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
    std::ofstream logFile;
    LogLevel currentLevel;

  public:
    Logger(const std::string& filename) : currentLevel(INFO)
    {
        logFile.open(filename, std::ios::app);
    }

    ~Logger()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void setLogLevel(LogLevel level)
    {
        currentLevel = level;
    }

    LogLevel getLogLevel() const
    {
        return currentLevel;
    }

    template <typename T> Logger& operator<<(const T& msg)
    {
        stream << msg;
        return *this;
    }

    void logMessage()
    {
        std::string logPrefix = getLogLevelPrefix(currentLevel);
        std::string logEntry = logPrefix + stream.str();

        std::cout << logEntry << std::endl;

        if (logFile.is_open())
        {
            logFile << logEntry << std::endl;
        }

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

  public:
    LogLevelStream(Logger& log, Logger::LogLevel lvl) : logger(log), level(lvl)
    {
        logger.setLogLevel(level);
    }

    template <typename T> LogLevelStream& operator<<(const T& msg)
    {
        logger << msg;
        return *this;
    }

    ~LogLevelStream()
    {
        logger.logMessage();
    }
};

#define LOG_DEBUG LogLevelStream(LOGGER, Logger::DEBUG)
#define LOG_INFO LogLevelStream(LOGGER, Logger::INFO)
#define LOG_WARN LogLevelStream(LOGGER, Logger::WARN)
#define LOG_ERROR LogLevelStream(LOGGER, Logger::ERROR)

#endif
/**
 * @file logger.hpp
 * @brief Simple logging utility for the framework
 * 
 * This file provides a simple logging utility with different log levels
 * and output formatting.
 */

#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <mutex>

namespace rad_ml {
namespace core {

/**
 * @brief Log levels
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * @brief Simple logging utility
 */
class Logger {
public:
    /**
     * @brief Initialize the logger
     * 
     * @param level Minimum log level to display
     * @param log_to_file Whether to log to a file
     * @param file_path Path to the log file (if log_to_file is true)
     */
    static void init(LogLevel level = LogLevel::INFO, bool log_to_file = false, const std::string& file_path = "rad_ml.log") {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
        log_to_file_ = log_to_file;
        file_path_ = file_path;
        
        if (log_to_file_ && !file_stream_.is_open()) {
            file_stream_.open(file_path_, std::ios::out | std::ios::app);
        }
    }
    
    /**
     * @brief Log a debug message
     * 
     * @param message Message to log
     */
    static void debug(const std::string& message) {
        log(LogLevel::DEBUG, message);
    }
    
    /**
     * @brief Log an info message
     * 
     * @param message Message to log
     */
    static void info(const std::string& message) {
        log(LogLevel::INFO, message);
    }
    
    /**
     * @brief Log a warning message
     * 
     * @param message Message to log
     */
    static void warning(const std::string& message) {
        log(LogLevel::WARNING, message);
    }
    
    /**
     * @brief Log an error message
     * 
     * @param message Message to log
     */
    static void error(const std::string& message) {
        log(LogLevel::ERROR, message);
    }
    
    /**
     * @brief Log a critical message
     * 
     * @param message Message to log
     */
    static void critical(const std::string& message) {
        log(LogLevel::CRITICAL, message);
    }
    
    /**
     * @brief Close the logger
     */
    static void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
    }
    
private:
    /**
     * @brief Log a message with a specific level
     * 
     * @param level Log level
     * @param message Message to log
     */
    static void log(LogLevel level, const std::string& message) {
        if (level < level_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << " [" << getLevelString(level) << "] " << message;
        
        std::cout << ss.str() << std::endl;
        
        if (log_to_file_ && file_stream_.is_open()) {
            file_stream_ << ss.str() << std::endl;
        }
    }
    
    /**
     * @brief Get string representation of log level
     * 
     * @param level Log level
     * @return String representation
     */
    static std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::CRITICAL:return "CRITICAL";
            default:                return "UNKNOWN";
        }
    }
    
    // Use inline for C++17 or later to avoid duplicate symbols
    inline static LogLevel level_ = LogLevel::INFO;
    inline static bool log_to_file_ = false;
    inline static std::string file_path_ = "rad_ml.log";
    inline static std::ofstream file_stream_;
    inline static std::mutex mutex_;
};

} // namespace core
} // namespace rad_ml 
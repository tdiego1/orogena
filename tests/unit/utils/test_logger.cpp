/**************************************************************************************************/
/**
 * @file test_logger.cpp
 * @brief Unit tests for Logger utility
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include <spdlog/spdlog.h>

#include "utils/utils_logger.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <numbers>
#include <regex>

using namespace Orogena::Utils;

/**************************************************************************************************/
/**
 * @brief Test fixture for Logger tests
 *
 * @details
 * Provides setup and teardown for logger testing. Uses a custom ostream sink
 * to capture log output for verification without creating actual files.
 */
class LoggerTest : public ::testing::Test
{
  protected:
    // Called once before the first test in this test suite
    static void SetUpTestSuite()
    {
        // Drop any existing logger instance to allow initialization
        spdlog::drop("orogena");

        // Initialize logger once for all tests
        Logger::Initialize();

        // Force flush on every log message for reliable testing
        auto logger = spdlog::get("orogena");
        if (logger)
        {
            logger->flush_on(spdlog::level::trace);
        }
    }

    // Called once after the last test in this test suite
    static void TearDownTestSuite()
    {
        // Flush and drop logger
        FlushLogger();
        spdlog::drop("orogena");

        // Clean up all generated log files
        CleanupLogFiles();
    }

    // Called before each individual test
    void SetUp() override
    {
        // Flush any pending logs from previous test
        FlushLogger();
    }

    /**
     * @brief Removes all log files matching the pattern orogena_*.log
     */
    static void CleanupLogFiles()
    {
        try
        {
            for (const auto& entry : std::filesystem::directory_iterator("."))
            {
                if (entry.is_regular_file())
                {
                    std::string filename = entry.path().filename().string();
                    if (filename.starts_with("orogena_") && filename.ends_with(".log"))
                    {
                        std::filesystem::remove(entry.path());
                    }
                }
            }
        }
        // NOLINTNEXTLINE(bugprone-empty-catch)
        catch (const std::filesystem::filesystem_error&)
        {
            // Intentionally ignore cleanup errors - test cleanup failures should not cause test
            // failures
        }
    }

    /**
     * @brief Finds the most recent log file
     * @return Path to the most recent log file, or empty path if none found
     */
    static std::filesystem::path FindLatestLogFile()
    {
        std::filesystem::path           latest_file;
        std::filesystem::file_time_type latest_time{};

        for (const auto& entry : std::filesystem::directory_iterator("."))
        {
            if (entry.is_regular_file())
            {
                std::string filename = entry.path().filename().string();
                if (filename.starts_with("orogena_") && filename.ends_with(".log"))
                {
                    auto file_time = std::filesystem::last_write_time(entry.path());
                    if (latest_file.empty() || file_time > latest_time)
                    {
                        latest_file = entry.path();
                        latest_time = file_time;
                    }
                }
            }
        }

        return latest_file;
    }

    /**
     * @brief Reads contents of a file
     * @param filepath Path to file to read
     * @return File contents as string
     */
    static std::string ReadFile(const std::filesystem::path& filepath)
    {
        std::ifstream file(filepath, std::ios::in);
        if (!file.is_open())
        {
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    /**
     * @brief Flushes all logger sinks and closes file to ensure data is written to disk
     *
     * This is necessary because spdlog keeps the file open for writing, which can
     * prevent reads from seeing flushed data. We drop and re-initialize to force
     * a file close/reopen cycle.
     */
    static void FlushAndCloseLogger()
    {
        auto logger = spdlog::get("orogena");
        if (logger)
        {
            logger->flush();
        }

        // Drop the logger to close the file
        spdlog::drop("orogena");

        // Reinitialize for next operation
        Logger::Initialize();

        // Reconfigure flush behavior
        logger = spdlog::get("orogena");
        if (logger)
        {
            logger->flush_on(spdlog::level::trace);
        }
    }

    /**
     * @brief Simple flush without closing (for backwards compatibility)
     */
    static void FlushLogger()
    {
        auto logger = spdlog::get("orogena");
        if (logger)
        {
            logger->flush();
        }
    }
};

/**************************************************************************************************/
// Basic Functionality Tests
/**************************************************************************************************/

TEST_F(LoggerTest, InitializeCreatesLogFile)
{
    // Arrange & Act
    // Logger already initialized in SetUp()

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    EXPECT_FALSE(log_file.empty()) << "No log file was created";
    EXPECT_TRUE(std::filesystem::exists(log_file)) << "Log file does not exist";
}

TEST_F(LoggerTest, LogFileHasCorrectTimestampFormat)
{
    // Arrange & Act
    std::filesystem::path log_file = FindLatestLogFile();

    // Assert
    std::string filename = log_file.filename().string();
    // Format: orogena_YYYY-MM-DD_HH-MM-SS.log
    std::regex pattern(R"(orogena_\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2}\.log)");
    EXPECT_TRUE(std::regex_match(filename, pattern))
        << "Log filename does not match expected timestamp format: " << filename;
}

/**************************************************************************************************/
// Log Level Tests
/**************************************************************************************************/

TEST_F(LoggerTest, TraceMessageWrittenToFile)
{
    // Arrange - Set logger level to TRACE to allow trace messages
    Logger::SetLevel(LogLevel::TRACE);

    // Act
    Logger::Trace("Test trace message");
    FlushAndCloseLogger();

    // Assert - Check file contains trace message
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test trace message") != std::string::npos)
        << "Trace message not found in log file";
}

TEST_F(LoggerTest, DebugMessageWrittenToFile)
{
    // Arrange & Act
    Logger::Debug("Test debug message");
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test debug message") != std::string::npos)
        << "Debug message not found in log file";
}

TEST_F(LoggerTest, InfoMessageWrittenToFile)
{
    // Arrange & Act
    Logger::Info("Test info message");
    FlushAndCloseLogger(); // Close file to make writes visible

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test info message") != std::string::npos)
        << "Info message not found in log file";
}

TEST_F(LoggerTest, WarnMessageWrittenToFile)
{
    // Arrange & Act
    Logger::Warn("Test warning message");
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test warning message") != std::string::npos)
        << "Warning message not found in log file";
}

TEST_F(LoggerTest, ErrorMessageWrittenToFile)
{
    // Arrange & Act
    Logger::Error("Test error message");
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test error message") != std::string::npos)
        << "Error message not found in log file";
}

TEST_F(LoggerTest, CriticalMessageWrittenToFile)
{
    // Arrange & Act
    Logger::Critical("Test critical message");
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test critical message") != std::string::npos)
        << "Critical message not found in log file";
}

/**************************************************************************************************/
// Formatted Logging Tests
/**************************************************************************************************/

TEST_F(LoggerTest, FormattedMessageWithSingleArgument)
{
    // Arrange
    int value = 42;

    // Act
    Logger::Info("Test value: {}", value);
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Test value: 42") != std::string::npos)
        << "Formatted message not found in log file";
}

TEST_F(LoggerTest, FormattedMessageWithMultipleArguments)
{
    // Arrange
    int         x = 10;
    int         y = 20;
    std::string operation = "add";

    // Act
    Logger::Info("Operation: {} with values x={}, y={}", operation, x, y);
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Operation: add with values x=10, y=20") != std::string::npos)
        << "Formatted message not found in log file";
}

TEST_F(LoggerTest, FormattedMessageWithFloatingPoint)
{
    // Arrange
    double pi = std::numbers::pi;

    // Act
    Logger::Debug("Pi value: {:.2f}", pi);
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Pi value: 3.14") != std::string::npos)
        << "Formatted floating point message not found in log file";
}

/**************************************************************************************************/
// Log Level Filtering Tests
/**************************************************************************************************/

TEST_F(LoggerTest, SetLevelFiltersLowerPriorityMessages)
{
    // Arrange
    Logger::SetLevel(LogLevel::WARN);

    // Act
    Logger::Debug("This debug message should not appear");
    Logger::Info("This info message should not appear");
    Logger::Warn("This warning message should appear");
    Logger::Error("This error message should appear");
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);

    EXPECT_TRUE(log_contents.find("This debug message should not appear") == std::string::npos)
        << "Debug message should be filtered out";
    EXPECT_TRUE(log_contents.find("This info message should not appear") == std::string::npos)
        << "Info message should be filtered out";
    EXPECT_TRUE(log_contents.find("This warning message should appear") != std::string::npos)
        << "Warning message should be present";
    EXPECT_TRUE(log_contents.find("This error message should appear") != std::string::npos)
        << "Error message should be present";
}

TEST_F(LoggerTest, SetConsoleLevelIndependentFromFileLevel)
{
    // Arrange
    Logger::SetLevel(LogLevel::TRACE); // Logger must allow TRACE for any sink to receive it
    Logger::SetConsoleLevel(LogLevel::ERROR);
    Logger::SetFileLevel(LogLevel::TRACE);

    // Act
    Logger::Trace("Trace message");
    Logger::Debug("Debug message");
    Logger::Error("Error message");
    FlushAndCloseLogger();

    // Assert
    // All messages should be in file (TRACE level)
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);

    EXPECT_TRUE(log_contents.find("Trace message") != std::string::npos)
        << "Trace message should be in file";
    EXPECT_TRUE(log_contents.find("Debug message") != std::string::npos)
        << "Debug message should be in file";
    EXPECT_TRUE(log_contents.find("Error message") != std::string::npos)
        << "Error message should be in file";

    // Note: Console output is not tested here as it goes to stdout
}

/**************************************************************************************************/
// Convenience Alias Tests
/**************************************************************************************************/

TEST_F(LoggerTest, ConvenienceAliasWorks)
{
    // Arrange & Act - Use Orogena::Log instead of Orogena::Utils::Logger
    Orogena::Log::Info("Testing convenience alias");
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find("Testing convenience alias") != std::string::npos)
        << "Message logged via convenience alias not found";
}

/**************************************************************************************************/
// Edge Cases
/**************************************************************************************************/

TEST_F(LoggerTest, EmptyMessageLogged)
{
    // Arrange & Act
    Logger::Info("");

    // Assert - Should not crash, file should exist
    std::filesystem::path log_file = FindLatestLogFile();
    EXPECT_TRUE(std::filesystem::exists(log_file));
}

TEST_F(LoggerTest, VeryLongMessageLogged)
{
    // Arrange
    std::string long_message(10000, 'A');

    // Act
    Logger::Info("{}", long_message);
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);
    EXPECT_TRUE(log_contents.find(long_message) != std::string::npos)
        << "Long message not found in log file";
}

TEST_F(LoggerTest, SpecialCharactersInMessage)
{
    // Arrange
    std::string special_chars = "Test\n\t\r\\\"'[]<>";

    // Act - Use format string to log runtime string
    Logger::Info("{}", special_chars);

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    EXPECT_TRUE(std::filesystem::exists(log_file))
        << "Log file should exist after logging special characters";
}

/**************************************************************************************************/
// Multithreading Tests
/**************************************************************************************************/

TEST_F(LoggerTest, ConcurrentLoggingDoesNotCrash)
{
    // Arrange
    constexpr int            c_NumThreads = 10;
    constexpr int            c_MessagesPerThread = 100;
    std::vector<std::thread> threads;
    threads.reserve(c_NumThreads);

    // Act
    for (int t = 0; t < c_NumThreads; ++t)
    {
        threads.emplace_back(
            [t]()
            {
                for (int i = 0; i < c_MessagesPerThread; ++i)
                {
                    Logger::Info("Thread {} message {}", t, i);
                }
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
    FlushAndCloseLogger();

    // Assert
    std::filesystem::path log_file = FindLatestLogFile();
    std::string           log_contents = ReadFile(log_file);

    // Verify at least some messages from each thread are present
    for (int t = 0; t < c_NumThreads; ++t)
    {
        std::string thread_marker = std::format("Thread {} message", t);
        EXPECT_TRUE(log_contents.find(thread_marker) != std::string::npos)
            << "Messages from thread " << t << " not found";
    }
}

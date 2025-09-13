#pragma once

enum LoggerType
{
	LOGGER_NONE,
	LOGGER_CONSOLE,
	LOGGER_FILE
};

enum LogLevel
{
	LOG_INFO,
	LOG_DEBUG,
	LOG_WARN,
	LOG_ERROR
};

namespace Logger
{
	LoggerType GetCurrentType();
	LogLevel GetCurrentLevel();
	void SetLogLevel(LogLevel logLevel);
	void Create(LoggerType type);
	void Information(const char* string);
	void Debug(const char* string);
	void Warning(const char* string);
	void Error(const char* string);
	void Close();
}

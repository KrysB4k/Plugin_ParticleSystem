#pragma once

enum LoggerType
{
	LOGGER_NONE,
	LOGGER_CONSOLE,
	LOGGER_FILE
};

enum LogLevel
{
	LOG_TRACE,
	LOG_INFO,
	LOG_DEBUG,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
};

namespace Logger
{
	LoggerType GetCurrentType();
	LogLevel GetCurrentLevel();
	void SetLogLevel(LogLevel logLevel);
	void Create(LoggerType type);
	void Trace(const char* string);
	void Information(const char* string);
	void Debug(const char* string);
	void Warning(const char* string);
	void Error(const char* string);
	void Fatal(const char* string);
	void Close();
}

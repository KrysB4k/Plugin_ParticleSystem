#pragma once

enum LoggerType
{
	LOG_NONE,
	LOG_CONSOLE,
	LOG_FILE
};

namespace Logger
{
	LoggerType GetCurrentType();
	void Create(LoggerType type);
	void Trace(const char* string);
	void Debug(const char* string);
	void Information(const char* string);
	void Warning(const char* string);
	void Error(const char* string);
	void Fatal(const char* string);
	void Close();
}

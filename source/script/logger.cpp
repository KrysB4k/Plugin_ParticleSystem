#include "pch.h"
#include "logger.h"

namespace
{
	enum LoggerType
	{
		LOG_NONE,
		LOG_CONSOLE,
		LOG_FILE
	};

	enum ConsoleColour
	{
		COLOUR_AQUA,
		COLOUR_WHITE,
		COLOUR_YELLOW,
		COLOUR_RED
	};

	const char* LEVEL_DEBUG = "DEBUG: ";
	const char* LEVEL_INFORMATION = "INFO: ";
	const char* LEVEL_WARNING = "WARN: ";
	const char* LEVEL_ERROR = "ERROR: ";
	const char* NEWLINE = "\r\n";

	HANDLE logger;
	LoggerType current;

	void SetConsoleColour(ConsoleColour colour)
	{
		if (current == LOG_CONSOLE && logger != INVALID_HANDLE_VALUE)
		{
			switch (colour)
			{
			case COLOUR_AQUA:
				SetConsoleTextAttribute(logger, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case COLOUR_WHITE:
				SetConsoleTextAttribute(logger, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_YELLOW:
				SetConsoleTextAttribute(logger, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_RED:
				SetConsoleTextAttribute(logger, FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			}
		}
	}
}

void Logger::Create()
{
	LoggerType type;
	SYSTEMTIME time;
	char filename[96];

	if (current != LOG_NONE)
		return;
	type = LOG_CONSOLE;
	switch (type)
	{
	case LOG_CONSOLE:
		AllocConsole();
		logger = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
		if (logger != INVALID_HANDLE_VALUE)
			SetConsoleActiveScreenBuffer(logger);
		break;
	case LOG_FILE:
		GetLocalTime(&time);
		snprintf(filename, 96, "Plugin_AdvancedParticles_%04d%02d%02d_%02d%02d%02d_log.txt", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		logger = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		break;
	}
	current = type;
}

void Logger::Debug(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_AQUA);
			WriteConsole(logger, string, strlen(string), &bytes, nullptr);
			WriteConsole(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			WriteFile(logger, LEVEL_DEBUG, strlen(LEVEL_DEBUG), &bytes, nullptr);
			WriteFile(logger, string, strlen(string), &bytes, nullptr);
			WriteFile(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	}
}

void Logger::Information(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_WHITE);
			WriteConsole(logger, string, strlen(string), &bytes, nullptr);
			WriteConsole(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			WriteFile(logger, LEVEL_INFORMATION, strlen(LEVEL_INFORMATION), &bytes, nullptr);
			WriteFile(logger, string, strlen(string), &bytes, nullptr);
			WriteFile(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	}
}

void Logger::Warning(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_YELLOW);
			WriteConsole(logger, string, strlen(string), &bytes, nullptr);
			WriteConsole(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			WriteFile(logger, LEVEL_WARNING, strlen(LEVEL_WARNING), &bytes, nullptr);
			WriteFile(logger, string, strlen(string), &bytes, nullptr);
			WriteFile(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	}
}

void Logger::Error(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_RED);
			WriteConsole(logger, string, strlen(string), &bytes, nullptr);
			WriteConsole(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (logger != INVALID_HANDLE_VALUE)
		{
			WriteFile(logger, LEVEL_ERROR, strlen(LEVEL_ERROR), &bytes, nullptr);
			WriteFile(logger, string, strlen(string), &bytes, nullptr);
			WriteFile(logger, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	}
}

void Logger::Close()
{
	if (current == LOG_NONE)
		return;
	switch (current)
	{
	case LOG_CONSOLE:
		if (logger != INVALID_HANDLE_VALUE)
			CloseHandle(logger);
		FreeConsole();
		break;
	case LOG_FILE:
		if (logger != INVALID_HANDLE_VALUE)
			CloseHandle(logger);
		break;
	}
	current = LOG_NONE;
}

#include "pch.h"
#include "logger.h"

namespace
{
	enum ConsoleColour
	{
		COLOUR_WHITE,
		COLOUR_GREEN,
		COLOUR_YELLOW,
		COLOUR_RED
	};

	const char* LEVEL_INFORMATION = "INFO: ";
	const char* LEVEL_DEBUG = "DEBUG: ";
	const char* LEVEL_WARNING = "WARN: ";
	const char* LEVEL_ERROR = "ERROR: ";
	const char* NEWLINE = "\n";

	HANDLE console;
	FILE* stream;
	LoggerType currentType;
	LogLevel currentLevel;

	void SetConsoleColour(ConsoleColour colour)
	{
		if (currentType == LOGGER_CONSOLE && console != INVALID_HANDLE_VALUE)
		{
			switch (colour)
			{
			case COLOUR_WHITE:
				SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_GREEN:
				SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case COLOUR_YELLOW:
				SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_RED:
				SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			}
		}
	}
}

LoggerType Logger::GetCurrentType()
{
	return currentType;
}

LogLevel Logger::GetCurrentLevel()
{
	return currentLevel;
}

void Logger::SetLogLevel(LogLevel logLevel)
{
	currentLevel = logLevel;
}

void Logger::Create(LoggerType type)
{
	SYSTEMTIME time;
	char filename[96];
	CONSOLE_FONT_INFOEX font;
	COORD size;
	SMALL_RECT rect;

	if (currentType != LOGGER_NONE)
		return;
	currentType = type;
	switch (currentType)
	{
	case LOGGER_CONSOLE:
		AllocConsole();
		SetConsoleTitle("Plugin_ParticleSystem log");
		console = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleActiveScreenBuffer(console);
			font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
			font.nFont = 0;
			font.dwFontSize.X = 0;
			font.dwFontSize.Y = 18;
			font.FontFamily = FF_DONTCARE;
			font.FontWeight = FW_NORMAL;
			wcscpy_s(font.FaceName, L"Consolas");
			SetCurrentConsoleFontEx(console, FALSE, &font);
			size.X = 100;
			size.Y = 10000;
			SetConsoleScreenBufferSize(console, size);
			rect.Top = 0;
			rect.Left = 0;
			rect.Bottom = 24;
			rect.Right = 99;
			SetConsoleWindowInfo(console, TRUE, &rect);
		}
		break;
	case LOGGER_FILE:
		GetLocalTime(&time);
		snprintf(filename, 96, "Plugin_ParticleSystem_%04d%02d%02d_%02d%02d%02d_log.txt", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		fopen_s(&stream, filename, "w");
		break;
	}
}

void Logger::Information(const char* string)
{
	DWORD bytes;

	if (currentLevel > LOG_INFO)
		return;

	switch (currentType)
	{
	case LOGGER_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_WHITE);
			WriteConsole(console, LEVEL_INFORMATION, strlen(LEVEL_INFORMATION), &bytes, nullptr);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOGGER_FILE:
		if (stream)
		{
			fputs(LEVEL_INFORMATION, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
		}
		break;
	}
}

void Logger::Debug(const char* string)
{
	DWORD bytes;

	if (currentLevel > LOG_DEBUG)
		return;

	switch (currentType)
	{
	case LOGGER_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_GREEN);
			WriteConsole(console, LEVEL_DEBUG, strlen(LEVEL_DEBUG), &bytes, nullptr);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOGGER_FILE:
		if (stream)
		{
			fputs(LEVEL_DEBUG, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
		}
		break;
	}
}

void Logger::Warning(const char* string)
{
	DWORD bytes;

	if (currentLevel > LOG_WARN)
		return;

	switch (currentType)
	{
	case LOGGER_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_YELLOW);
			WriteConsole(console, LEVEL_WARNING, strlen(LEVEL_WARNING), &bytes, nullptr);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOGGER_FILE:
		if (stream)
		{
			fputs(LEVEL_WARNING, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
		}
		break;
	}
}

void Logger::Error(const char* string)
{
	DWORD bytes;

	switch (currentType)
	{
	case LOGGER_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_RED);
			WriteConsole(console, LEVEL_ERROR, strlen(LEVEL_ERROR), &bytes, nullptr);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOGGER_FILE:
		if (stream)
		{
			fputs(LEVEL_ERROR, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
		}
		break;
	}
}

void Logger::Close()
{
	if (currentType == LOGGER_NONE)
		return;
	switch (currentType)
	{
	case LOGGER_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
			CloseHandle(console);
		FreeConsole();
		break;
	case LOGGER_FILE:
		if (stream)
			fclose(stream);
		break;
	}
	currentType = LOGGER_NONE;
}

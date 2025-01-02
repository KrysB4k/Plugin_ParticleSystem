#include "pch.h"
#include "logger.h"

namespace
{
	enum ConsoleColour
	{
		COLOUR_AQUA,
		COLOUR_GREEN,
		COLOUR_WHITE,
		COLOUR_YELLOW,
		COLOUR_PINK,
		COLOUR_RED
	};

	const char* LEVEL_TRACE = "TRACE: ";
	const char* LEVEL_DEBUG = "DEBUG: ";
	const char* LEVEL_INFORMATION = "INFO: ";
	const char* LEVEL_WARNING = "WARN: ";
	const char* LEVEL_ERROR = "ERROR: ";
	const char* LEVEL_FATAL = "FATAL: ";
	const char* NEWLINE = "\n";

	HANDLE console;
	FILE* stream;
	LoggerType current;

	void SetConsoleColour(ConsoleColour colour)
	{
		if (current == LOG_CONSOLE && console != INVALID_HANDLE_VALUE)
		{
			switch (colour)
			{
			case COLOUR_AQUA:
				SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case COLOUR_GREEN:
				SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case COLOUR_WHITE:
				SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_YELLOW:
				SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_PINK:
				SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case COLOUR_RED:
				SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			}
		}
	}
}

void Logger::Create(LoggerType type)
{
	SYSTEMTIME time;
	char filename[96];
	CONSOLE_FONT_INFOEX font;

	if (current != LOG_NONE)
		return;
	current = type;
	switch (current)
	{
	case LOG_CONSOLE:
		AllocConsole();
		SetConsoleTitle("Plugin_AdvancedParticles log");
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
		}
		break;
	case LOG_FILE:
		GetLocalTime(&time);
		snprintf(filename, 96, "Plugin_AdvancedParticles_%04d%02d%02d_%02d%02d%02d_log.txt", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		fopen_s(&stream, filename, "w");
		break;
	}
}

void Logger::Trace(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_AQUA);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (stream)
		{
			fputs(LEVEL_TRACE, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
		}
		break;
	}
}

void Logger::Debug(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_GREEN);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (stream)
		{
			fputs(LEVEL_DEBUG, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
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
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_WHITE);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (stream)
		{
			fputs(LEVEL_INFORMATION, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
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
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_YELLOW);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
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

	switch (current)
	{
	case LOG_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_PINK);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (stream)
		{
			fputs(LEVEL_ERROR, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
		}
		break;
	}
}

void Logger::Fatal(const char* string)
{
	DWORD bytes;

	switch (current)
	{
	case LOG_CONSOLE:
		if (console != INVALID_HANDLE_VALUE)
		{
			SetConsoleColour(COLOUR_RED);
			WriteConsole(console, string, strlen(string), &bytes, nullptr);
			WriteConsole(console, NEWLINE, strlen(NEWLINE), &bytes, nullptr);
		}
		break;
	case LOG_FILE:
		if (stream)
		{
			fputs(LEVEL_FATAL, stream);
			fputs(string, stream);
			fputs(NEWLINE, stream);
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
		if (console != INVALID_HANDLE_VALUE)
			CloseHandle(console);
		FreeConsole();
		break;
	case LOG_FILE:
		if (stream)
			fclose(stream);
		break;
	}
	current = LOG_NONE;
}

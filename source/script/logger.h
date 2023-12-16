#pragma once

namespace Logger
{
	void Create();
	void Debug(const char* string);
	void Information(const char* string);
	void Warning(const char* string);
	void Error(const char* string);
	void Close();
}

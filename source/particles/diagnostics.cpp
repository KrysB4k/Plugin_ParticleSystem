#include "..\definitions\includes.h"

namespace Diagnostics
{
	double performanceMultiplier;
	std::map<int, double> values;
	std::map<int, double> peaks;
	std::map<int, const char*> labels;
}

void Diagnostics::Initialise()
{
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	performanceMultiplier = (double)1000 / frequency.QuadPart;
}

double Diagnostics::Time(void (*targetFunction)())
{
	LARGE_INTEGER startTime, endTime;

	QueryPerformanceCounter(&startTime);
	targetFunction();
	QueryPerformanceCounter(&endTime);
	return performanceMultiplier * (endTime.QuadPart - startTime.QuadPart);
}

void Diagnostics::SetLabel(int order, const char* label)
{
	labels[order] = label;
}

void Diagnostics::IncrementValue(int order, double increment)
{
	values[order] += increment;
}

void Diagnostics::SetValue(int order, double value)
{
	values[order] = value;
}

void Diagnostics::UpdatePeaks()
{
	for (const auto& [order, label] : labels)
	{
		if (peaks.find(order) == peaks.end() || peaks[order] < values[order])
			peaks[order] = values[order];
	}
}

void Diagnostics::ResetValues()
{
	values.clear();
}

void Diagnostics::ResetPeaks()
{
	peaks.clear();
}

void Diagnostics::Print()
{
	for (const auto& [order, label] : labels)
	{
		PrintString(8, (2 * order + 1) * font_height, 6, (char*)FormatString("%s: %.2f", label, values[order]), 0);
		PrintString(8, (2 * order + 2) * font_height, 6, (char*)FormatString("    peak: %.2f", peaks[order]), 0);
	}
}

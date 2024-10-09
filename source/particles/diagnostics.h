#pragma once

namespace Diagnostics
{
	void Initialise();
	double Time(void (*targetFunction)());
	void SetLabel(int order, const char* label);
	void IncrementValue(int order, double increment);
	void SetValue(int order, double value);
	void UpdatePeaks();
	void ResetValues();
	void ResetPeaks();
	void Print();
}

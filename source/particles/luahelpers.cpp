#include "../definitions/Tomb4Globals.h"
#include "../trng/trng.h"
#include "particle.h"
#include "utilities.h"

using namespace Utilities;

namespace LuaHelpers
{
	int GetInteger(int argument)
	{
		if (!Script::IsInteger(argument))
			Script::ThrowError("integer expected");
		return Script::ToInteger(argument);
	}

	bool GetBoolean(int argument)
	{
		if (!Script::IsBoolean(argument))
			Script::ThrowError("boolean expected");
		return Script::ToBoolean(argument);
	}

	float GetNumber(int argument)
	{
		if (!Script::IsNumber(argument))
			Script::ThrowError("number expected");
		return Script::ToNumber(argument);
	}

	int GetFunction(int argument)
	{
		if (!Script::IsFunction(argument))
			Script::ThrowError("function or nil expected");
		return Script::StoreFunction(argument);
	}

	const char* GetLuaString(int argument)
	{
		if (!Script::IsString(argument))
			Script::ThrowError("string expected");
		return Script::ToString(argument);
	}

	int GetTable(int argument)
	{
		if (!Script::IsTable(argument))
			Script::ThrowError("table expected");
		int length = Script::ExplodeTable(argument);
		if (!length)
			Script::ThrowError("table is empty");
		return length;
	}

	int GetArgCount(int minimum, int maximum)
	{
		int count;

		count = Script::ArgCount();
		if (count < minimum)
			Script::ThrowError(FormatString("at least %d arguments expected", minimum));
		if (count > maximum)
			Script::ThrowError(FormatString("at most %d arguments expected", maximum));
		return count;
	}

	int VerifyItemIndex(int argument)
	{
		int index;
		index = GetInteger(argument);
		if (index < 0 || index >= level_items)
			Script::ThrowError(FormatString("%d does not correspond to a valid Tomb index", index));
		return index;
	}

	int GetTombIndexByNGLEIndex(int argument)
	{
		int ngindex, index;

		ngindex = GetInteger(argument);
		if (ngindex >= 0 && ngindex < 6000)
		{
			index = FromNgleIndexToTomb4Index(ngindex);
			if (index != -1)
				return index;
		}
		Script::EmitWarning(FormatString("%d does not correspond to a valid NGLE index", ngindex));
		return -1;
	}

	float GetMathResult(int argument, float (*operation)(float))
	{
		float x, result;

		x = GetNumber(argument);
		result = operation(x);
		if (!isnan(x) && isnan(result))
			Script::EmitWarning("the operation resulted in a NaN");
		return result;
	}

	float GetMathResult(int firstArgument, int secondArgument, float (*operation)(float, float))
	{
		float x, y, result;

		x = GetNumber(firstArgument);
		y = GetNumber(secondArgument);
		result = operation(x, y);
		if (!isnan(x) && !isnan(y) && isnan(result))
			Script::EmitWarning("the operation resulted in a NaN");
		return result;
	}

	int GetClampedInteger(int argument, int min, int max, bool throwBoundsError)
	{
		int x;

		x = GetInteger(argument);
		if (x < min)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%d is less than the minimum of %d, aborting", x, min));
			Script::EmitWarning(FormatString("%d is less than the minimum of %d, clamping to minimum", x, min));
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%d is greater than the maximum of %d, aborting", x, max));
			Script::EmitWarning(FormatString("%d is greater than the maximum of %d, clamping to maximum", x, max));
			return max;
		}
		return x;
	}

	float GetClampedNumber(int argument, float min, float max, bool throwBoundsError)
	{
		float x;

		x = GetNumber(argument);
		if (x < min)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%f is less than the minimum of %f, aborting", x, min));
			Script::EmitWarning(FormatString("%f is less than the minimum of %f, clamping to minimum", x, min));
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%f is greater than the maximum of %f, aborting", x, max));
			Script::EmitWarning(FormatString("%f is greater than the maximum of %f, clamping to maximum", x, max));

			return max;
		}
		return x;
	}

	int GetConstrainedInteger(int argument, int defaultValue, int count, ...)
	{
		va_list args;
		int x;
		bool present;

		x = GetInteger(argument);
		present = false;
		va_start(args, count);
		for (int i = 0; i < count; i++)
		{
			if (x == va_arg(args, int))
			{
				present = true;
				break;
			}
		}
		va_end(args);
		if (!present)
		{
			Script::EmitWarning(FormatString("%d is not in the list of allowed values", x));
			return defaultValue;
		}
		return x;
	}

	void CheckCaller(int callers, const char* function)
	{
		if (!(Particles::caller & callers))
			Script::ThrowError(FormatString("calling function \"%s\" is forbidden in this phase", function));
	}

	void CheckFieldCaller(int callers, const char* field)
	{
		if (!(Particles::caller & callers))
			Script::ThrowError(FormatString("assignment to field \"%s\" is forbidden in this phase", field));
	}

	void ReadOnlyFieldError(const char* field)
	{
		Script::ThrowError(FormatString("field \"%s\" is read-only and cannot be assigned to", field));
	}

	Vector3f GetItemPos(int argument)
	{
		auto item = &items[VerifyItemIndex(argument)];
		return Vector3f(item->pos.xPos, item->pos.yPos, item->pos.zPos);
	}

	void AssignGroupID(Particles::ParticleGroup* group, int id)
	{
		if (!group)
			return;
		if (Particles::groupIds[id])
			Script::ThrowError(FormatString("ID = %d is already assigned to a different group", id));

		Particles::groupIds[id] = group;
	}
}
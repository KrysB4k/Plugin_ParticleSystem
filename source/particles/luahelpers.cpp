#include "../definitions/Tomb4Globals.h"
#include "../trng/trng.h"
#include "particle.h"
#include "utilities.h"
#include "logger.h"

using namespace Utilities;

namespace LuaHelpers
{
	int GetInteger(int argument)
	{
		if (Script::IsInteger(argument))
			return Script::ToInteger(argument);
		if (Script::IsNumber(argument))
		{
			float number = Script::ToNumber(argument);
			if (!isfinite(number))
				Script::Throw("number is invalid");
			return SaturateRound<int>(number);
		}
		Script::Throw("number expected");
	}

	bool GetBoolean(int argument)
	{
		if (!Script::IsBoolean(argument))
			Script::Throw("boolean expected");
		return Script::ToBoolean(argument);
	}

	float GetNumber(int argument)
	{
		if (Script::IsNumber(argument))
		{
			float number = Script::ToNumber(argument);
			if (!isfinite(number))
				Script::Throw("number is invalid");
			return number;
		}
		if (Script::IsInteger(argument))
			return (float)Script::ToInteger(argument);
		Script::Throw("number expected");
	}

	int GetFunction(int argument)
	{
		if (!Script::IsFunction(argument))
			Script::Throw("function or nil expected");
		return Script::StoreFunction(argument);
	}

	const char* GetLuaString(int argument)
	{
		if (!Script::IsString(argument))
			Script::Throw("string expected");
		return Script::ToString(argument);
	}

	int GetTable(int argument)
	{
		if (!Script::IsTable(argument))
			Script::Throw("table expected");
		int length = Script::ExplodeTable(argument);
		if (!length)
			Script::Throw("table is empty");
		return length;
	}

	int GetArgCount(int minimum, int maximum)
	{
		int count;

		count = Script::ArgCount();
		if (count < minimum)
			Script::Throw(FormatString("at least %d arguments expected", minimum));
		if (count > maximum)
			Script::Throw(FormatString("at most %d arguments expected", maximum));
		return count;
	}

	int VerifyItemIndex(int argument)
	{
		int index;
		index = GetInteger(argument);
		if (index < 0 || index >= level_items)
			Script::Throw(FormatString("%d does not correspond to a valid Tomb index", index));
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
		Script::EmitFailure(FormatString("%d does not correspond to a valid NGLE index", ngindex), Logger::Warning);
		return -1;
	}

	int GetClampedInteger(int argument, int min, int max, bool throwBoundsError)
	{
		int x;

		x = GetInteger(argument);
		if (x < min)
		{
			if (throwBoundsError)
				Script::Throw(FormatString("%d is less than the minimum of %d, aborting", x, min));
			Script::EmitFailure(FormatString("%d is less than the minimum of %d, clamping to minimum", x, min), Logger::Warning);
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::Throw(FormatString("%d is greater than the maximum of %d, aborting", x, max));
			Script::EmitFailure(FormatString("%d is greater than the maximum of %d, clamping to maximum", x, max), Logger::Warning);
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
				Script::Throw(FormatString("%f is less than the minimum of %f, aborting", x, min));
			Script::EmitFailure(FormatString("%f is less than the minimum of %f, clamping to minimum", x, min), Logger::Warning);
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::Throw(FormatString("%f is greater than the maximum of %f, aborting", x, max));
			Script::EmitFailure(FormatString("%f is greater than the maximum of %f, clamping to maximum", x, max), Logger::Warning);

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
			Script::EmitFailure(FormatString("%d is not in the list of allowed values", x), Logger::Warning);
			return defaultValue;
		}
		return x;
	}

	void CheckCaller(int callers, const char* function)
	{
		if (!(Particles::GetCaller() & callers))
			Script::Throw(FormatString("calling function '%s' is forbidden in this phase", function));
	}

	void CheckFieldCaller(int callers, const char* field)
	{
		if (!(Particles::GetCaller() & callers))
			Script::Throw(FormatString("assignment to field '%s' is forbidden in this phase", field));
	}

	void ReadOnlyFieldError(const char* field)
	{
		Script::Throw(FormatString("field '%s' is read-only and cannot be assigned to", field));
	}

	int ThrowArithmetic()
	{
		Script::Throw("attempt to perform arithmetic on an unsupported object");
	}

	int ThrowArithmeticFunction()
	{
		Script::Throw("attempt to perform arithmetic on a built-in function");
	}

	Vector3f GetItemPos(int argument)
	{
		auto item = &items[VerifyItemIndex(argument)];
		return Vector3f((float)item->pos.xPos, (float)item->pos.yPos, (float)item->pos.zPos);
	}

	const char* GetBoundedLuaString(int argument, unsigned int length)
	{
		auto string = GetLuaString(argument);
		if (strlen(string) > length)
			Script::Throw(FormatString("string %s contains more than %d characters", string, length));
		return string;
	}

	void RequireModule(int argument)
	{
		Particles::CallerGuard guard(FUNCTION_MODULE);
		auto string = GetBoundedLuaString(argument, 50);
		Logger::Information(FormatString("Loading module: %s", string));
		Script::PreFunctionLoop();
		if (!Script::Require(string))
		{
			if (!GetScriptIntegrity())
				Script::EmitFailure(FormatString("cannot load '%s' module", string), Logger::Error);
			else
				ExitSystem(FormatString("Module '%s' cannot be found.", string));
		}
		Script::PostFunctionLoop();
		int last = Particles::Module::GetLastModule();
		if (last != -1 && Particles::Module::modules[last].createdInCurrentModule)
		{
			Particles::Module::modules[last].createdInCurrentModule = false;
			Script::PushData(&Particles::Module::modules[last]);
		}
		else
			Script::PushNil();
	}

	void CheckModuleParameter(int argument)
	{
		if (!Script::IsBoolean(argument) && !Script::IsNumber(argument) && !Script::IsInteger(argument) && !Script::IsString(argument))
			Script::Throw("boolean, number or string expected");
	}

	void CheckParticleData(int argument)
	{
		if (!Script::IsBoolean(argument) && !Script::IsNumber(argument) && !Script::IsInteger(argument) && !Script::IsNil(argument))
			Script::Throw("boolean, number or nil expected");
	}

	Particles::BoundFunction* GetBoundFunction(int index)
	{
		if (index < 1)
		{
			Script::EmitFailure(FormatString("index %d is less than the minimum of %d", index, 1), Logger::Error);
			return nullptr;
		}
		if (index > MAX_FUNCREFS)
		{
			Script::EmitFailure(FormatString("index %d is greater than the maximum of %d", index, MAX_FUNCREFS), Logger::Error);
			return nullptr;
		}
		auto function = &Particles::BoundFunction::functionRefs[index - 1];
		if (function->ref == SCRIPT_REFNIL)
		{
			Script::EmitFailure(FormatString("index %d is not bound to a Lua function", index), Logger::Error);
			return nullptr;
		}
		return function;
	}

	bool GetScriptIntegrity()
	{
		if (LocalScriptIntegrityDefined)
			return LocalScriptIntegrity;

		if (GlobalScriptIntegrityDefined)
			return GlobalScriptIntegrity;

		return false;
	}

	void ExitSystem(const char* message)
	{
		MessageBox(NULL, message, "Plugin_ParticleSystem", MB_ICONWARNING);
		TerminateProcess(GetCurrentProcess(), 0);
	}
}
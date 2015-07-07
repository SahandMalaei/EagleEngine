#include "EagleScript.h"

using namespace EagleScript;
using namespace EagleAssembler;
using namespace EagleVirtualMachine;
using namespace EagleScriptCompiler;

int globalVariable = 120;

namespace EagleScript
{
	int initializeEagleScript()
	{
		registerDefaultFunctions();

		return 1;
	}
};
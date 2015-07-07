#ifndef _PARSER_H
#define _PARSER_H 1

#include "../EagleScript.h"

using namespace EagleScript;

#define ES_SCOPE_GLOBAL 0

namespace EagleScriptCompiler
{
	bool readToken(Token requiredToken);

	bool parseSourceCode();

	void shutdownParser();

	struct LoopStackElement
	{
		char type;

		int startTargetIndex;
		int endTargetIndex;
	};
};

#endif
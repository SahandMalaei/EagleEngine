#ifndef _COMPILER_H
#define _COMPILER_H 1

#include "../EagleScript.h"

#define ES_MAX_SOURCE_LINE_LENGTH 1024
#define ES_MAX_IDENTIFIER_LENGTH 256

#define ES_PRELOAD_FUNCTION_NAME "Preload"
#define ES_INITIALIZE_FUNCTION_NAME "Initialize"
#define ES_UPDATE_FUNCTION_NAME "Update"
#define ES_RENDER_FUNCTION_NAME "Render"
#define ES_END_FUNCTION_NAME "End"

#define ES_COMPILER_VERSION_MAJOR 0
#define ES_COMPILER_VERSION_MINOR 2

#define ES_SCOPE_GLOBAL 0

#define ES_SYMBOL_TYPE_VARIABLE 0x0000
#define ES_SYMBOL_TYPE_PARAMETER 0x0001

#define ES_JUMP_TARGET_PREFIX "Label"

using namespace EagleScript;

namespace EagleScriptCompiler
{
	struct ScriptHeader
	{
		int stackSize;
		int globalDataSize;
		
		bool isInitializeFunctionPresent;
		short initializeFunctionIndex;

		bool isUpdateFunctionPresent;
		short updateFunctionIndex;

		bool isRenderFunctionPresent;
		short renderFunctionIndex;

		bool isEndFunctionPresent;
		short endFunctionIndex;
	};

	struct IncludeData
	{
		char *fileAddress;
		LinkedList sourceCode;
	};

	extern EagleScript::LinkedList sourceCode;
	extern EagleScript::LinkedList functionTable;
	extern EagleScript::LinkedList symbolTable;
	extern EagleScript::LinkedList stringTable;

	extern ScriptHeader scriptHeader;

	extern int functionParameterListIndex;

	extern LinkedList includeDataList;

	extern std::string rootAddress;

	bool compileESFile(EagleVirtualMachine::Script *script, char *inputFileAddress, char *outputFileAddress, bool optimize, bool outputEASMAndEVEFiles, bool addComments);
	bool compileESString(EagleVirtualMachine::Script *script, std::string scriptText, bool optimize);
	void initializeCompiler();
	bool loadSourceFile(char *fileAddress);
	bool includeSourceFile(char *fileAddress, int mainSourceFileLineIndex);
	void preprocessSourceFile();
	bool compileSourceFile();
	void emitCode(char *fileAddress, bool addComments);
	void shutdownCompiler();
};

#endif
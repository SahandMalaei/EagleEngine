#ifndef _TABLES_H
#define _TABLES_H 1

#include "../EagleScript.h"

using namespace EagleScript;

namespace EagleScriptCompiler
{
	struct SymbolNode
	{
		int index;
		char identifier[ES_MAX_IDENTIFIER_LENGTH];
		int size;
		int scope;
		int type;
		int parameterIndex;
		int stackIndex;
	};

	int addSymbol(char *identifier, int size, int scope, int type, int parameterIndex = 0);
	SymbolNode *getSymbolByIdentifier(char *identifier, int scope);
	SymbolNode *getSymbolByIndex(int index);
	int getSymbolSizeByIdentifier(char *identifier, int scope);

	struct FunctionNode
	{
		int index;
		char name[ES_MAX_IDENTIFIER_LENGTH];
		bool hasBody;
		int parameterCount;
		int prototypeParameterCount;
		int localDataSize;

		LinkedList iCodeStream;

		void deleteInstruction(int instructionIndex);
	};

	int addFunction(char *name);
	FunctionNode *getFunctionByName(char *name);
	FunctionNode *getFunctionByNameCaseInsensitive(char *name);
	FunctionNode *getFunctionByIndex(int index);
	std::string getHostFunctionIdentifier(int index);
	int getHostFunctionIndex(std::string identifier);
	bool isHostFunctionPresent(std::string identifier);
	void setFunctionParameterCount(int index, int parameterCount);
};

#endif
#include "../EagleScript.h"

using namespace EagleScript;

namespace EagleScriptCompiler
{
	int globalVariableTopIndex = 0;

	// Symbol table

	int addSymbol(char *identifier, int size, int scope, int type, int parameterIndex)
	{
		if(getSymbolByIdentifier(identifier, scope))
		{
			return -1;
		}

		SymbolNode *newSymbol = new SymbolNode();

		strcpy(newSymbol->identifier, identifier);
		newSymbol->scope = scope;
		newSymbol->size = size;
		newSymbol->type = type;
		newSymbol->index = symbolTable.addNode(newSymbol);

		if(scope != ES_SCOPE_GLOBAL)
		{
			FunctionNode *function = getFunctionByIndex(scope);

			if(type == ES_SYMBOL_TYPE_PARAMETER)
			{
				int stackIndex = -(function->localDataSize - function->parameterCount + 2 + parameterIndex);

				function->parameterCount++;
				newSymbol->parameterIndex = parameterIndex;
				newSymbol->stackIndex = stackIndex;
			}
			else
			{
				int stackIndex = -(function->localDataSize + 2);
				newSymbol->stackIndex = stackIndex;
			}

			function->localDataSize += size;
		}
		else
		{
			newSymbol->stackIndex = globalVariableTopIndex;
			globalVariableTopIndex += size;

			scriptHeader.globalDataSize += size;
		}

		return newSymbol->index;
	}

	SymbolNode *getSymbolByIdentifier(char *identifier, int scope)
	{
		LinkedListNode *currentNode = symbolTable.head;
		SymbolNode *currentSymbol;

		for(int i = 0; i < symbolTable.nodeCount; ++i)
		{
			currentSymbol = (SymbolNode *)currentNode->data;

			if(currentSymbol)
			{
				if(strcmp(currentSymbol->identifier, identifier) == 0)
				{
					if(currentSymbol->scope == scope || currentSymbol->scope == ES_SCOPE_GLOBAL)
					{
						return currentSymbol;
					}
				}
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	SymbolNode *getSymbolByIndex(int index)
	{
		if(symbolTable.nodeCount == 0) return 0;

		LinkedListNode *currentNode = symbolTable.head;

		for(int i = 0; i < symbolTable.nodeCount; ++i)
		{
			SymbolNode *currentSymbol = (SymbolNode *)currentNode->data;

			if(currentSymbol->index == index)
			{
				return currentSymbol;
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	int getSymbolSizeByIdentifier(char *identifier, int scope)
	{
		SymbolNode *symbol = getSymbolByIdentifier(identifier, scope);

		if(!symbol)
		{
			return 0;
		}

		return symbol->size;
	}

	// Function table

	int addFunction(char *name)
	{
		if(getFunctionByName(name)) 
		{
			return -1;
		}
		else if(isHostFunctionPresent(name))
		{
			return -2;
		}

		FunctionNode *newFunction = new FunctionNode();

		strcpy(newFunction->name, name);
		newFunction->index = functionTable.addNode(newFunction) + 1;
		newFunction->parameterCount = 0;
		newFunction->hasBody = 0;
		newFunction->prototypeParameterCount = 0;
		newFunction->localDataSize = 0;

		if(_stricmp(name, ES_INITIALIZE_FUNCTION_NAME) == 0)
		{
			scriptHeader.isInitializeFunctionPresent = 1;
			scriptHeader.initializeFunctionIndex = newFunction->index;
		}
		else if(_stricmp(name, ES_UPDATE_FUNCTION_NAME) == 0)
		{
			scriptHeader.isUpdateFunctionPresent = 1;
			scriptHeader.updateFunctionIndex = newFunction->index;
		}
		if(_stricmp(name, ES_RENDER_FUNCTION_NAME) == 0)
		{
			scriptHeader.isRenderFunctionPresent = 1;
			scriptHeader.renderFunctionIndex = newFunction->index;
		}
		else if(_stricmp(name, ES_END_FUNCTION_NAME) == 0)
		{
			scriptHeader.isEndFunctionPresent = 1;
			scriptHeader.endFunctionIndex = newFunction->index;
		}

		return newFunction->index;
	}

	FunctionNode *getFunctionByName(char *name)
	{
		FunctionNode *currentFunction;

		LinkedListNode *nodeIterator0 = functionTable.head;

		for(int i = 1; i < functionTable.nodeCount + 1; ++i)
		{
			currentFunction = (FunctionNode *)nodeIterator0->data;

			if(currentFunction)
			{
				if(strcmp(currentFunction->name, name) == 0)
				{
					return currentFunction;
				}
			}

			nodeIterator0 = nodeIterator0->next;
		}

		return 0;
	}

	FunctionNode *getFunctionByNameCaseInsensitive(char *name)
	{
		FunctionNode *currentFunction;

		LinkedListNode *nodeIterator0 = functionTable.head;

		for(int i = 0; i < functionTable.nodeCount; ++i)
		{
			currentFunction = (FunctionNode *)nodeIterator0->data;

			if(currentFunction)
			{
				if(_stricmp(currentFunction->name, name) == 0)
				{
					return currentFunction;
				}
			}

			nodeIterator0 = nodeIterator0->next;
		}

		return 0;
	}

	FunctionNode *getFunctionByIndex(int index)
	{
		if(index < 1 || index > functionTable.nodeCount)
		{
			return 0;
		}

		LinkedListNode *currentNode = functionTable.head;

		for(int i = 0; i < functionTable.nodeCount; ++i)
		{
			FunctionNode *currentFunction = (FunctionNode *)currentNode->data;

			if(currentFunction->index == index) 
			{
				return currentFunction;
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	std::string getHostFunctionIdentifier(int index)
	{
		int vectorSize = EagleVirtualMachine::virtualMachine.m_hostFunctionList.size();

		if(index < 0 || index > vectorSize - 1)
		{
			return "";
		}

		return EagleVirtualMachine::virtualMachine.m_hostFunctionList[index].identifier;
	}

	int getHostFunctionIndex(std::string identifier)
	{
		int arraySize = virtualMachine.m_hostFunctionList.size();

		for(int i = 0; i < arraySize; ++i)
		{
			if(identifier == virtualMachine.m_hostFunctionList[i].identifier)
			{
				return i;
			}
		}

		return -1;
	}

	bool isHostFunctionPresent(std::string identifier)
	{
		int arraySize = virtualMachine.m_hostFunctionList.size();

		for(int i = 0; i < arraySize; ++i)
		{
			if(identifier == virtualMachine.m_hostFunctionList[i].identifier)
			{
				return 1;
			}
		}

		return 0;
	}

	void setFunctionParameterCount(int index, int parameterCount)
	{
		FunctionNode *function = getFunctionByIndex(index);

		if(!function)
		{
			return;
		}

		function->parameterCount = parameterCount;
	}

	void FunctionNode::deleteInstruction(int instructionIndex)
	{
		ICodeNode *node = (ICodeNode *)iCodeStream.findNode(instructionIndex)->data;

		node->instruction.operandList.cleanup();

		iCodeStream.deleteNode(instructionIndex);
	}
};
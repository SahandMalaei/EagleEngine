#include "EagleScript.h"

using namespace EagleScript;

namespace EagleAssembler
{
	char **sourceCode = 0;
	int sourceCodeLineCount;

	InstructionLookup instructionTable[MAX_INSTRUCTION_LOOKUP_COUNT];

	Instruction *instructionStream;
	int instructionStreamSize;

	ScriptHeader scriptHeader;

	LinkedList stringTable;
	LinkedList functionTable;
	LinkedList symbolTable;
	LinkedList labelTable;

	Lexer lexer;

	bool isSetStackSizeFound;

	bool isLocalScopeActive;
	//FunctionNode *currentFunction;
	int currentFunctionIndex;
	char currentFunctionName[EASM_MAX_IDENTIFIER_LENGTH];
	int currentFunctionParameterCount;
	int currentFunctionLocalDataSize;

	InstructionLookup currentInstruction;
	int currentInstructionIndex;

	unsigned long stringTableSecurityCode = 0;

	// Host interface

	//GlobalVariable *globalVariableList[4096];
	std::vector<GlobalVariable> globalVariableList;
	short globalVariableCount;

	int addToStringTable(char *s)
	{
		LinkedListNode *node = stringTable.head;

		for(int i = 0; i < stringTable.nodeCount; ++i)
		{
			if(strcmp((char *)node->data, s) == 0)
			{
				return i;
			}

			node = node->next;
		}

		char *str = new char[strlen(s) + 1];
		strcpy(str, s);

		return stringTable.addNode(str);
	}

	FunctionNode *getFunctionByName(char *name)
	{
		if(!functionTable.nodeCount)
		{
			return 0;
		}

		LinkedListNode *currentNode = functionTable.head;

		for(int i = 0; i < functionTable.nodeCount; ++i)
		{
			FunctionNode *currentFunction = (FunctionNode *)currentNode->data;

			if(strcmp(currentFunction->name, name) == 0)
			{
				return currentFunction;
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	int addToFunctionTable(char *name, int entryPoint)
	{
		if(getFunctionByName(name))
		{
			return -1;
		}

		FunctionNode *newFunction = new FunctionNode();
		strcpy(newFunction->name, name);
		newFunction->entryPoint = entryPoint;

		int index = functionTable.addNode(newFunction);
		newFunction->index = index;

		return index;
	}

	void setFunctionInformation(char *name, int parameterCount, int localDataSize)
	{
		FunctionNode *functionNode = getFunctionByName(name);
		functionNode->parameterCount = parameterCount;
		functionNode->localDataSize = localDataSize;
	}

	SymbolNode *getSymbolByIdentifier(char *identifier, int functionIndex)
	{
		LinkedListNode *currentNode = symbolTable.head;

		for(int i = 0; i < symbolTable.nodeCount; ++i)
		{
			SymbolNode *currentSymbol = (SymbolNode *)currentNode->data;

			if(strcmp(currentSymbol->identifier, identifier) == 0)
			{
				if(currentSymbol->functionIndex == functionIndex || currentSymbol->stackIndex > -1)
				{
					return currentSymbol;
				}
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	int addToSymbolTable(char *identifier, int size, int stackIndex, int functionIndex)
	{
		if(getSymbolByIdentifier(identifier, functionIndex))
		{
			return -1;
		}

		SymbolNode *newSymbol = new SymbolNode();
		strcpy(newSymbol->identifier, identifier);
		newSymbol->size = size;
		newSymbol->stackIndex = stackIndex;
		newSymbol->functionIndex = functionIndex;

		//debugMessage("Adding symbol...");
		//debugMessage("	Identifier : " + (string)identifier);
		//debugMessage("	Stack index : " + INT_TO_STRING(stackIndex));
		//debugMessage("	Function index : " + INT_TO_STRING(functionIndex));
		//message("");

		//newSymbol->dimensionCount = dimensionCount;
		//newSymbol->dimensions = new int[dimensionCount];
		//for(int i = 0; i < dimensionCount; ++i)
		//{
		//newSymbol->dimensions[i] = dimensions[i];
		//}

		int index = symbolTable.addNode(newSymbol);
		newSymbol->index = index;

		return index;
	}

	int getSymbolStackIndexByIdentifier(char *identifier, int functionIndex)
	{
		SymbolNode *symbol = getSymbolByIdentifier(identifier, functionIndex);

		return symbol->stackIndex;
	}

	int getSymbolSizeByIdentifier(char *identifier, int functionIndex)
	{
		SymbolNode *symbol = getSymbolByIdentifier(identifier, functionIndex);

		return symbol->size;
	}

	LabelNode *getLabelByIdentifier(char *identifier, int functionIndex)
	{
		if(!labelTable.nodeCount)
		{
			return 0;
		}

		LinkedListNode *currentNode = labelTable.head;

		for(int i = 0; i < labelTable.nodeCount; ++i)
		{
			LabelNode *currentLabel = (LabelNode *)currentNode->data;

			if(strcmp(currentLabel->identifier, identifier) == 0)
			{
				if(currentLabel->functionIndex == functionIndex)
				{
					return currentLabel;
				}
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	int addToLabelTable(char *identifier, int targetInstructionIndex, int functionIndex)
	{
		if(getLabelByIdentifier(identifier, functionIndex))
		{
			return -1;
		}

		LabelNode *newLabel = new LabelNode();
		strcpy(newLabel->identifier, identifier);
		newLabel->targetInstructionIndex = targetInstructionIndex + 1;
		newLabel->functionIndex = functionIndex;

		int index = labelTable.addNode(newLabel);
		newLabel->index = index;

		return index;
	}

	int addInstructionLookup(char *mnemonic, int opcode, int operandCount)
	{
		static int instructionIndex = 0;

		if(instructionIndex == MAX_INSTRUCTION_LOOKUP_COUNT) return -1;

		strcpy(instructionTable[instructionIndex].mnemonic, mnemonic);
		_strupr(instructionTable[instructionIndex].mnemonic);

		instructionTable[instructionIndex].opcode = opcode;
		instructionTable[instructionIndex].operandCount = operandCount;
		instructionTable[instructionIndex].operandList = new OperandType[operandCount];

		instructionIndex++;

		return instructionIndex - 1;
	}

	void setOperandType(int instructionIndex, int operandIndex, OperandType operandType)
	{
		instructionTable[instructionIndex].operandList[operandIndex] = operandType;
	}

	InstructionLookup *getInstructionByMnemonic(char *mnemonic)
	{
		if(mnemonic[0] == '\0') return 0;
		char mnemonicUpper[MAX_INSTRUCTION_MNEMONIC_LENGTH + 1];
		strcpy(mnemonicUpper, mnemonic);
		_strupr(mnemonicUpper);

		for(int i = 0; i < MAX_INSTRUCTION_LOOKUP_COUNT; ++i)
		{
			if(strcmp(instructionTable[i].mnemonic, mnemonicUpper) == 0)
			{
				return &instructionTable[i];
			}
		}

		return 0;
	}

	void initializeInstructionTable()
	{
		int instructionIndex;

		// Revision 0 instructions

		{
			// Memory

			instructionIndex = addInstructionLookup("Mov", ES_OPCODE_MOV, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			// Arithmetic

			instructionIndex = addInstructionLookup("Add", ES_OPCODE_ADD, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Sub", ES_OPCODE_SUB, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Mul", ES_OPCODE_MUL, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Div", ES_OPCODE_DIV, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Mod", ES_OPCODE_MOD, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Exp", ES_OPCODE_EXP, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Neg", ES_OPCODE_NEG, 1);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Inc", ES_OPCODE_INC, 1);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Dec", ES_OPCODE_DEC, 1);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			// Bitwise

			instructionIndex = addInstructionLookup("And", ES_OPCODE_AND, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Or", ES_OPCODE_OR, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("XOr", ES_OPCODE_XOR, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Not", ES_OPCODE_NOT, 1);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("ShL", ES_OPCODE_SHL, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("ShR", ES_OPCODE_SHR, 2);
			setOperandType(instructionIndex, OPERAND_POSITION_DESTINATION, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, OPERAND_POSITION_SOURCE, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			// String Processing

			instructionIndex = addInstructionLookup("Concat", ES_OPCODE_CONCAT, 2);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("GetChar", ES_OPCODE_GETCHAR, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("SetChar", ES_OPCODE_SETCHAR, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			// Conditional

			instructionIndex = addInstructionLookup("Jmp", ES_OPCODE_JMP, 1);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_LINELABEL);

			instructionIndex = addInstructionLookup("JE", ES_OPCODE_JE, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_LINELABEL);

			instructionIndex = addInstructionLookup("JNE", ES_OPCODE_JNE, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_LINELABEL);

			instructionIndex = addInstructionLookup("JG", ES_OPCODE_JG, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_LINELABEL);

			instructionIndex = addInstructionLookup("JL", ES_OPCODE_JL, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_LINELABEL);

			instructionIndex = addInstructionLookup("JGE", ES_OPCODE_JGE, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_LINELABEL);

			instructionIndex = addInstructionLookup("JLE", ES_OPCODE_JLE, 3);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 2, OPERAND_FLAG_TYPE_LINELABEL);

			// Stack Interface

			instructionIndex = addInstructionLookup("Push", ES_OPCODE_PUSH, 1);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			instructionIndex = addInstructionLookup("Pop", ES_OPCODE_POP, 1);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);

			// Function Interface

			instructionIndex = addInstructionLookup("Call", ES_OPCODE_CALL, 1 + ES_MAX_FUNCTION_PARAMETER_COUNT);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_FUNCTION_NAME);
			for(int i = 1; i < ES_MAX_FUNCTION_PARAMETER_COUNT + 1; ++i)
			{
				setOperandType(instructionIndex, i, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			}
			instructionTable[instructionIndex].operandCount = 1;

			instructionIndex = addInstructionLookup("Return", ES_OPCODE_RETURN, 0);

			instructionIndex = addInstructionLookup("CallHost", ES_OPCODE_CALLHOST, 1 + MAX_CALL_HOST_PARAMTETER_COUNT);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_HOST_API_CALL);

			for(int i = 1; i < MAX_CALL_HOST_PARAMTETER_COUNT + 1; ++i)
			{
				setOperandType(instructionIndex, i, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			}
			//instructionTable[instructionIndex].operandCount = 1;
		}

		// Revision 1 instructions

		{
			instructionIndex = addInstructionLookup("GetHostGlobal", ES_OPCODE_GETHOSTGLOBAL, 2);
			setOperandType(instructionIndex, 0, OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
			setOperandType(instructionIndex, 1, OPERAND_FLAG_TYPE_INT | OPERAND_FLAG_TYPE_FLOAT | OPERAND_FLAG_TYPE_STRING | OPERAND_FLAG_TYPE_MEMORY_REFRENCE | OPERAND_FLAG_TYPE_REGISTER);
		}
	}

	// Lexing and Tokenizing

	void stripComments(char *sourceLine)
	{
		bool inString = 0;

		for(int i = 0; i < strlen(sourceLine); ++i)
		{
			if(sourceLine[i] == '"')
			{
				inString = !inString;

				continue;
			}

			if(sourceLine[i] == ';')
			{
				if(!inString)
				{
					sourceLine[i] = '\n';
					sourceLine[i + 1] = '\0';

					break;
				}
			}
		}
	}

	void trimWhitespace(char *s)
	{
		int stringLength = strlen(s);
		int padLength;
		int currentCharIndex;

		if(stringLength > 1)
		{
			for(currentCharIndex = 0; currentCharIndex < stringLength; ++currentCharIndex)
			{
				if(!isCharWhitespace(s[currentCharIndex])) break;
			}

			padLength = currentCharIndex;
			if(padLength)
			{
				for(; currentCharIndex < stringLength; ++currentCharIndex)
				{
					s[currentCharIndex - padLength] = s[currentCharIndex];
				}

				for(currentCharIndex = stringLength - padLength; currentCharIndex < stringLength; ++currentCharIndex)
				{
					s[currentCharIndex] = ' ';
				}
			}

			for(currentCharIndex = stringLength - 1; currentCharIndex > 0; --currentCharIndex)
			{
				if(!isCharWhitespace(s[currentCharIndex]))
				{
					s[currentCharIndex + 1] = '\0';

					break;
				}
			}
		}
	}

	void replaceEmptyStrings(char *s)
	{
		if(strlen(s) < 2) return;

		for(int i = 0; i < strlen(s) - 1; ++i)
		{
			if(s[i] == '"' && s[i + 1] == '"' && isCharWhitespace(s[i - 1]))
			{
				s[i - 1] = '"';
				s[i] = ' ';
				s[i + 1] = '"';
			}
		}
	}

	void removeParentheses(char *s)
	{
		if(strlen(s) == 0) return;

		for(int i = 0; i < strlen(s) - 1; ++i)
		{
			if(s[i] == '(' && s[i + 1] == ')')
			{
				s[i] = ' ';
			}
		}
	}

	Token getNextToken()
	{
		int lineLength = strlen(sourceCode[lexer.currentLine]);

		lexer.index0 = lexer.index1;

		if(lexer.index0 > lineLength - 1)
		{
			if(!skipToNextLine()) return END_OF_TOKEN_STREAM;
			else lineLength = strlen(sourceCode[lexer.currentLine]);
		}

		if(lexer.stringState == LEXER_STATE_END_STRING) lexer.stringState = LEXER_STATE_NO_STRING;

		if(lexer.stringState != LEXER_STATE_IN_STRING)
		{
			while(1)
			{
				if(!isCharWhitespace(sourceCode[lexer.currentLine][lexer.index0])) break;

				lexer.index0++;
			}
		}

		lexer.index1 = lexer.index0;

		while(1)
		{
			if(lexer.stringState == LEXER_STATE_IN_STRING)
			{
				if(lexer.index1 > lineLength - 1)
				{
					lexer.currentToken = TOKEN_TYPE_INVALID;
					//message("Invalid token : " + (string)lexer.currentLexeme);
					return lexer.currentToken;
				}

				if(sourceCode[lexer.currentLine][lexer.index1] == '\\')
				{
					lexer.index1 += 2;

					continue;
				}

				if(sourceCode[lexer.currentLine][lexer.index1] == '"') break;

				lexer.index1++;
			}
			else
			{
				if(lexer.index1 > lineLength - 1) break;
				if(isCharDelimiter(sourceCode[lexer.currentLine][lexer.index1])) break;

				lexer.index1++;
			}
		}

		if(lexer.index1 == lexer.index0) lexer.index1++;

		int currentDestinationIndex = 0;
		for(int currentSourceIndex = lexer.index0; currentSourceIndex < lexer.index1; ++currentSourceIndex)
		{
			if(lexer.stringState == LEXER_STATE_IN_STRING)
			{
				//if(currentSourceIndex == lexer.index1 - 1) break;

				if(sourceCode[lexer.currentLine][currentSourceIndex] == '\\')
				{
					//message("\\");
					if(sourceCode[lexer.currentLine][currentSourceIndex + 1] == 'n')
					{
						currentSourceIndex++;
						sourceCode[lexer.currentLine][currentSourceIndex] = '\n';
					}
					else if(sourceCode[lexer.currentLine][currentSourceIndex + 1] == '\\')
					{
						currentSourceIndex++;
						//message("\\\\");
						//sourceCode[lexer.currentLine][currentSourceIndex] = '\\';
					}
					else
					{
						continue;
					}
				}
			}

			lexer.currentLexeme[currentDestinationIndex] = sourceCode[lexer.currentLine][currentSourceIndex];

			currentDestinationIndex++;
		}

		lexer.currentLexeme[currentDestinationIndex] = '\0';

		//if(lexer.stringState != LEXER_STATE_IN_STRING) strupr(lexer.currentLexeme);

		//string s = (string)lexer.currentLexeme; 
		//message((char *)s.c_str());

		lexer.currentToken = TOKEN_TYPE_INVALID;

		int lexemeLength = strlen(lexer.currentLexeme);

		if(lexemeLength > 1 || lexer.currentLexeme[0] != '"')
		{
			if(lexer.stringState == LEXER_STATE_IN_STRING)
			{
				lexer.currentToken = TOKEN_TYPE_STRING;

				return TOKEN_TYPE_STRING;
			}
		}

		if(lexemeLength == 0)
		{
			lexer.currentToken = TOKEN_TYPE_NEW_LINE;
			return lexer.currentToken;
		}

		if(lexemeLength == 1)
		{
			switch(lexer.currentLexeme[0])
			{
			case '"':
				if(lexer.stringState == LEXER_STATE_NO_STRING)
				{
					lexer.stringState = LEXER_STATE_IN_STRING;
				}
				else if(lexer.stringState == LEXER_STATE_IN_STRING)
				{
					lexer.stringState = LEXER_STATE_END_STRING;

					return getNextToken();
				}

				lexer.currentToken = TOKEN_TYPE_DOUBLE_QUOTE;

				break;

			case ',':
				lexer.currentToken = TOKEN_TYPE_COMMA;

				break;

			case ':':
				lexer.currentToken = TOKEN_TYPE_COLON;

				break;

			case '[':
				lexer.currentToken = TOKEN_TYPE_OPEN_BRACKET;

				break;

			case ']':
				lexer.currentToken = TOKEN_TYPE_CLOSE_BRACKET;

				break;

			case '{':
				lexer.currentToken = TOKEN_TYPE_OPEN_BRACE;

				break;

			case '}':
				lexer.currentToken = TOKEN_TYPE_CLOSE_BRACE;

				break;

			case '\n':
				lexer.currentToken = TOKEN_TYPE_NEW_LINE;

				break;
			}
		}

		if(isStringInt(lexer.currentLexeme)) lexer.currentToken = TOKEN_TYPE_INT;
		if(isStringFloat(lexer.currentLexeme)) lexer.currentToken = TOKEN_TYPE_FLOAT;
		if(isStringValidIdentifier(lexer.currentLexeme)) lexer.currentToken = TOKEN_TYPE_IDENTIFIER;

		char currentLexemeUpper[EASM_MAX_IDENTIFIER_LENGTH];
		strcpy(currentLexemeUpper, lexer.currentLexeme);
		_strupr(currentLexemeUpper);

		if(strcmp(currentLexemeUpper, "SETSTACKSIZE") == 0) lexer.currentToken = TOKEN_TYPE_SET_STACK_SIZE;
		if(strcmp(currentLexemeUpper, "VARIABLE") == 0) lexer.currentToken = TOKEN_TYPE_VARIABLE;
		if(strcmp(currentLexemeUpper, "FUNCTION") == 0) lexer.currentToken = TOKEN_TYPE_FUNCTION;
		if(strcmp(currentLexemeUpper, "RETURNVALUE") == 0) lexer.currentToken = TOKEN_TYPE_RETURN_VALUE_REGISTER;
		if(strcmp(currentLexemeUpper, "HOSTRETURNVALUE") == 0) lexer.currentToken = TOKEN_TYPE_REGISTER_HOST_RETURN_VALUE;
		if(strcmp(currentLexemeUpper, "REGISTER0") == 0) lexer.currentToken = TOKEN_TYPE_REGISTER_0;
		if(strcmp(currentLexemeUpper, "REGISTER1") == 0) lexer.currentToken = TOKEN_TYPE_REGISTER_1;
		if(strcmp(currentLexemeUpper, "REGISTER2") == 0) lexer.currentToken = TOKEN_TYPE_REGISTER_2;

		if(strlen(lexer.currentLexeme) < MAX_INSTRUCTION_MNEMONIC_LENGTH + 1)
		{
			if(getInstructionByMnemonic(lexer.currentLexeme)) lexer.currentToken = TOKEN_TYPE_INSTRUCTION;
		}

		if(lexer.currentToken == TOKEN_TYPE_INVALID)
		{
			//message(INT_TO_STRING(lexer.currentLexeme));
		}

		return lexer.currentToken;
	}

	int skipToNextLine()
	{
		lexer.currentLine++;
		if(lexer.currentLine > sourceCodeLineCount - 1) return 0;

		lexer.index0 = 0;
		lexer.index1 = 0;

		lexer.stringState = LEXER_STATE_NO_STRING;

		return 1;
	}

	int goToPreviousLine()
	{
		if(lexer.currentLine == 0) return 0;

		lexer.currentLine--;

		lexer.index0 = 0;
		lexer.index1 = 0;

		lexer.stringState = LEXER_STATE_NO_STRING;

		return 1;
	}

	void resetLexer()
	{
		lexer.currentLine = 0;

		lexer.index0 = 0;
		lexer.index1 = 0;

		lexer.currentToken = TOKEN_TYPE_INVALID;

		lexer.stringState = LEXER_STATE_NO_STRING;
	}

	char getLookAheadChar()
	{
		int currentLine = lexer.currentLine;
		int index = lexer.index1;

		if(lexer.stringState != LEXER_STATE_IN_STRING)
		{
			while(1)
			{
				if(index > strlen(sourceCode[currentLine]) - 1)
				{
					currentLine++;
					if(currentLine > sourceCodeLineCount - 1) return 0;

					index = 0;
				}

				if(!isCharWhitespace(sourceCode[currentLine][index])) break;

				index++;
			}
		}

		//string str = "Current look ahead char : " + sourceCode[currentLine][index];
		//message((char *)str.c_str());

		return sourceCode[currentLine][index];
	}

	// Error handling and debugging

	void throwCodeError(std::string text, int line)
	{
		Debug::outputLogEvent("");
		Debug::throwError("Error in line " + INT_TO_STRING(line + 1) + " : " + text);

		Debug::throwMessage("Code Error");
		//cout << "Error in line " << line + 1 << " : " << text << "\n";
		//cout << sourceCode[line] << "\n";

		//_sleep(0xffffffff);
	}

	void outputStatistics()
	{
		message("EagleAssembler version " + to_string(EASM_VERSION_MAJOR) + "." + to_string(EASM_VERSION_MINOR) + "\n");
		message("Total lines of code : " + to_string(sourceCodeLineCount));
		message("Stack size : " + to_string(scriptHeader.stackSize));
		message("Global data size : " + to_string(scriptHeader.globalDataSize));
		message("Instructions : " + to_string(instructionStreamSize));
		message("Variables : " + to_string(symbolTable.nodeCount));
		message("Strings : " + to_string(stringTable.nodeCount));
		message("Labels : " + to_string(labelTable.nodeCount));
		message("Functions : " + to_string(functionTable.nodeCount));
		//message("Is Main function present? : " + to_string(scriptHeader.isMainFunctionPresent));
		//message("Main function index : " + to_string(scriptHeader.mainFunctionIndex));
	}

	// Parsing

	void initializeParser()
	{
		scriptHeader.stackSize = 0;
		scriptHeader.globalDataSize = 0;

		scriptHeader.isInitializeFunctionPresent = 0;
		scriptHeader.isUpdateFunctionPresent = 0;
		scriptHeader.isRenderFunctionPresent = 0;
		scriptHeader.isEndFunctionPresent = 0;

		instructionStreamSize = 0;
		isSetStackSizeFound = 0;

		isLocalScopeActive = 0;
		//currentFunctionName[EASM_MAX_IDENTIFIER_LENGTH];
		currentFunctionParameterCount = 0;
		currentFunctionLocalDataSize = 0;

		currentInstructionIndex = 0;

		resetLexer();
	}

	void initializeSecondPass()
	{
		instructionStream = new Instruction[instructionStreamSize];

		currentFunctionIndex = 0;
		currentFunctionLocalDataSize = 0;
		currentFunctionParameterCount = 0;
	}

	void parseSourceCode()
	{
		// First pass

		//message("First pass started.");

		bool firstLoopFinished = 0;

		while(1)
		{
			if(getNextToken() == END_OF_TOKEN_STREAM)
			{
				if(isLocalScopeActive) throwCodeError("Expected a }", lexer.currentLine);

				break;
			}

			switch(lexer.currentToken)
			{
			case TOKEN_TYPE_SET_STACK_SIZE:
				{
					if(isLocalScopeActive) throwCodeError("Cannot set stack size in the local scope", lexer.currentLine);
					if(isSetStackSizeFound) throwCodeError("Multiple attempts to set stack size detected", lexer.currentLine);
					if(getNextToken() != TOKEN_TYPE_INT) throwCodeError("invalid stack size", lexer.currentLine);

					scriptHeader.stackSize = atoi(lexer.currentLexeme);

					isSetStackSizeFound = 1;

					break;
				}
			case TOKEN_TYPE_FUNCTION:
				{
					if(isLocalScopeActive) throwCodeError("Nested functions detected", lexer.currentLine);
					if(getNextToken() != TOKEN_TYPE_IDENTIFIER) throwCodeError("Invalid function declaration. Expected a valid identifier", lexer.currentLine);

					char functionName[EASM_MAX_IDENTIFIER_LENGTH];
					strcpy(functionName, lexer.currentLexeme);
					int entryPoint = instructionStreamSize;

					int functionIndex = addToFunctionTable(functionName, entryPoint);
					if(functionIndex == -1) throwCodeError("Function redefinition", lexer.currentLine);

					if(_stricmp(functionName, ES_INITIALIZE_FUNCTION_NAME) == 0)
					{
						scriptHeader.isInitializeFunctionPresent = 1;
						scriptHeader.initializeFunctionIndex = functionIndex;
					}

					if(_stricmp(functionName, ES_UPDATE_FUNCTION_NAME) == 0)
					{
						scriptHeader.isUpdateFunctionPresent = 1;
						scriptHeader.updateFunctionIndex = functionIndex;
					}

					if(_stricmp(functionName, ES_RENDER_FUNCTION_NAME) == 0)
					{
						scriptHeader.isRenderFunctionPresent = 1;
						scriptHeader.renderFunctionIndex = functionIndex;
					}

					if(_stricmp(functionName, ES_END_FUNCTION_NAME) == 0)
					{
						scriptHeader.isEndFunctionPresent = 1;
						scriptHeader.endFunctionIndex = functionIndex;
					}

					isLocalScopeActive = 1;
					strcpy(currentFunctionName, functionName);
					currentFunctionIndex = functionIndex;
					currentFunctionParameterCount = 0;
					currentFunctionLocalDataSize = 0;

					while(getNextToken() == TOKEN_TYPE_NEW_LINE){}

					if(strcmp(lexer.currentLexeme, "(") == 0)
					{
						getNextToken();

						if(strcmp(lexer.currentLexeme, ")") != 0)
						{
							//getNextToken();

							if(lexer.currentToken == TOKEN_TYPE_IDENTIFIER)
							{
								currentFunctionParameterCount++;
								currentFunctionLocalDataSize++;

								getNextToken();
								while(lexer.currentToken == TOKEN_TYPE_COMMA)
								{
									getNextToken();
									if(lexer.currentToken != TOKEN_TYPE_IDENTIFIER)
									{
										throwCodeError("Invalid function declaration.", lexer.currentLine);
									}

									currentFunctionParameterCount++;
									currentFunctionLocalDataSize++;

									getNextToken();
								}

								if(strcmp(lexer.currentLexeme, ")") != 0)
								{
									throwCodeError("Invalid function declaration.", lexer.currentLine);
								}
							}
							else
							{
								throwCodeError("Invalid function declaration.", lexer.currentLine);
							}
						}

						getNextToken();
						if(lexer.currentToken != TOKEN_TYPE_OPEN_BRACE)
						{
							throwCodeError("Expected a {", lexer.currentLine);
						}
					}
					else if(lexer.currentToken != TOKEN_TYPE_OPEN_BRACE)
					{
						//message((char *)to_string(lexer.currentToken).c_str());
						throwCodeError("Expected a {", lexer.currentLine);
					}

					break;
				}
			case TOKEN_TYPE_CLOSE_BRACE:
				{
					if(!isLocalScopeActive) throwCodeError("} must be placed at the end of functions", lexer.currentLine);

					setFunctionInformation(currentFunctionName, currentFunctionParameterCount, currentFunctionLocalDataSize);

					isLocalScopeActive = 0;

					instructionStreamSize++;

					break;
				}

			case TOKEN_TYPE_VARIABLE:
				{
					if(getNextToken() != TOKEN_TYPE_IDENTIFIER) throwCodeError("Invalid variable declaration. Expected a valid identifier", lexer.currentLine);

					char identifier[EASM_MAX_IDENTIFIER_LENGTH];
					strcpy(identifier, lexer.currentLexeme);

					int size = 1;
					//int dimensionCount = 0;
					//int dimensions[MAX_ARRAY_DIMENSION_COUNT];

					bool isArray = 0;

					if(getLookAheadChar() == '[')
					{
						getNextToken();

						if(getNextToken() != TOKEN_TYPE_INT) throwCodeError("Invalid array declaration. Array size must be an integer", lexer.currentLine);

						size *= atoi(lexer.currentLexeme);
						if(size < 1) throwCodeError("Invalid array declaration. Array size must be an greater than zero", lexer.currentLine);

						//dimensions[dimensionCount] = atoi(lexer.currentLexeme);
						//dimensionCount++;

						if(getNextToken() != TOKEN_TYPE_CLOSE_BRACKET) throwCodeError("Invalid array declaration. Expected a ]", lexer.currentLine);

						isArray = 1;
					}

					int stackIndex;
					if(isLocalScopeActive)
					{
						stackIndex = -(currentFunctionLocalDataSize + 2);
						currentFunctionLocalDataSize += size;
					}
					else
					{
						stackIndex = scriptHeader.globalDataSize;

						if(!isArray)
						{
							GlobalVariable globalVariable;
							globalVariable.identifier = identifier;
							globalVariable.stackIndex = stackIndex;

							globalVariableList.push_back(globalVariable);
							//message(identifier);

							globalVariableCount++;
						}

						scriptHeader.globalDataSize += size;
					}

					//if(dimensionCount)
					//{
					//if(addToSymbolTable(identifier, size, stackIndex, currentFunctionIndex, dimensionCount, dimensions) == -1)
					//{
					//	throwCodeError("Variable redefinition detected : " + (string)identifier, lexer.currentLine);
					//}

					//string str = "Array declaration found. Number of dimensions : " + to_string(dimensionCount);
					//message((char *)str.c_str());
					//}
					if(!isLocalScopeActive)
					{
						if(addToSymbolTable(identifier, size, stackIndex, -1) == -1)
						{
							throwCodeError("Variable redefinition detected : " + (string)identifier, lexer.currentLine);
						}
					}
					else
					{
						if(addToSymbolTable(identifier, size, stackIndex, currentFunctionIndex) == -1)
						{
							throwCodeError("Variable redefinition detected : " + (string)identifier, lexer.currentLine);
						}
					}

					break;
				}

				//case TOKEN_TYPE_PARAMETER:

				//break;

			case TOKEN_TYPE_IDENTIFIER:
				{
					if(getLookAheadChar() != ':') throwCodeError("Invalid instruction", lexer.currentLine);
					if(!isLocalScopeActive) throwCodeError("Line labels must be placed in a function scope", lexer.currentLine);

					char *identifier = lexer.currentLexeme;

					int targetIndex = instructionStreamSize - 1;
					int functionIndex = currentFunctionIndex;

					if(addToLabelTable(identifier, targetIndex, functionIndex) == -1) throwCodeError("Line label redefinition detected", lexer.currentLine);

					break;
				}

			case TOKEN_TYPE_INSTRUCTION:
				{
					instructionStreamSize++;
					//message("Instruction found : " + (string)lexer.currentLexeme);
					skipToNextLine();

					break;
				}
			}
		}

		resetLexer();

		// Second pass

		initializeSecondPass();

		//message("Second pass started.");

		while(1)
		{
			if(getNextToken() == END_OF_TOKEN_STREAM)
			{
				if(isLocalScopeActive) throwCodeError("Expected a }", lexer.currentLine);

				break;
			}

			switch(lexer.currentToken)
			{
				//case TOKEN_TYPE_SET_STACK_SIZE:

				//break;

			case TOKEN_TYPE_FUNCTION:
				{
					getNextToken();

					//currentFunction = getFunctionByName(lexer.currentLexeme);

					getNextToken();

					if(strcmp(lexer.currentLexeme, "(") == 0)
					{
						getNextToken();

						if(strcmp(lexer.currentLexeme, ")") != 0)
						{
							int parameterCounter = 0;

							if(lexer.currentToken == TOKEN_TYPE_IDENTIFIER)
							{
								char *identifier = lexer.currentLexeme;

								int stackIndex = -(currentFunctionLocalDataSize - currentFunctionParameterCount + 2 + parameterCounter);

								if(addToSymbolTable(identifier, 1, stackIndex, currentFunctionIndex) == -1) throwCodeError("Identifier redefinition deteted", lexer.currentLine);

								parameterCounter++;

								getNextToken();

								while(lexer.currentToken == TOKEN_TYPE_COMMA)
								{
									getNextToken();

									char *identifier = lexer.currentLexeme;

									int stackIndex = -(currentFunctionLocalDataSize - currentFunctionParameterCount + 2 + parameterCounter);

									if(addToSymbolTable(identifier, 1, stackIndex, currentFunctionIndex) == -1) throwCodeError("Identifier redefinition deteted", lexer.currentLine);

									parameterCounter++;

									getNextToken();
								}
							}
						}
					}

					break;
				}

			case TOKEN_TYPE_CLOSE_BRACE:
				{
					currentInstruction = *getInstructionByMnemonic("Return");

					instructionStream[currentInstructionIndex].opcode = currentInstruction.opcode;
					instructionStream[currentInstructionIndex].operandCount = currentInstruction.operandCount;

					currentInstructionIndex++;

					currentFunctionIndex++;

					break;
				}
				//case TOKEN_TYPE_VARIABLE:

				//break;

				//case TOKEN_TYPE_IDENTIFIER:

				//break;

			case TOKEN_TYPE_INSTRUCTION:
				{
					//message(lexer.currentLexeme);
					//message((char *)to_string(currentInstructionIndex).c_str());

					currentInstruction = *getInstructionByMnemonic(lexer.currentLexeme);

					instructionStream[currentInstructionIndex].opcode = currentInstruction.opcode;
					instructionStream[currentInstructionIndex].operandCount = currentInstruction.operandCount;

					Operand *operandList = 0;
					if(strcmp(_strupr(lexer.currentLexeme), "CALL") == 0)
					{
					}
					else
					{
						operandList = new Operand[currentInstruction.operandCount];
					}

					for(int i = 0; i < currentInstruction.operandCount; ++i)
					{
						OperandType currentOperandType = currentInstruction.operandList[i];
						Token initialOperandToken = getNextToken();
						char initialOperandLexeme[EASM_MAX_IDENTIFIER_LENGTH];
						strcpy(initialOperandLexeme, lexer.currentLexeme);

						switch(initialOperandToken)
						{
						case TOKEN_TYPE_INT:
							if(currentOperandType & OPERAND_FLAG_TYPE_INT)
							{
								operandList[i].type = EASM_OPERAND_TYPE_INT;
								operandList[i].integerLiteral = atoi(initialOperandLexeme);
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Integer", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_FLOAT:
							if(currentOperandType & OPERAND_FLAG_TYPE_FLOAT)
							{
								operandList[i].type = EASM_OPERAND_TYPE_FLOAT;
								operandList[i].floatLiteral = atof(initialOperandLexeme);
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Float", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_DOUBLE_QUOTE:
							if(currentOperandType & OPERAND_FLAG_TYPE_STRING)
							{
								Token nextToken = getNextToken();

								if(nextToken != TOKEN_TYPE_STRING)
								{
									if(nextToken == TOKEN_TYPE_DOUBLE_QUOTE)
									{
										operandList[i].stringIndex = addToStringTable("");
									}
									else
									{
										throwCodeError("Invalid operand", lexer.currentLine);
									}
								}
								else
								{
									operandList[i].stringIndex = addToStringTable(lexer.currentLexeme);
								}

								operandList[i].type = EASM_OPERAND_TYPE_STRING;
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction.. Passed oerand type : String", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_RETURN_VALUE_REGISTER:
							if(currentOperandType & OPERAND_FLAG_TYPE_REGISTER)
							{
								operandList[i].type = EASM_OPERAND_TYPE_REGISTER;
								operandList[i].registerCode = ES_REGISTER_CODE_RETURN_VALUE;
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Register", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_REGISTER_HOST_RETURN_VALUE:
							if(currentOperandType & OPERAND_FLAG_TYPE_REGISTER)
							{
								operandList[i].type = EASM_OPERAND_TYPE_REGISTER;
								operandList[i].registerCode = ES_REGISTER_CODE_HOST_RETURN_VALUE;
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Register", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_REGISTER_0:
							if(currentOperandType & OPERAND_FLAG_TYPE_REGISTER)
							{
								operandList[i].type = EASM_OPERAND_TYPE_REGISTER;
								operandList[i].registerCode = ES_REGISTER_CODE_0;
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Register", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_REGISTER_1:
							if(currentOperandType & OPERAND_FLAG_TYPE_REGISTER)
							{
								operandList[i].type = EASM_OPERAND_TYPE_REGISTER;
								operandList[i].registerCode = ES_REGISTER_CODE_1;
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Register", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_REGISTER_2:
							if(currentOperandType & OPERAND_FLAG_TYPE_REGISTER)
							{
								operandList[i].type = EASM_OPERAND_TYPE_REGISTER;
								operandList[i].registerCode = ES_REGISTER_CODE_2;
							}
							else
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Register", lexer.currentLine);
							}

							break;

						case TOKEN_TYPE_IDENTIFIER:
							if(currentOperandType & OPERAND_FLAG_TYPE_LINELABEL)
							{
								operandList[i].type = EASM_OPERAND_TYPE_INSTRUCTION_INDEX;
								operandList[i].instructionIndex = getLabelByIdentifier(initialOperandLexeme, currentFunctionIndex)->targetInstructionIndex;
							}
							else if(currentOperandType & OPERAND_FLAG_TYPE_FUNCTION_NAME)
							{
								FunctionNode *destinationFunction = getFunctionByName(initialOperandLexeme);
								instructionStream[currentInstructionIndex].operandCount = destinationFunction->parameterCount + 1;
								currentInstruction.operandCount = destinationFunction->parameterCount + 1;

								operandList = new Operand[instructionStream[currentInstructionIndex].operandCount];
								operandList[i].type = EASM_OPERAND_TYPE_FUNCTION_INDEX;
								operandList[i].functionIndex = destinationFunction->index;
							}
							else if(currentOperandType & OPERAND_FLAG_TYPE_HOST_API_CALL)
							{
								operandList[i].type = EASM_OPERAND_TYPE_HOST_FUNCTION_INDEX;

								operandList[i].hostFunctionIndex = virtualMachine.getHostFunctionIndex(initialOperandLexeme);
							}
							else if(currentOperandType & OPERAND_FLAG_TYPE_MEMORY_REFRENCE)
							{
								if(getLookAheadChar() != '[')
								{
									if(!getSymbolByIdentifier(initialOperandLexeme, currentFunctionIndex)) throwCodeError("Use of undeclared variable detected", lexer.currentLine);
									operandList[i].type = EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX;
									operandList[i].stackIndex = getSymbolStackIndexByIdentifier(initialOperandLexeme, currentFunctionIndex);
								}
								else
								{
									SymbolNode *currentSymbol = getSymbolByIdentifier(initialOperandLexeme, currentFunctionIndex);
									operandList[i].stackIndex = currentSymbol->stackIndex;

									if(getLookAheadChar() == '[')
									{
										getNextToken();

										getNextToken();

										if(lexer.currentToken == TOKEN_TYPE_INT)
										{
											operandList[i].type = EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX;
											int integerIndex = atoi(lexer.currentLexeme);
											operandList[i].stackIndex += integerIndex;
											if(integerIndex < 0) throwCodeError("Invalid array index. Array index must be non-negative", lexer.currentLine);

											//dimensionCount++;
										}
										else if(lexer.currentToken == TOKEN_TYPE_IDENTIFIER)
										{
											if(getLookAheadChar() == '[') throwCodeError("Invalid array index. Cannot accept an array element as array inex", lexer.currentLine);

											SymbolNode *indexSymbol = getSymbolByIdentifier(lexer.currentLexeme, currentFunctionIndex);
											//debugMessage(indexSymbol->identifier);
											if(!indexSymbol) throwCodeError("Use of undeclared variable detected", lexer.currentLine);

											operandList[i].type = EASM_OPERAND_TYPE_RELATIVE_STACK_INDEX;
											operandList[i].offsetIndex = indexSymbol->stackIndex;
											//if(position[dimensionCount] < 1) throwCodeError("Invalid array declaration. Array index must be non-negative", lexer.currentLine);

											//isInteger[dimensionCount] = 0;
											//dimensionCount++;
										}
										else if(lexer.currentToken == TOKEN_TYPE_RETURN_VALUE_REGISTER)
										{
											if(getLookAheadChar() == '[') throwCodeError("Invalid array index. Cannot accept an array element as array inex", lexer.currentLine);

											operandList[i].type = EASM_OPERAND_TYPE_REGISTER_STACK_INDEX;
											operandList[i].offsetIndex = ES_REGISTER_CODE_RETURN_VALUE;
										}
										else if(lexer.currentToken == TOKEN_TYPE_REGISTER_HOST_RETURN_VALUE)
										{
											if(getLookAheadChar() == '[') throwCodeError("Invalid array index. Cannot accept an array element as array inex", lexer.currentLine);

											operandList[i].type = EASM_OPERAND_TYPE_REGISTER_STACK_INDEX;
											operandList[i].offsetIndex = ES_REGISTER_CODE_HOST_RETURN_VALUE;
										}
										else if(lexer.currentToken == TOKEN_TYPE_REGISTER_0)
										{
											if(getLookAheadChar() == '[') throwCodeError("Invalid array index. Cannot accept an array element as array inex", lexer.currentLine);

											operandList[i].type = EASM_OPERAND_TYPE_REGISTER_STACK_INDEX;
											operandList[i].offsetIndex = ES_REGISTER_CODE_0;
										}
										else if(lexer.currentToken == TOKEN_TYPE_REGISTER_1)
										{
											if(getLookAheadChar() == '[') throwCodeError("Invalid array index. Cannot accept an array element as array inex", lexer.currentLine);

											operandList[i].type = EASM_OPERAND_TYPE_REGISTER_STACK_INDEX;
											operandList[i].offsetIndex = ES_REGISTER_CODE_1;
										}
										else if(lexer.currentToken == TOKEN_TYPE_REGISTER_2)
										{
											if(getLookAheadChar() == '[') throwCodeError("Invalid array index. Cannot accept an array element as array inex", lexer.currentLine);

											operandList[i].type = EASM_OPERAND_TYPE_REGISTER_STACK_INDEX;
											operandList[i].offsetIndex = ES_REGISTER_CODE_2;
										}
										else
										{
											throwCodeError("Invalid array index", lexer.currentLine);
										}

										if(getNextToken() != TOKEN_TYPE_CLOSE_BRACKET) throwCodeError("Invalid array declaration. Expected a ]", lexer.currentLine);
									}

									//int offsetIndex = 0;
									//int absoluteIndex = 0;

									//operandList[i].stackIndex = currentSymbol->stackIndex;
									//operandList[i].offsetIndex = offsetIndex;
								}
							}
							else 
							{
								throwCodeError("Operand type does not match the type required by the instruction. Passed oerand type : Memory Reference", lexer.currentLine);
							}

							break;
						}

						if(i < currentInstruction.operandCount - 1)
						{
							if(currentInstruction.opcode == ES_OPCODE_CALLHOST)
							{
								int initialLine = lexer.currentLine;

								if(getNextToken() != TOKEN_TYPE_COMMA)
								{
									instructionStream[currentInstructionIndex].operandCount = i + 1;

									if(lexer.currentLine != initialLine)
									{
										lexer.index0 = 0;
										lexer.index1 = 0;
									}

									break;
								}
							}
							else
							{
								int currentLine = lexer.currentLine;

								if(getNextToken() != TOKEN_TYPE_COMMA)
								{
									throwCodeError("Expected a ,", currentLine);
								}
							}
						}
						else
						{
							int offset = 0;

							for(int i = 0; i < currentInstruction.operandCount; ++i)
							{
								if(operandList[i].type == EASM_OPERAND_TYPE_STRING) offset += 1;
							}

							if(strlen(sourceCode[lexer.currentLine]) > lexer.index1 + offset)
							{
								throwCodeError("Unexpected characters", lexer.currentLine);
							}

							skipToNextLine();
						}
					}


					//if(getNextToken() != TOKEN_TYPE_NEW_LINE) throwCodeError("Expected end of line", lexer.currentLine);

					instructionStream[currentInstructionIndex].operandList = operandList;

					/*if(flag)
					{
					///int a = 0;
					//a += 18;
					message("call found");
					message(INT_TO_STRING(instructionStream[currentInstructionIndex].operandCount));
					message(INT_TO_STRING(instructionStream[currentInstructionIndex].operandList[1].integerLiteral));
					flag = 1;
					Sleep(100000);
					}*/

					currentInstructionIndex++;

					//skipToNextLine();

					break;
				}
			}
		}
	}

	void encryptStringTable()
	{
		stringTableSecurityCode = 0;

		LinkedListNode *node;

		node = stringTable.head;

		for(int i = 0; i < stringTable.nodeCount; ++i)
		{
			char *currentString = (char *)node->data;
			int currentStringLength = strlen(currentString);

			for(int j = 0; j < currentStringLength; ++j)
			{
				stringTableSecurityCode += currentString[j];
				currentString[j]++;
			}

			node = node->next;
		}

		stringTableSecurityCode /= 7.77;
	}

	void outputAssembledEVE(char *outputFileAddress)
	{
		FILE *file;
		if(!(file = fopen(outputFileAddress, "wb"))) throwError("Could not create EVE file");

		// Header

		fwrite(EASM_ID_STRING, strlen(EASM_ID_STRING), 1, file);

		char versionMajor = EASM_VERSION_MAJOR;
		char versionMinor = EASM_VERSION_MINOR;
		fwrite(&versionMajor, 1, 1, file);
		fwrite(&versionMinor, 1, 1, file);

		fwrite(&scriptHeader.stackSize, 4, 1, file);
		fwrite(&scriptHeader.globalDataSize, 4, 1, file);

		char isFunctionPresent = 0;
		if(scriptHeader.isInitializeFunctionPresent) isFunctionPresent = 1;

		fwrite(&isFunctionPresent, 1, 1, file);
		fwrite(&scriptHeader.initializeFunctionIndex, 4, 1, file);

		isFunctionPresent = 0;
		if(scriptHeader.isUpdateFunctionPresent) isFunctionPresent = 1;

		fwrite(&isFunctionPresent, 1, 1, file);
		fwrite(&scriptHeader.updateFunctionIndex, 4, 1, file);

		isFunctionPresent = 0;
		if(scriptHeader.isRenderFunctionPresent) isFunctionPresent = 1;

		fwrite(&isFunctionPresent, 1, 1, file);
		fwrite(&scriptHeader.renderFunctionIndex, 4, 1, file);

		isFunctionPresent = 0;
		if(scriptHeader.isEndFunctionPresent) isFunctionPresent = 1;

		fwrite(&isFunctionPresent, 1, 1, file);
		fwrite(&scriptHeader.endFunctionIndex, 4, 1, file);

		// Instruction stream

		fwrite(&instructionStreamSize, sizeof(int), 1, file);

		for(int i = 0; i < instructionStreamSize; ++i)
		{
			short opcode = instructionStream[i].opcode;
			fwrite(&opcode, 2, 1, file);
			
			char operandCount = instructionStream[i].operandCount;
			fwrite(&operandCount, 1, 1, file);

			for(int j = 0; j < instructionStream[i].operandCount; ++j)
			{
				Operand currentOperand = instructionStream[i].operandList[j];

				char operandType = currentOperand.type;

				fwrite(&operandType, 1, 1, file);

				switch(operandType)
				{
				case EASM_OPERAND_TYPE_INT:
					fwrite(&currentOperand.integerLiteral, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					fwrite(&currentOperand.floatLiteral, sizeof(float), 1, file);

					break;

				case EASM_OPERAND_TYPE_STRING:
					fwrite(&currentOperand.stringIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_INSTRUCTION_INDEX:
					fwrite(&currentOperand.instructionIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX:
					fwrite(&currentOperand.stackIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_RELATIVE_STACK_INDEX:
					fwrite(&currentOperand.stackIndex, sizeof(int), 1, file);
					fwrite(&currentOperand.offsetIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_REGISTER_STACK_INDEX:
					fwrite(&currentOperand.stackIndex, sizeof(int), 1, file);
					fwrite(&currentOperand.offsetIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_FUNCTION_INDEX:
					fwrite(&currentOperand.functionIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_HOST_FUNCTION_INDEX:
					fwrite(&currentOperand.hostFunctionIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_REGISTER:
					fwrite(&currentOperand.registerCode, sizeof(int), 1, file);

					break;
				}
			}
		}

		LinkedListNode *node;
		fwrite(&stringTable.nodeCount, sizeof(int), 1, file);

		node = stringTable.head;
		for(int i = 0; i < stringTable.nodeCount; ++i)
		{
			char *currentString = (char *)node->data;
			int currentStringLength = strlen(currentString);

			fwrite(&currentStringLength, sizeof(int), 1, file);
			fwrite(currentString, currentStringLength, 1, file);

			node = node->next;
		}

		// Function table

		fwrite(&functionTable.nodeCount, sizeof(int), 1, file);

		node = functionTable.head;
		for(int i = 0; i < functionTable.nodeCount; ++i)
		{
			FunctionNode *function = (FunctionNode *)node->data;
			fwrite(&function->entryPoint, sizeof(int), 1, file);
			fwrite(&function->parameterCount, sizeof(int), 1, file);
			fwrite(&function->localDataSize, sizeof(int), 1, file);

			int nameLength = strlen(function->name);
			fwrite(&nameLength, sizeof(int), 1, file);
			fwrite(function->name, nameLength, 1, file);

			node = node->next;
		}

		// Global variables

		fwrite(&globalVariableCount, sizeof(short), 1, file);

		std::vector<GlobalVariable>::iterator listIterator;
		for(listIterator = globalVariableList.begin(); listIterator != globalVariableList.end(); ++listIterator)
		{
			short identifierLength = (*listIterator).identifier.length();
			fwrite(&identifierLength, sizeof(short), 1, file);
			fwrite((*listIterator).identifier.c_str(), (*listIterator).identifier.length(), 1, file);
			fwrite(&(*listIterator).stackIndex, sizeof(int), 1, file);
		}

		fclose(file);
	}

	void loadEASMFile(char *fileAddress)
	{
		ifstream file;
		file.open(fileAddress);
		if(file.bad()) return;

		sourceCodeLineCount = 0;
		char s[EASM_MAX_LINE_LENGTH];
		while(!file.eof())
		{
			file.getline(s, EASM_MAX_LINE_LENGTH);
			sourceCodeLineCount++;
		}

		file.close();

		file.open(fileAddress);

		sourceCode = new char *[sourceCodeLineCount];

		for(int i = 0; i < sourceCodeLineCount; ++i)
		{
			sourceCode[i] = new char[EASM_MAX_LINE_LENGTH];
			file.getline(sourceCode[i], EASM_MAX_LINE_LENGTH);
			stripComments(sourceCode[i]);
			trimWhitespace(sourceCode[i]);
			replaceEmptyStrings(sourceCode[i]);
		}

		file.close();
	}

	void cleanup()
	{
		for(int i = 0; i < sourceCodeLineCount; ++i)
		{
			delete[](sourceCode[i]);
		}

		delete[](sourceCode);

		sourceCodeLineCount = 0;

		for(int i = 0; i < instructionStreamSize; ++i)
		{
			if(!instructionStream[i].operandCount)
			{
				continue;
			}

			delete[](instructionStream[i].operandList);
		}

		delete[](instructionStream);

		instructionStreamSize = 0;

		stringTable.cleanup();

		functionTable.cleanup();

		symbolTable.cleanup();

		labelTable.cleanup();

		/*for(int i = 0; i < globalVariableCount; ++i)
		{
			delete[](globalVariableList[i]->identifier);
			delete(globalVariableList[i]);
		}*/

		globalVariableList.clear();
		globalVariableCount = 0;
	}
};
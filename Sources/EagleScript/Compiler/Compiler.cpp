#include "../EagleScript.h"

using namespace EagleScript;

namespace EagleScriptCompiler
{
	LinkedList sourceCode;
	LinkedList functionTable;
	LinkedList symbolTable;
	LinkedList stringTable;

	ScriptHeader scriptHeader;

	int functionParameterListIndex;

	LinkedList includeDataList;

	string rootAddress;

	bool compileESFile(EagleVirtualMachine::Script *script, char *inputFileAddress, char *outputFileAddress, bool optimize, bool outputEASMAndEVEFiles, bool addComments)
	{
		int characterIndex = -1;

		for(int i = strlen(inputFileAddress) - 2; i > -1; --i)
		{
			if(inputFileAddress[i] == '/')
			{
				characterIndex = i;

				break;
			}
		}

		if(characterIndex != -1)
		{
			rootAddress = inputFileAddress;
			rootAddress = rootAddress.substr(0, characterIndex + 1);
		}
		else
		{
			rootAddress = "";
		}

		initializeCompiler();

		if(!loadSourceFile(inputFileAddress))
		{
			shutdownCompiler();

			return 0;
		}

		preprocessSourceFile();

		if(!compileSourceFile())
		{
			shutdownCompiler();

			return 0;
		}

		if(optimize)
		{
			optimizeICode();
		}

		if(outputEASMAndEVEFiles)
		{
			emitCode(outputFileAddress, addComments);
		}

		script->loadCompilerData();

		shutdownCompiler();

		if(outputEASMAndEVEFiles)
		{
			std::string eveAddress = (std::string)outputFileAddress + ".eve";
			EagleAssembler::loadEASMFile(outputFileAddress);
			EagleAssembler::initializeParser();
			EagleAssembler::parseSourceCode();
			//script->loadAssemblerData();
			EagleAssembler::outputAssembledEVE((char *)eveAddress.c_str());
			EagleAssembler::cleanup();
		}

		Debug::outputLogEvent((char *)("Script \"" + (string)inputFileAddress + "\" successfully loaded").c_str());
		Debug::outputLogEvent("");

		return 1;
	}

	void initializeCompiler()
	{
		scriptHeader.isInitializeFunctionPresent = 0;
		scriptHeader.isUpdateFunctionPresent = 0;
		scriptHeader.isRenderFunctionPresent = 0;
		scriptHeader.isEndFunctionPresent = 0;

		scriptHeader.stackSize = 0;
		scriptHeader.globalDataSize = 0;

		currentLexerState.currentLineIndex = 0;
	}

	bool loadSourceFile(char *fileAddress)
	{
		FILE *file;

		if(!(file = fopen(fileAddress, "r"))) 
		{
			throwError("Could not open source file " + (string)fileAddress);

			return 0;
		}

		fseek(file, 0, SEEK_END);
		sourceCodeStreamSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		while(!feof(file))
		{
			char *currentLine = new char[ES_MAX_SOURCE_LINE_LENGTH + 1];

			fgets(currentLine, ES_MAX_SOURCE_LINE_LENGTH, file);

			sourceCode.addNode(currentLine);
		}

		fclose(file);

		return 1;
	}

	bool includeSourceFile(char *fileAddress, int mainSourceFileLineIndex)
	{
		LinkedListNode *currentNode = includeDataList.head;

		for(int i = 0; i < includeDataList.nodeCount; ++i)
		{
			IncludeData *includeData = (IncludeData *)currentNode->data;

			if(_stricmp(includeData->fileAddress, getCurrentLexeme()) == 0)
			{
				Debug::outputLogEvent((char *)("Warning : Duplicate file inclusion : \"" + (string)getCurrentLexeme() + "\"; include skipped").c_str());

				return 1;
			}

			currentNode = currentNode->next;

			if(!currentNode)
			{
				break;
			}
		}

		IncludeData *includeData = new IncludeData();
		includeData->fileAddress = new char[strlen(fileAddress) + 1];
		strcpy(includeData->fileAddress, fileAddress);

		FILE *file;

		if(!(file = fopen(fileAddress, "r")))
		{
			throwError("Could not open source file " + (string)fileAddress);

			return 0;
		}

		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);

		if(fileSize)
		{
			sourceCodeStreamSize += fileSize;

			fseek(file, 0, SEEK_SET);

			while(!feof(file))
			{
				char *currentLine = new char[ES_MAX_SOURCE_LINE_LENGTH + 1];

				fgets(currentLine, ES_MAX_SOURCE_LINE_LENGTH, file);

				includeData->sourceCode.addNode(currentLine);
			}

			fclose(file);

			if(includeData->sourceCode.nodeCount > 0)
			{
				sourceCode.insertLinkedListAfterIndex(&includeData->sourceCode, mainSourceFileLineIndex);
			}

			preprocessSourceFile();
		}

		includeDataList.addNode(includeData);

		currentLexerState.currentLexemeStart = 0;
		currentLexerState.currentLexemeEnd = 0;
		currentLexerState.currentLineNode = sourceCode.head;
		currentLexerState.currentLineIndex = 0;
		currentLexerState.operatorIndex = 0;
		currentLexerState.lexerState = ES_LEXER_STATE_START;
		currentLexerState.currentToken = 0;

		copyLexerState(&previousLexerState, currentLexerState);

		return 1;
	}

	void preprocessSourceFile()
	{
		bool isInBlockComment = 0;
		int isInString = 0;

		LinkedListNode *currentNode = sourceCode.head;

		while(1)
		{
			char *currentLine = (char *)currentNode->data;

			int stringLength = strlen(currentLine);

			for(int i = 0; i < stringLength; ++i)
			{
				if(currentLine[i] == '\\' && isInString)
				{
					i++;

					continue;
				}

				if(currentLine[i] == '"' && !isInBlockComment)
				{
					isInString = !isInString;
				}
				
				if(currentLine[i] == '/' && currentLine[i + 1] == '/')
				{
					if(!isInString && !isInBlockComment)
					{
						currentLine[i] = '\n';
						currentLine[i + 1] = '\0';

						break;
					}
				}
				else if(currentLine[i] == '/' && currentLine[i + 1] == '*')
				{
					if(!isInString && !isInBlockComment)
					{
						isInBlockComment = 1;
					}
				}
				else if(currentLine[i] == '*' && currentLine[i + 1] == '/')
				{
					if(isInBlockComment)
					{
						currentLine[i] = ' ';
						currentLine[i + 1] = ' ';

						isInBlockComment = 0;
					}
				}

				if(isInBlockComment)
				{
					if(currentLine[i] != '\n') currentLine[i] = ' ';
				}
			}

			currentNode = currentNode->next;

			if(!currentNode) break;
		}
	}

	bool compileSourceFile()
	{
		functionParameterListIndex = addSymbol("FunctionParameterList", ES_MAX_FUNCTION_PARAMETER_COUNT, ES_SCOPE_GLOBAL, ES_SYMBOL_TYPE_VARIABLE);

		if(!parseSourceCode())
		{
			return 0;
		}

		return 1;
	}

	void emitCode(char *fileAddress, bool addComments)
	{
		emitEASMCode(fileAddress, addComments);
	}

	void shutdownCompiler()
	{
		sourceCode.cleanup();

		FunctionNode *currentFunction;
		LinkedListNode *listIterator = functionTable.head;

		for(int i = 0; i < functionTable.nodeCount; ++i)
		{
			currentFunction = (FunctionNode *)listIterator->data;

			if(currentFunction)
			{
				LinkedListNode *currentNode = currentFunction->iCodeStream.head;

				for(int j = 0; j < currentFunction->iCodeStream.nodeCount; ++j)
				{
					((ICodeNode *)currentNode->data)->instruction.operandList.cleanup();

					currentNode = currentNode->next;
				}

				currentFunction->iCodeStream.cleanup();
			}

			listIterator = listIterator->next;
		}

		functionTable.cleanup();

		symbolTable.cleanup();
		stringTable.cleanup();

		LinkedListNode *currentNode = includeDataList.head;

		for(int i = 0; i < includeDataList.nodeCount; ++i)
		{
			IncludeData *includeData = (IncludeData *)currentNode->data;

			delete[](includeData->fileAddress);

			//includeData->sourceCode.cleanup();

			currentNode = currentNode->next;
		}

		includeDataList.cleanup();

		shutdownParser();
	}

	bool compileESString(EagleVirtualMachine::Script *script, std::string scriptText, bool optimize)
	{
		rootAddress = "";

		initializeCompiler();

		sourceCodeStreamSize = scriptText.length();

		/*char *currentLine = new char[scriptText.length() + 1];

		strcpy(currentLine, (char *)scriptText.c_str());

		sourceCode.addNode(currentLine);*/

		int stringStartIndex = 0;
		int stringLength = scriptText.length();
		for(int i = 0; i < stringLength; ++i)
		{
			if(scriptText[i] == '\n' || i == stringLength - 1)
			{
				std::string currentLineString = scriptText.substr(stringStartIndex, i - stringStartIndex + 1);

				char *currentLine = new char[i - stringStartIndex + 2];
				strcpy(currentLine, currentLineString.c_str());
				currentLine[i - stringStartIndex + 1] = '\0';

				sourceCode.addNode(currentLine);

				stringStartIndex = i + 1;
			}
		}

		preprocessSourceFile();

		if(!compileSourceFile())
		{
			shutdownCompiler();

			return 0;
		}

		if(optimize)
		{
			optimizeICode();
		}

		script->loadCompilerData();

		shutdownCompiler();

		return 1;
	}
};
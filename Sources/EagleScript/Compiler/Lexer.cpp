#include "../EagleScript.h"

using namespace EagleScript;

namespace EagleScriptCompiler
{
	char *sourceCodeStream = 0;
	int sourceCodeStreamSize = 0;
	
	LexerState currentLexerState;
	LexerState previousLexerState;

	int delimiterCount = 9;
	char delimiterList[ES_MAX_DELIMITER_COUNT] = {',', '(', ')', '[', ']', '{', '}', ';', ':'};

	OperatorState operatorCharacterList[3][ES_MAX_OPERATOR_STATE_COUNT];

	string operatorStringList[3 * ES_MAX_OPERATOR_STATE_COUNT];

	LexerState::LexerState()
	{
		currentLexemeStart = 0;
		currentLexemeEnd = 0;

		lexerState = 0;

		operatorIndex = 0;
	}

	void resetEagleScriptLexer()
	{
		currentLexerState.currentLexemeStart = 0;
	    currentLexerState.currentLexemeEnd = 0;
		currentLexerState.currentLineNode = sourceCode.head;

		operatorCharacterList[0][0].setProperties('+', 0, 2, 0);
		operatorCharacterList[0][1].setProperties('-', 2, 2, 1);
		operatorCharacterList[0][2].setProperties('*', 4, 1, 2);
		operatorCharacterList[0][3].setProperties('/', 5, 1, 3);
		operatorCharacterList[0][4].setProperties('%', 6, 1, 4);
		operatorCharacterList[0][5].setProperties('^', 7, 1, 5);
		operatorCharacterList[0][6].setProperties('&', 8, 2, 6);
		operatorCharacterList[0][7].setProperties('|', 10, 2, 7);
		operatorCharacterList[0][8].setProperties('#', 12, 1, 8);
		operatorCharacterList[0][9].setProperties('~', 0, 0, 9);
		operatorCharacterList[0][10].setProperties('!', 13, 1, 10);
		operatorCharacterList[0][11].setProperties('=', 14, 1, 11);
		operatorCharacterList[0][12].setProperties('<', 15, 2, 12);
		operatorCharacterList[0][13].setProperties('>', 17, 2, 13);

		operatorCharacterList[1][0].setProperties('=', 0, 0, 14); // +=
		operatorCharacterList[1][1].setProperties('+', 0, 0, 15); // ++
		operatorCharacterList[1][2].setProperties('=', 0, 0, 16); // -=
		operatorCharacterList[1][3].setProperties('-', 0, 0, 17); // --
		operatorCharacterList[1][4].setProperties('=', 0, 0, 18); // *=
		operatorCharacterList[1][5].setProperties('=', 0, 0, 19); // /=
		operatorCharacterList[1][6].setProperties('=', 0, 0, 20); // %=
		operatorCharacterList[1][7].setProperties('=', 0, 0, 21); // ^=
		operatorCharacterList[1][8].setProperties('=', 0, 0, 22); // &=
		operatorCharacterList[1][9].setProperties('&', 0, 0, 23); // &&
		operatorCharacterList[1][10].setProperties('=', 0, 0, 24); // |=
		operatorCharacterList[1][11].setProperties('|', 0, 0, 25); // ||
		operatorCharacterList[1][12].setProperties('=', 0, 0, 26); // #=
		operatorCharacterList[1][13].setProperties('=', 0, 0, 27); // !=
		operatorCharacterList[1][14].setProperties('=', 0, 0, 28); // ==
		operatorCharacterList[1][15].setProperties('=', 0, 0, 29); // <=
		operatorCharacterList[1][16].setProperties('<', 0, 1, 30); // <<
		operatorCharacterList[1][17].setProperties('=', 0, 0, 31); // >=
		operatorCharacterList[1][18].setProperties('>', 1, 1, 32); // >>

		operatorCharacterList[2][0].setProperties('=', 0, 0, 33); // <<=
		operatorCharacterList[2][1].setProperties('=', 0, 0, 34); // >>=

		operatorStringList[0] = "Add +";
		operatorStringList[1] = "Subtract -";
		operatorStringList[2] = "Multiply *";
		operatorStringList[3] = "Divide /";
		operatorStringList[4] = "Mod %";
		operatorStringList[5] = "Exponent ^";
		operatorStringList[6] = "Bitwise And &";
		operatorStringList[7] = "Bitwise Or |";
		operatorStringList[8] = "Bitwise XOr #";
		operatorStringList[9] = "Bitwise Not ~";
		operatorStringList[10] = "Logical Not !";
		operatorStringList[11] = "Assign =";
		operatorStringList[12] = "Relational Less than <";
		operatorStringList[13] = "Relational Greater than >";
		operatorStringList[14] = "Assign Add +=";
		operatorStringList[15] = "Increment ++";
		operatorStringList[16] = "Assign Subtract -=";
		operatorStringList[17] = "Decrement --";
		operatorStringList[18] = "Assign Multiply *=";
		operatorStringList[19] = "Assign Divide /=";
		operatorStringList[20] = "Assign Mod %=";
		operatorStringList[21] = "Assign Exponent ^=";
		operatorStringList[22] = "Assign And &=";
		operatorStringList[23] = "Logical And &&";
		operatorStringList[24] = "Assign Or |=";
		operatorStringList[25] = "Logical Or ||";
		operatorStringList[26] = "Assign XOr #=";
		operatorStringList[27] = "Relational Not Equal !=";
		operatorStringList[28] = "Relational Equal ==";
		operatorStringList[29] = "Relational Less Equal <=";
		operatorStringList[30] = "Bitwise Shift Left <<";
		operatorStringList[31] = "Relational Grater Equal >=";
		operatorStringList[32] = "Bitwise Shift Right >>";
		operatorStringList[33] = "Assign Shift Left <<=";
		operatorStringList[34] = "Assign Shift Right >>=";

		currentLexerState.operatorIndex = 0;
	}
	
	int loadESFile(char *fileAddress)
	{
		message("Lexer demo");
		message("");

		FILE *file;

		if(!(file = fopen(fileAddress, "rb")))
		{
			throwError("File input error");
		}

		fseek(file, 0, SEEK_END);
		sourceCodeStreamSize = ftell(file);
		message("Source file size : " + INT_TO_STRING(sourceCodeStreamSize) + " bytes");
		message("");
		fseek(file, 0, SEEK_SET);

		if(sourceCodeStream)
		{
			delete[](sourceCodeStream);
		}

		sourceCodeStream = new char[sourceCodeStreamSize + 1];

		char currentCharacter;

		for(int i = 0; i < sourceCodeStreamSize; ++i)
		{
			currentCharacter = fgetc(file);

			if(currentCharacter == 13)
			{
				fgetc(file);
				sourceCodeStreamSize--;

				sourceCodeStream[i] = '\n';
			}
			else
			{
				sourceCodeStream[i] = currentCharacter;
			}
		}

		sourceCodeStream[sourceCodeStreamSize] = '\0';

		fclose(file);

		resetEagleScriptLexer();

		Token currentToken;
		char tokenTypeString[128];
		int tokenCount = 0;

		while(1)
		{
			currentToken = getNextToken();

			if(currentToken == ES_TOKEN_TYPE_END_OF_STREAM)
			{
				//message("End of stream");

				break;
			}

			switch(currentToken)
			{
			case ES_TOKEN_TYPE_INT:
				strcpy(tokenTypeString, "Integer");

				break;

			case ES_TOKEN_TYPE_FLOAT:
				strcpy(tokenTypeString, "Float");

				break;

			case ES_TOKEN_TYPE_IDENTIFIER:
				strcpy(tokenTypeString, "Identifier");

				break;

			case ES_TOKEN_TYPE_RESERVED_VARIABLE:
				strcpy(tokenTypeString, "Variable");

				break;

			case ES_TOKEN_TYPE_RESERVED_TRUE:
				strcpy(tokenTypeString, "True");

				break;

			case ES_TOKEN_TYPE_RESERVED_FALSE:
				strcpy(tokenTypeString, "False");

				break;

			case ES_TOKEN_TYPE_RESERVED_IF:
				strcpy(tokenTypeString, "If");

				break;

			case ES_TOKEN_TYPE_RESERVED_ELSE:
				strcpy(tokenTypeString, "Else");

				break;

			case ES_TOKEN_TYPE_RESERVED_SWITCH:
				strcpy(tokenTypeString, "Switch");

				break;

			case ES_TOKEN_TYPE_RESERVED_CASE:
				strcpy(tokenTypeString, "Case");

				break;

			case ES_TOKEN_TYPE_RESERVED_DEFAULT:
				strcpy(tokenTypeString, "Default");

				break;

			case ES_TOKEN_TYPE_RESERVED_BREAK:
				strcpy(tokenTypeString, "Break");

				break;
			
			case ES_TOKEN_TYPE_RESERVED_CONTINUE:
				strcpy(tokenTypeString, "Continue");

				break;

			case ES_TOKEN_TYPE_RESERVED_FOR:
				strcpy(tokenTypeString, "For");

				break;

			case ES_TOKEN_TYPE_RESERVED_WHILE:
				strcpy(tokenTypeString, "While");

				break;

			case ES_TOKEN_TYPE_RESERVED_FUNCTION:
				strcpy(tokenTypeString, "Function");

				break;

			case ES_TOKEN_TYPE_RESERVED_RETURN:
				strcpy(tokenTypeString, "Return");

				break;

			case ES_TOKEN_TYPE_DELIMITER_COMMA:
				strcpy(tokenTypeString, "Comma");

				break;

			case ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE:
				strcpy(tokenTypeString, "Open parenthese");

				break;

			case ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE:
				strcpy(tokenTypeString, "Close parenthese");

				break;

			case ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE:
				strcpy(tokenTypeString, "Open brace");

				break;

			case ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE:
				strcpy(tokenTypeString, "Close brace");

				break;

			case ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE:
				strcpy(tokenTypeString, "Open curly brace");

				break;

			case ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE:
				strcpy(tokenTypeString, "Close curly brace");

				break;

			case ES_TOKEN_TYPE_DELIMITER_SEMICOLON:
				strcpy(tokenTypeString, "Semicolon");

				break;

			case ES_TOKEN_TYPE_STRING:
				strcpy(tokenTypeString, "String");

				break;

			case ES_TOKEN_TYPE_OPERATOR:
				strcpy(tokenTypeString, "Operator (");
				strcat(tokenTypeString, getCurrentOperatorString().c_str());
				strcat(tokenTypeString, ")");

				break;

			case ES_TOKEN_TYPE_END_OF_STREAM:
				strcpy(tokenTypeString, "End of stream");

				break;
			}

			message("Index : " + INT_TO_STRING(tokenCount) + ", Type : " + (string)tokenTypeString + ", Lexeme : \"" + (string)getCurrentLexeme() + "\"");

			tokenCount++;
		}

		message("");
		message("Token count : " + INT_TO_STRING(tokenCount));

		return 0;
	}

	Token getNextToken()
	{
		copyLexerState(&previousLexerState, currentLexerState);

		currentLexerState.currentLexemeStart = currentLexerState.currentLexemeEnd;

		if(currentLexerState.currentLexemeStart >= sourceCodeStreamSize)
		{
			return ES_TOKEN_TYPE_END_OF_STREAM;
		}

		currentLexerState.lexerState = ES_LEXER_STATE_START;

		bool isLexemeFinished = 0;

		char currentCharacter;

		int nextLexemeCharIndex = 0;

		bool addCurrentCharacter;

		int currentOperatorCharacterIndex = 0;
		int currentOperatorStateIndex = 0;
		OperatorState currentOperatorState;

		while(1)
		{
			currentCharacter = getNextCharacter();

			if(currentCharacter == '\0') break;

			addCurrentCharacter = 1;

			switch(currentLexerState.lexerState)
			{
			case ES_LEXER_STATE_START:
				{
					if(ES_isCharWhitespace(currentCharacter))
					{
						++currentLexerState.currentLexemeStart;

						addCurrentCharacter = 0;
					}
					else if(isCharNumeric(currentCharacter))
					{
						currentLexerState.lexerState = ES_LEXER_STATE_INT;
					}
					else if(currentCharacter == '.')
					{
						currentLexerState.lexerState = ES_LEXER_STATE_FLOAT;
					}
					else if(isCharAlphabetic(currentCharacter))
					{
						currentLexerState.lexerState = ES_LEXER_STATE_IDENTIFIER;
					}
					else if(ES_isCharDelimiter(currentCharacter))
					{
						currentLexerState.lexerState = ES_LEXER_STATE_DELIMITER;
					}
					else if(ES_isCharOperator(currentCharacter, 0))
					{
						currentOperatorStateIndex = getOperatorStateIndex(currentCharacter, 0, 0, 0);

						if(currentOperatorStateIndex == -1) throwCodeError("Invalid token");

						currentOperatorState = getOperatorState(0, currentOperatorStateIndex);

						currentOperatorCharacterIndex = 1;

						currentLexerState.operatorIndex = currentOperatorState.index;

						currentLexerState.lexerState = ES_LEXER_STATE_OPERATOR;
					}
					else if(currentCharacter == '"')
					{
						addCurrentCharacter = 0;

						currentLexerState.lexerState = ES_LEXER_STATE_STRING;
					}
					else
					{
						//throwCodeError("Invalid token. current lexer state : Start");
					}

					break;
				}

			case ES_LEXER_STATE_INT:
				{
					if(isCharNumeric(currentCharacter))
					{
					}
					else if(currentCharacter == '.')
					{
						currentLexerState.lexerState = ES_LEXER_STATE_FLOAT;
					}
					else if(ES_isCharWhitespace(currentCharacter) || ES_isCharDelimiter(currentCharacter) || ES_isCharOperator(currentCharacter, 0))
					{
						addCurrentCharacter = 0;

						isLexemeFinished = 1;
					}
					else
					{
						//throwCodeError("Invalid token. current lexer state : Integer");
					}

					break;
				}

			case ES_LEXER_STATE_FLOAT:
				{
					if(isCharNumeric(currentCharacter))
					{
					}
					else if(ES_isCharWhitespace(currentCharacter) || ES_isCharDelimiter(currentCharacter) || ES_isCharOperator(currentCharacter, 0))
					{
						addCurrentCharacter = 0;

						isLexemeFinished = 1;
					}
					else
					{
						//throwCodeError("Invalid token. current lexer state : Float");
					}

					break;
				}

			case ES_LEXER_STATE_IDENTIFIER:
				{
					if(isCharAlphabetic(currentCharacter) || isCharNumeric(currentCharacter) || currentCharacter == '_')
					{
					}
					else if(ES_isCharWhitespace(currentCharacter) || ES_isCharDelimiter(currentCharacter) || ES_isCharOperator(currentCharacter, 0))
					{
						addCurrentCharacter = 0;

						isLexemeFinished = 1;
					}
					else
					{
						//throwCodeError("Invalid token. current lexer state : Identifier");
					}

					break;
				}

			case ES_LEXER_STATE_STRING:
				{
					if(currentCharacter == '"')
					{
						addCurrentCharacter = 0;

						currentLexerState.lexerState = ES_LEXER_STATE_STRING_CLOSE_QUOTE;
					}
					else if(currentCharacter == '\\')
					{
						addCurrentCharacter = 1;

						currentLexerState.lexerState = ES_LEXER_STATE_STRING_ESCAPE;
					}

					break;
				}

			case ES_LEXER_STATE_STRING_CLOSE_QUOTE:
				{
					addCurrentCharacter = 0;

					isLexemeFinished = 1;

					break;
				}

			case ES_LEXER_STATE_STRING_ESCAPE:
				{
					currentLexerState.lexerState = ES_LEXER_STATE_STRING;

					break;
				}

			case ES_LEXER_STATE_DELIMITER:
				{
					addCurrentCharacter = 0;

					isLexemeFinished = 1;

					break;
				}

			case ES_LEXER_STATE_OPERATOR:
				{
					if(currentOperatorState.substateCount == 0)
					{	
						addCurrentCharacter = 0;

						isLexemeFinished = 1;

						break;
					}

					if(ES_isCharOperator(currentCharacter, currentOperatorCharacterIndex))
					{
						currentOperatorStateIndex = getOperatorStateIndex(currentCharacter, currentOperatorCharacterIndex, currentOperatorState.substateIndex, currentOperatorState.substateCount);

						if(currentOperatorStateIndex == -1) throwCodeError("Invalid token");

						currentOperatorState = getOperatorState(currentOperatorCharacterIndex, currentOperatorStateIndex);

						currentOperatorCharacterIndex++;

						currentLexerState.operatorIndex = currentOperatorState.index;
					}
					else
					{
						addCurrentCharacter = 0;

						isLexemeFinished = 1;
					}

					break;
				}
			}

			if(addCurrentCharacter)
			{
				currentLexerState.currentLexeme[nextLexemeCharIndex] = currentCharacter;

				nextLexemeCharIndex++;
			}

			if(isLexemeFinished) break;
		}

		currentLexerState.currentLexeme[nextLexemeCharIndex] = '\0';

		currentLexerState.currentLexemeEnd--;

		Token tokenType;

		switch(currentLexerState.lexerState)
		{
		case ES_LEXER_STATE_INT:
			tokenType = ES_TOKEN_TYPE_INT;

			break;

		case ES_LEXER_STATE_FLOAT:
			tokenType = ES_TOKEN_TYPE_FLOAT;

			break;

		case ES_LEXER_STATE_IDENTIFIER:
			tokenType = ES_TOKEN_TYPE_IDENTIFIER;

			if(_stricmp(currentLexerState.currentLexeme, "Variable") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_VARIABLE;
			if(_stricmp(currentLexerState.currentLexeme, "True") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_TRUE;
			if(_stricmp(currentLexerState.currentLexeme, "False") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_FALSE;
			if(_stricmp(currentLexerState.currentLexeme, "If") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_IF;
			if(_stricmp(currentLexerState.currentLexeme, "Else") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_ELSE;
			if(_stricmp(currentLexerState.currentLexeme, "Switch") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_SWITCH;
			if(_stricmp(currentLexerState.currentLexeme, "Case") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_CASE;
			if(_stricmp(currentLexerState.currentLexeme, "Default") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_DEFAULT;
			if(_stricmp(currentLexerState.currentLexeme, "Break") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_BREAK;
			if(_stricmp(currentLexerState.currentLexeme, "Continue") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_CONTINUE;
			if(_stricmp(currentLexerState.currentLexeme, "For") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_FOR;
			if(_stricmp(currentLexerState.currentLexeme, "While") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_WHILE;
			if(_stricmp(currentLexerState.currentLexeme, "Function") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_FUNCTION;
			if(_stricmp(currentLexerState.currentLexeme, "Return") == 0) tokenType = ES_TOKEN_TYPE_RESERVED_RETURN;

			break;

		case ES_LEXER_STATE_DELIMITER:
			tokenType = ES_TOKEN_TYPE_IDENTIFIER;

			switch(currentLexerState.currentLexeme[0])
			{
			case ',':
				tokenType = ES_TOKEN_TYPE_DELIMITER_COMMA;

				break;

			case '(':
				tokenType = ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE;

				break;

			case ')':
				tokenType = ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE;

				break;

			case '[':
				tokenType = ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE;

				break;

			case ']':
				tokenType = ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE;

				break;

			case '{':
				tokenType = ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE;

				break;

			case '}':
				tokenType = ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE;

				break;

			case ';':
				tokenType = ES_TOKEN_TYPE_DELIMITER_SEMICOLON;

				break;

			case ':':
				tokenType = ES_TOKEN_TYPE_DELIMITER_COLON;

				break;
			}

			break;

		case ES_LEXER_STATE_STRING_CLOSE_QUOTE:
			tokenType = ES_TOKEN_TYPE_STRING;

			break;

		case ES_LEXER_STATE_OPERATOR:
			tokenType = ES_TOKEN_TYPE_OPERATOR;

			break;

		default:
			//message("Current lexer state : " + INT_TO_STRING(currentLexerState.lexerState));

			char str[1] = {currentLexerState.currentLexeme[0]};

			//message(INT_TO_STRING(currentLexerState.currentLexeme[0]) + " !!");

			tokenType = ES_TOKEN_TYPE_END_OF_STREAM;

			break;
		}

		currentLexerState.currentToken = tokenType;

		return tokenType;
	}

	char getNextCharacter()
	{
		char *currentLineString;

		if(currentLexerState.currentLineNode)
		{
			currentLineString = (char *)currentLexerState.currentLineNode->data;
		}
		else
		{
			return '\0';
		}

		int currentLineLength = strlen(currentLineString);

		if(currentLexerState.currentLexemeEnd > currentLineLength - 1 || ES_isStringWhitespace(currentLineString))
		{
			currentLexerState.currentLineNode = currentLexerState.currentLineNode->next;

			if(currentLexerState.currentLineNode)
			{
				currentLineString = (char *)currentLexerState.currentLineNode->data;

				currentLexerState.currentLineIndex++;

				currentLexerState.currentLexemeStart = 0;
				currentLexerState.currentLexemeEnd = 0;
			}
			else
			{
				return '\0';
			}
		}

		//char a[1] = {currentLineString[currentLexerState.currentLexemeEnd+1]};
		//message(a);
		//message(INT_TO_STRING(currentLineString[currentLexerState.currentLexemeEnd+1]));

		//cout << "(" << currentLineString[currentLexerState.currentLexemeEnd+1] << ")";

		return currentLineString[currentLexerState.currentLexemeEnd++];
	}

	char *getCurrentLexeme()
	{
		return currentLexerState.currentLexeme;
	}

	void copyCurrentLexeme(char *destinationString)
	{
		strcpy(destinationString, currentLexerState.currentLexeme);
	}

	bool ES_isCharWhitespace(char c)
	{
		if(c == ' ' || c == '\t' || c == '\n')
		{
			return 1;
		}

		return 0;
	}

	bool ES_isCharDelimiter(char c)
	{
		for(int i = 0; i < delimiterCount; ++i)
		{
			if(c == delimiterList[i])
			{
				return 1;
			}
		}

		return 0;
	}

	bool ES_isStringWhitespace(char *s)
	{
		/*if(!IS_STRING_VALID(s))
		{
			return 0;
		}*/

		int stringLength = strlen(s);

		for(int i = 0; i < stringLength; ++i)
		{
			if(!isCharWhitespace(s[i]))
			{
				return 0;
			}
		}

		return 1;
	}

	int getCurrentOperatorIndex()
	{
		return currentLexerState.operatorIndex;
	}

	bool ES_isCharOperator(char c, int index)
	{
		for(int i = 0; i < ES_MAX_OPERATOR_STATE_COUNT; ++i)
		{
			char operatorChar = operatorCharacterList[index][i].character;

			if(c == operatorChar)
			{
				return 1;
			}
		}

		return 0;
	}

	int getOperatorStateIndex(char c, int characterIndex, int substateIndex, int substateCount)
	{
		int startStateIndex;
		int endStateIndex;

		if(characterIndex == 0)
		{
			startStateIndex = 0;
			endStateIndex = ES_MAX_OPERATOR_STATE_COUNT;
		}
		else
		{
			startStateIndex = substateIndex;
			endStateIndex = startStateIndex + substateCount;
		}

		for(int i = startStateIndex; i < endStateIndex; ++i)
		{
			char operatorChar = operatorCharacterList[characterIndex][i].character;

			if(c == operatorChar)
			{
				return i;
			}
		}

		return -1;
	}

	OperatorState getOperatorState(int characterIndex, int stateIndex)
	{
		return operatorCharacterList[characterIndex][stateIndex];
	}

	string getCurrentOperatorString()
	{
		return operatorStringList[currentLexerState.operatorIndex];
	}

	char getLookAheadChar()
	{
		LexerState lexerState;
		copyLexerState(&lexerState, currentLexerState);

		char currentCharacter;

		while(1)
		{
			currentCharacter = getNextCharacter();

			if(!ES_isCharWhitespace(currentCharacter))
			{
				break;
			}
		}

		copyLexerState(&currentLexerState, lexerState);

		return currentCharacter;
	}

	void copyLexerState(LexerState *destinationState, LexerState sourceState)
	{
		strcpy(destinationState->currentLexeme, sourceState.currentLexeme);
		destinationState->currentLexemeStart = sourceState.currentLexemeStart;
		destinationState->currentLexemeEnd = sourceState.currentLexemeEnd;
		destinationState->operatorIndex = sourceState.operatorIndex;
		destinationState->lexerState = sourceState.lexerState;
		destinationState->currentLineIndex = sourceState.currentLineIndex;
		destinationState->currentLineNode = sourceState.currentLineNode;
		destinationState->currentToken = sourceState.currentToken;
	}

	void rewindTokenStream()
	{
		copyLexerState(&currentLexerState, previousLexerState);
	}

	Token getCurrentToken()
	{
		return currentLexerState.currentToken;
	}

	char *getCurrentSourceLine()
	{
		if(currentLexerState.currentLineNode)
		{
			return (char *)currentLexerState.currentLineNode->data;
		}
		else
		{
			return 0;
		}
	}

	int getCurrentSourceLineIndex()
	{
		return currentLexerState.currentLineIndex;
	}

	int getCurrentLexemeStartIndex()
	{
		return currentLexerState.currentLexemeStart;
	}

	int getCurrentLexemeEndIndex()
	{
		return currentLexerState.currentLexemeEnd;
	}

	void throwCodeError(std::string errorMessage)
	{
		Debug::outputLogEvent("");

		Debug::outputLogEvent((char *)((string)"EagleScript Error : " + (string)errorMessage).c_str());
		Debug::outputLogEvent((char *)("Line : " + INT_TO_STRING(getCurrentSourceLineIndex() + 1)).c_str());

		char sourceLineString[ES_MAX_SOURCE_LINE_LENGTH];

		char *currentSourceLine = getCurrentSourceLine();
		if(currentSourceLine)
		{
			Debug::outputLogEvent(currentSourceLine);
		}

		//console.print("EagleScript Error : " + errorMessage, ConsoleOutput_Warning);

		//Debug::throwError(errorMessage + " in line " + INT_TO_STRING(getCurrentSourceLineIndex() + 1));
	}
};
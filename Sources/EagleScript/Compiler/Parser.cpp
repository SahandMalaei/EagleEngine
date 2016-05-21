#include "../EagleScript.h"

#include <unordered_map>

using namespace EagleScript;

#define ES_LOOP_WHILE 0
#define ES_LOOP_FOR 1
#define ES_LOOP_SWITCH 2

namespace EagleScriptCompiler
{
	int currentScope;

	Stack loopStack;

	int preloadFunctionIndex;

	bool parseSourceCodeDirectives();

	bool parseStatement();
	bool parseBlock();
	bool parseFunction();
	bool parseVariable(bool ignoreRedifinition = 0, bool requireSemicolon = 1);
	bool parseExpression();
	bool parseLogicOr();
	bool parseLogicAnd();
	bool parseRelational();
	bool parseSubexpression();
	bool parseTerm();
	bool parseSubterm();
	bool parseFactor();
	bool parseFunctionCall();
	bool parseHostFunctionCall();
	bool parseAssign(bool noSemicolon = 0, bool ignoreRedifinition = 0);
	bool parseReturn();
	bool parseWhile();
	bool parseFor();
	bool parseBreak();
	bool parseContinue();
	bool parseIf();
	bool parseSwitch();
	bool parseDirective();

	bool readToken(Token requiredToken)
	{
		if(getNextToken() != requiredToken)
		{
			switch(requiredToken)
			{
			case ES_TOKEN_TYPE_INT:
				throwCodeError("Integer expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_FLOAT:
				throwCodeError("Float expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_IDENTIFIER:
				throwCodeError("Identifier expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_VARIABLE:
				throwCodeError("Variable expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_TRUE:
				throwCodeError("True expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_FALSE:
				throwCodeError("False expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_SWITCH:
				throwCodeError("Switch expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_IF:
				throwCodeError("If expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_ELSE:
				throwCodeError("Else expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_BREAK:
				throwCodeError("Break expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_CONTINUE:
				throwCodeError("Continue expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_FOR:
				throwCodeError("For expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_WHILE:
				throwCodeError("While expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_FUNCTION:
				throwCodeError("Function expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_RESERVED_RETURN:
				throwCodeError("Return expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_OPERATOR:
				throwCodeError("Operator expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_COMMA:
				throwCodeError(", expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE:
				throwCodeError("( expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE:
				throwCodeError(") expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE:
				throwCodeError("[ expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE:
				throwCodeError("] expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE:
				throwCodeError("{ expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE:
				throwCodeError("} expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_DELIMITER_SEMICOLON:
				throwCodeError("; expected");

				return 0;

				break;

			case ES_TOKEN_TYPE_STRING:
				throwCodeError("String expected");

				return 0;

				break;
			}
		}

		return 1;
	}

	bool parseSourceCode()
	{
		resetEagleScriptLexer();

		if(!parseSourceCodeDirectives())
		{
			return 0;
		}

		preloadFunctionIndex = addFunction(ES_PRELOAD_FUNCTION_NAME);

		currentScope = ES_SCOPE_GLOBAL;

		while(1)
		{
			if(getNextToken() == ES_TOKEN_TYPE_END_OF_STREAM)
			{
				break;
			}
			else
			{
				rewindTokenStream();
			}

			if(!parseStatement())
			{
				return 0;
			}
		}

		int instructionIndex = addICodeInstruction(preloadFunctionIndex, ES_OPCODE_RETURN);

		return 1;
	}

	struct DefineDirectiveInformation
	{
		bool isFunction;

		int line;

		string expression;

		std::vector<std::string> parameterList;
	};

	bool parseSourceCodeDirectives()
	{
		currentScope = ES_SCOPE_GLOBAL;

		std::unordered_map<std::string, DefineDirectiveInformation> defineDirectiveExpressionMap;

		while(1)
		{
			if(getNextToken() == ES_TOKEN_TYPE_END_OF_STREAM)
			{
				break;
			}
			else
			{
				if(getCurrentToken() == ES_TOKEN_TYPE_OPERATOR)
				{
					if(getCurrentOperatorIndex() == ES_OPERATOR_TYPE_BITWISE_XOR)
					{
						getNextToken();

						if(getCurrentToken() == ES_TOKEN_TYPE_IDENTIFIER)
						{
							if(_stricmp(getCurrentLexeme(), "SetStackSize") == 0)
							{
							}
							else if(_stricmp(getCurrentLexeme(), "include") == 0)
							{
							}
							else if(_stricmp(getCurrentLexeme(), "define") == 0)
							{
								if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
								{
									return 0;
								}

								std::string identifier = getCurrentLexeme();

								std::pair<std::string, DefineDirectiveInformation> newPair;
								newPair.first = identifier;

								if(getNextToken() != ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE)
								{
									newPair.second.isFunction = 0;

									rewindTokenStream();
								}
								else
								{
									newPair.second.isFunction = 1;

									while(1)
									{
										if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
										{
											return 0;
										}

										newPair.second.parameterList.push_back(getCurrentLexeme());

										getNextToken();

										if(getCurrentToken() == ES_TOKEN_TYPE_DELIMITER_COMMA)
										{
											continue;
										}
										else if(getCurrentToken() == ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE)
										{
											break;
										}
										else
										{
											if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
											{
												return 0;
											}
										}
									}
								}

								std::string expressionString = (char *)currentLexerState.currentLineNode->data;
								expressionString = expressionString.substr(currentLexerState.currentLexemeEnd + 1, expressionString.length() - 1);

								newPair.second.expression = expressionString;
								newPair.second.line = currentLexerState.currentLineIndex;

								std::string currentLine = (char *)currentLexerState.currentLineNode->data;
								currentLine = "//";
								strcpy((char *)currentLexerState.currentLineNode->data, currentLine.c_str());

								defineDirectiveExpressionMap.insert(newPair);

								LinkedListNode *nextLineNode = currentLexerState.currentLineNode->next;

								currentLexerState.currentLexemeStart = 0;
								currentLexerState.currentLexemeEnd = 0;
								currentLexerState.currentLineNode = nextLineNode;
								currentLexerState.currentLineIndex++;
							}
						}
						else
						{
							throwCodeError("Invalid directive");

							return 0;
						}
					}
				}
			}
		}

		preprocessSourceFile();

		currentLexerState.currentLexemeStart = 0;
		currentLexerState.currentLexemeEnd = 0;
		currentLexerState.currentLineNode = sourceCode.head;
		currentLexerState.currentLineIndex = 0;
		currentLexerState.operatorIndex = 0;
		currentLexerState.lexerState = ES_LEXER_STATE_START;
		currentLexerState.currentToken = 0;

		copyLexerState(&previousLexerState, currentLexerState);

		while(1)
		{
			if(getNextToken() == ES_TOKEN_TYPE_END_OF_STREAM)
			{
				break;
			}
			else
			{
				if(getCurrentToken() == ES_TOKEN_TYPE_IDENTIFIER)
				{
					std::unordered_map<std::string, DefineDirectiveInformation>::iterator listIterator;
					for(listIterator = defineDirectiveExpressionMap.begin(); listIterator != defineDirectiveExpressionMap.end(); ++listIterator)
					{
						int currentLineIndex = currentLexerState.currentLineIndex;

						if(currentLineIndex < (*listIterator).second.line) continue;

						std::string identifier = (*listIterator).first;
						std::string expressionString = (*listIterator).second.expression;

						if(strcmp(getCurrentLexeme(), identifier.c_str()) == 0)
						{
							string currentLine = (char *)currentLexerState.currentLineNode->data;

							if((*listIterator).second.isFunction)
							{
								int expressionStart = currentLexerState.currentLexemeStart;
								int expressionEnd = 0;

								if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
								{
									return 0;
								}

								int parameterCount = 0;
								int targetParameterCount = (*listIterator).second.parameterList.size();

								std::string currentParameter;

								int parentheseCount = 0;

								std::vector<std::string> parameterList;

								for(int i = currentLexerState.currentLexemeEnd; i < currentLine.length(); ++i)
								{
									if(currentLine[i] == '(')
									{
										parentheseCount++;
									}
									else if(currentLine[i] == ')')
									{
										if(parentheseCount == 0)
										{
											parentheseCount = -1;

											expressionEnd = i;

											if(currentParameter.length())
											{
												parameterCount++;

												parameterList.push_back(currentParameter);
											}

											break;
										}

										parentheseCount--;
									}
									else if(currentLine[i] == ',' && parentheseCount == 0)
									{
										parameterCount++;

										parameterList.push_back(currentParameter);

										currentParameter = "";

										continue;
									}

									currentParameter += currentLine[i];
								}

								if(parentheseCount != -1)
								{
									throwCodeError(") expected");

									return 0;
								}

								if(parameterCount != targetParameterCount)
								{
									throwCodeError("Incorrect parameter count");

									return 0;
								}

								currentLine.replace(expressionStart, expressionEnd - expressionStart + 1, expressionString);

								expressionEnd = expressionStart + expressionString.length() - 1;

								int newLineCharacterIndex = expressionEnd;
								currentLine.erase(newLineCharacterIndex, 1);

								strcpy((char *)currentLexerState.currentLineNode->data, currentLine.c_str());

								currentLexerState.currentLexemeEnd = expressionStart;

								if(targetParameterCount)
								{
									while(1)
									{
										getNextToken();

										if(currentLineIndex != currentLexerState.currentLineIndex)
										{
											rewindTokenStream();

											break;
										}

										if(getCurrentToken() != ES_TOKEN_TYPE_IDENTIFIER)
										{
											continue;
										}

										std::string currentIdentifier = getCurrentLexeme();

										int index = 0;
										std::vector<std::string>::iterator listIterator0;
										for(listIterator0 = (*listIterator).second.parameterList.begin(); listIterator0 != (*listIterator).second.parameterList.end(); ++listIterator0)
										{
											if((*listIterator0) == currentIdentifier)
											{
												currentLine.replace(currentLexerState.currentLexemeStart, currentLexerState.currentLexemeEnd - currentLexerState.currentLexemeStart, parameterList[index]);

												strcpy((char *)currentLexerState.currentLineNode->data, currentLine.c_str());
											}

											index++;
										}
									}
								}
							}
							else
							{
								currentLine.replace(currentLexerState.currentLexemeStart, strlen(currentLexerState.currentLexeme), expressionString);

								int newLineCharacterIndex = currentLexerState.currentLexemeStart + expressionString.length() - 1;
								currentLine.erase(newLineCharacterIndex, 1);

								strcpy((char *)currentLexerState.currentLineNode->data, currentLine.c_str());
							}

							currentLexerState.currentLexemeStart = 0;
							currentLexerState.currentLexemeEnd = 0;

							break;
						}
					}
				}
			}
		}

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

	void shutdownParser()
	{
		loopStack.cleanup();
		
	}

	bool parseStatement()
	{
		if(getLookAheadChar() == ';')
		{
			if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
			{
				return 0;
			}

			return 1;
		}

		static char *lastSourceLineString = "";

		Token initialToken = getNextToken();

		switch(initialToken)
		{
		case ES_TOKEN_TYPE_END_OF_STREAM:
			throwError("Unexpected end of script");

			return 0;

			break;

		case ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE:
			if(!parseBlock())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE:
			throwCodeError("Unexpected }");

			return 0;

			break;

		case ES_TOKEN_TYPE_RESERVED_FUNCTION:
			if(!parseFunction())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_VARIABLE:
			if(!parseVariable())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_IDENTIFIER:
			if(getSymbolByIdentifier(getCurrentLexeme(), currentScope))
			{
				if(!parseAssign())
				{
					return 0;
				}
			}
			else if(getFunctionByName(getCurrentLexeme()))
			{
				addICodeSourceLine(currentScope, getCurrentSourceLine());

				if(!parseFunctionCall())
				{
					return 0;
				}

				if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
				{
					return 0;
				}
			}
			else if(isHostFunctionPresent(getCurrentLexeme()))
			{
				addICodeSourceLine(currentScope, getCurrentSourceLine());

				if(!parseHostFunctionCall())
				{
					return 0;
				}

				if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
				{
					return 0;
				}
			}
			else
			{
				throwCodeError("Undeclared identifier \"" + (string)getCurrentLexeme() + "\"");

				return 0;
			}

			break;


		case ES_TOKEN_TYPE_OPERATOR:
			if(getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT)
			{
				if(!parseExpression())
				{
					return 0;
				}

				int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			}
			else if(getCurrentOperatorIndex() == ES_OPERATOR_TYPE_DECREMENT)
			{
				if(!parseExpression())
				{
					return 0;
				}

				int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			}
			else if(getCurrentOperatorIndex() == ES_OPERATOR_TYPE_BITWISE_XOR)
			{
				if(!parseDirective())
				{
					return 0;
				}
			}
			else
			{
				throwCodeError("Invalid expression");

				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_RETURN:
			if(!parseReturn())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_WHILE:
			if(!parseWhile())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_FOR:
			if(!parseFor())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_BREAK:
			if(!parseBreak())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_CONTINUE:
			if(!parseContinue())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_IF:
			if(!parseIf())
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_SWITCH:
			if(!parseSwitch())
			{
				return 0;
			}

			break;

		default:
			throwCodeError("Unexpected token \"" + (string)getCurrentLexeme() + "\"");

			return 0;

			break;
		}

		return 1;
	}

	bool parseBlock()
	{
		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Code block outside function scope");

			return 0;
		}

		while(getLookAheadChar() != '}')
		{
			if(!parseStatement())
			{
				return 0;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE))
		{
			return 0;
		}

		return 1;
	}

	bool parseFunction()
	{
		if(currentScope != ES_SCOPE_GLOBAL)
		{
			throwCodeError("Nested functions are not supported");

			return 0;
		}

		if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
		{
			return 0;
		}

		int functionIndex = addFunction(getCurrentLexeme());

		std::string identifier = getCurrentLexeme();

		FunctionNode *redefinedFunction = 0;
		bool redefinition = 0;

		if(functionIndex == -1)
		{
			redefinition = 1;

			redefinedFunction = getFunctionByName(getCurrentLexeme());

			if(redefinedFunction->hasBody)
			{
				throwCodeError("Function redefinition");

				return 0;
			}

			functionIndex = redefinedFunction->index;
		}

		currentScope = functionIndex;

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		int lastParameterDefaultValueInstructionIndex = -1;

		if(getLookAheadChar() != ')')
		{
			int parameterCount = 0;

			char parameterStringList[ES_MAX_FUNCTION_PARAMETER_COUNT][ES_MAX_IDENTIFIER_LENGTH];

			while(1)
			{
				if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
				{
					return 0;
				}

				copyCurrentLexeme(parameterStringList[parameterCount]);

				int symbolIndex = addSymbol(parameterStringList[parameterCount], 1, currentScope, ES_SYMBOL_TYPE_PARAMETER, parameterCount);
				if(symbolIndex == -1)
				{
					symbolIndex = getSymbolByIdentifier(parameterStringList[parameterCount], currentScope)->index;
				}

				parameterCount++;
				
				if(getLookAheadChar() == '=')
				{
					getNextToken();

					parseExpression();

					int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addSymbolIndexICodeOperand(currentScope, instructionIndex, symbolIndex);
				}

				if(getLookAheadChar() == ')')
				{
					break;
				}

				if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
				{
					return 0;
				}
			}

			if(redefinition)
			{
				if(parameterCount != redefinedFunction->prototypeParameterCount)
				{
					throwCodeError("Function definition is incompatible with the function prototype(wrong parameter count)");

					return 0;
				}
			}
			else
			{
				FunctionNode *currentFunction = getFunctionByName((char *)identifier.c_str());
				currentFunction->prototypeParameterCount = parameterCount;
				currentFunction->parameterCount = parameterCount;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		if(getLookAheadChar() != '{')
		{
			if(redefinition)
			{
				throwCodeError("Function redefinition");

				return 0;
			}

			if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
			{
				return 0;
			}

			currentScope = ES_SCOPE_GLOBAL;
		}
		else
		{
			if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE))
			{
				return 0;
			}

			if(!parseBlock())
			{
				return 0;
			}

			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_RETURN);

			currentScope = ES_SCOPE_GLOBAL;

			FunctionNode *currentFunction = getFunctionByName((char *)identifier.c_str());
			currentFunction->hasBody = 1;
		}

		return 1;
	}

	bool parseVariable(bool ignoreRedifinition, bool requireSemicolon)
	{
		if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
		{
			return 0;
		}

		char identifier[ES_MAX_IDENTIFIER_LENGTH];
		copyCurrentLexeme(identifier);

		int size = 1;

		if(getLookAheadChar() == '[')
		{
			if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE))
			{
				return 0;
			}

			if(!readToken(ES_TOKEN_TYPE_INT))
			{
				return 0;
			}

			size = atoi(getCurrentLexeme());

			if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE))
			{
				return 0;
			}
		}

		if(addSymbol(identifier, size, currentScope, ES_SYMBOL_TYPE_VARIABLE) == -1 && !ignoreRedifinition)
		{
			throwCodeError("Identifier redefinition");

			return 0;
		}

		if(getLookAheadChar() == '=')
		{
			if(currentScope == ES_SCOPE_GLOBAL)
			{
				int instructionIndex;
				int assignmentOperator;

				addICodeSourceLine(preloadFunctionIndex, getCurrentSourceLine());

				SymbolNode *symbol = getSymbolByIdentifier(identifier, currentScope);

				if(!readToken(ES_TOKEN_TYPE_OPERATOR))
				{
					return 0;
				}

				assignmentOperator = getCurrentOperatorIndex();

				if(assignmentOperator != ES_OPERATOR_TYPE_ASSIGN)
				{
					throwCodeError("Invalid assignment");

					return 0;
				}

				currentScope = preloadFunctionIndex;

				if(symbol->size == 1)
				{
					if(!parseExpression())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

					if(symbol->size > 1)
					{
						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

						addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
					}
					else
					{
						addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
					}

					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				}
				else
				{
					if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE))
					{
						return 0;
					}

					int arrayIndex = 0;

					if(!parseExpression())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

					addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					arrayIndex++;

					while(getLookAheadChar() == ',')
					{
						if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
						{
							return 0;
						}

						if(!parseExpression())
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

						addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						arrayIndex++;
					}

					if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE))
					{
						return 0;
					}

					if(arrayIndex != symbol->size)
					{
						throwCodeError("Incorrect element count. Array size is " + INT_TO_STRING(symbol->size) + " elements");

						return 0;
					}
				}

				//Debug::throwMessage("Assignment complete");

				currentScope = ES_SCOPE_GLOBAL;
			}
			else
			{

				int instructionIndex;
				int assignmentOperator;

				addICodeSourceLine(currentScope, getCurrentSourceLine());

				SymbolNode *symbol = getSymbolByIdentifier(identifier, currentScope);

				if(!readToken(ES_TOKEN_TYPE_OPERATOR))
				{
					return 0;
				}

				assignmentOperator = getCurrentOperatorIndex();

				if(assignmentOperator != ES_OPERATOR_TYPE_ASSIGN)
				{
					throwCodeError("Invalid assignment");

					return 0;
				}

				if(symbol->size == 1)
				{
					if(!parseExpression())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

					if(symbol->size > 1)
					{
						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

						addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
					}
					else
					{
						addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
					}

					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				}
				else
				{
					if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE))
					{
						return 0;
					}

					int arrayIndex = 0;

					if(!parseExpression())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

					addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					arrayIndex++;

					while(getLookAheadChar() == ',')
					{
						if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
						{
							return 0;
						}

						if(!parseExpression())
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

						addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						arrayIndex++;
					}

					if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE))
					{
						return 0;
					}

					if(arrayIndex != symbol->size)
					{
						throwCodeError("Incorrect element count. Array size is " + INT_TO_STRING(symbol->size) + " elements");

						return 0;
					}
				}
			}
		}

		while(getLookAheadChar() == ',')
		{
			if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
			{
				return 0;
			}

			if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
			{
				return 0;
			}

			char identifier[ES_MAX_IDENTIFIER_LENGTH];
			copyCurrentLexeme(identifier);

			int size = 1;

			if(getLookAheadChar() == '[')
			{
				if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE))
				{
					return 0;
				}

				if(!readToken(ES_TOKEN_TYPE_INT))
				{
					return 0;
				}

				size = atoi(getCurrentLexeme());

				if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE))
				{
					return 0;
				}
			}

			if(addSymbol(identifier, size, currentScope, ES_SYMBOL_TYPE_VARIABLE) == -1 && !ignoreRedifinition)
			{
				throwCodeError("Identifier redefinition");

				return 0;
			}

			if(getLookAheadChar() == '=')
			{
				if(currentScope == ES_SCOPE_GLOBAL)
				{
					//throwCodeError("Assignment in global scope");

					int instructionIndex;
					int assignmentOperator;

					addICodeSourceLine(preloadFunctionIndex, getCurrentSourceLine());

					SymbolNode *symbol = getSymbolByIdentifier(identifier, currentScope);

					if(!readToken(ES_TOKEN_TYPE_OPERATOR))
					{
						return 0;
					}

					assignmentOperator = getCurrentOperatorIndex();

					if(assignmentOperator != ES_OPERATOR_TYPE_ASSIGN)
					{
						throwCodeError("Invalid assignment");
						
						return 0;
					}

					currentScope = preloadFunctionIndex;

					//Debug::throwMessage(INT_TO_STRING(preloadFunctionIndex));

					if(symbol->size == 1)
					{
						if(!parseExpression())
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

						if(symbol->size > 1)
						{
							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
							addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

							addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
						}
						else
						{
							addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
						}

						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
					}
					else
					{
						if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE))
						{
							return 0;
						}

						int arrayIndex = 0;

						if(!parseExpression())
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

						addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						arrayIndex++;

						while(getLookAheadChar() == ',')
						{
							if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
							{
								return 0;
							}

							if(!parseExpression())
							{
								return 0;
							}

							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
							addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

							addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

							addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

							arrayIndex++;
						}

						if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE))
						{
							return 0;
						}

						if(arrayIndex != symbol->size)
						{
							throwCodeError("Incorrect element count. Array size is " + INT_TO_STRING(symbol->size) + " elements");

							return 0;
						}
					}

					currentScope = ES_SCOPE_GLOBAL;
				}
				else
				{

					int instructionIndex;
					int assignmentOperator;

					addICodeSourceLine(currentScope, getCurrentSourceLine());

					SymbolNode *symbol = getSymbolByIdentifier(identifier, currentScope);

					if(!readToken(ES_TOKEN_TYPE_OPERATOR))
					{
						return 0;
					}

					assignmentOperator = getCurrentOperatorIndex();

					if(assignmentOperator != ES_OPERATOR_TYPE_ASSIGN)
					{
						throwCodeError("Invalid assignment");

						return 0;
					}

					if(symbol->size == 1)
					{
						if(!parseExpression())
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

						if(symbol->size > 1)
						{
							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
							addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

							addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
						}
						else
						{
							addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
						}

						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
					}
					else
					{
						if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE))
						{
							return 0;
						}

						int arrayIndex = 0;

						if(!parseExpression())
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

						addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

						arrayIndex++;

						while(getLookAheadChar() == ',')
						{
							if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
							{
								return 0;
							}

							if(!parseExpression())
							{
								return 0;
							}

							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
							addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

							addArrayIndexICodeOperand(currentScope, instructionIndex, symbol->index, arrayIndex);

							addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

							arrayIndex++;
						}

						if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE))
						{
							return 0;
						}

						if(arrayIndex != symbol->size)
						{
							throwCodeError("Incorrect element count. Array size is " + INT_TO_STRING(symbol->size) + " elements");

							return 0;
						}
					}
				}
			}
		}

		if(requireSemicolon)
		{
			if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
			{
				return 0;
			}
		}

		return 1;
	}

	bool parseExpression()
	{
		if(!parseLogicOr())
		{
			return 0;
		}

		return 1;
	}

	bool parseLogicOr()
	{
		int instructionIndex;
		int operatorType;

		if(!parseLogicAnd())
		{
			return 0;
		}

		while(1)
		{
			if(getNextToken() != ES_TOKEN_TYPE_OPERATOR || getCurrentOperatorIndex() != ES_OPERATOR_TYPE_LOGICAL_OR)
			{
				rewindTokenStream();

				break;
			}

			operatorType = getCurrentOperatorIndex();

			if(!parseLogicAnd())
			{
				return 0;
			}

			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			int targetIndex0 = getNextICodeJumpTargetIndex();
			int targetIndex1 = getNextICodeJumpTargetIndex();
			int targetIndex2 = getNextICodeJumpTargetIndex();

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

			addICodeJumpTarget(currentScope, targetIndex0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JNE);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex2);

			addICodeJumpTarget(currentScope, targetIndex1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, 1);

			addICodeJumpTarget(currentScope, targetIndex2);
		}

		return 1;
	}

	bool parseLogicAnd()
	{
		int instructionIndex;
		int operatorType;

		if(!parseRelational())
		{
			return 0;
		}

		while(1)
		{
			if(getNextToken() != ES_TOKEN_TYPE_OPERATOR || getCurrentOperatorIndex() != ES_OPERATOR_TYPE_LOGICAL_AND)
			{
				rewindTokenStream();

				break;
			}

			operatorType = getCurrentOperatorIndex();

			if(!parseRelational())
			{
				return 0;
			}

			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			int targetIndex0 = getNextICodeJumpTargetIndex();
			int targetIndex1 = getNextICodeJumpTargetIndex();

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, 1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

			addICodeJumpTarget(currentScope, targetIndex0);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);

			addICodeJumpTarget(currentScope, targetIndex1);
		}

		return 1;
	}

	bool parseRelational()
	{
		int instructionIndex;
		int operatorType;

		if(!parseSubexpression())
		{
			return 0;
		}

		while(1)
		{
			if(getNextToken() != ES_TOKEN_TYPE_OPERATOR || (getCurrentOperatorIndex() != ES_OPERATOR_TYPE_EQUAL && 
				getCurrentOperatorIndex() != ES_OPERATOR_TYPE_NOT_EQUAL && getCurrentOperatorIndex() != ES_OPERATOR_TYPE_GREATER &&
				getCurrentOperatorIndex() != ES_OPERATOR_TYPE_LESS && getCurrentOperatorIndex() != ES_OPERATOR_TYPE_GREATER_EQUAL && 
				getCurrentOperatorIndex() != ES_OPERATOR_TYPE_LESS_EQUAL))
			{
				//message("Current token type : " + INT_TO_STRING(getCurrentToken()));
				//message("Current operator type : " + INT_TO_STRING(getCurrentOperatorIndex()));

				rewindTokenStream();

				break;
			}

			operatorType = getCurrentOperatorIndex();

			if(!parseSubexpression())
			{
				return 0;
			}

			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			if(operatorType == ES_OPERATOR_TYPE_EQUAL)
			{
				int targetIndex0 = getNextICodeJumpTargetIndex();
				int targetIndex1 = getNextICodeJumpTargetIndex();

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

				addICodeJumpTarget(currentScope, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 1);

				addICodeJumpTarget(currentScope, targetIndex1);
			}
			else if(operatorType == ES_OPERATOR_TYPE_NOT_EQUAL)
			{
				int targetIndex0 = getNextICodeJumpTargetIndex();
				int targetIndex1 = getNextICodeJumpTargetIndex();

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JNE);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

				addICodeJumpTarget(currentScope, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 1);

				addICodeJumpTarget(currentScope, targetIndex1);
			}
			else if(operatorType == ES_OPERATOR_TYPE_GREATER)
			{
				int targetIndex0 = getNextICodeJumpTargetIndex();
				int targetIndex1 = getNextICodeJumpTargetIndex();

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JG);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

				addICodeJumpTarget(currentScope, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 1);

				addICodeJumpTarget(currentScope, targetIndex1);
			}
			else if(operatorType == ES_OPERATOR_TYPE_LESS)
			{
				int targetIndex0 = getNextICodeJumpTargetIndex();
				int targetIndex1 = getNextICodeJumpTargetIndex();

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JL);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

				addICodeJumpTarget(currentScope, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 1);

				addICodeJumpTarget(currentScope, targetIndex1);
			}
			else if(operatorType == ES_OPERATOR_TYPE_GREATER_EQUAL)
			{
				int targetIndex0 = getNextICodeJumpTargetIndex();
				int targetIndex1 = getNextICodeJumpTargetIndex();

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JGE);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

				addICodeJumpTarget(currentScope, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 1);

				addICodeJumpTarget(currentScope, targetIndex1);
			}
			else if(operatorType == ES_OPERATOR_TYPE_LESS_EQUAL)
			{
				int targetIndex0 = getNextICodeJumpTargetIndex();
				int targetIndex1 = getNextICodeJumpTargetIndex();

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JLE);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
				addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

				addICodeJumpTarget(currentScope, targetIndex0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addIntegerICodeOperand(currentScope, instructionIndex, 1);

				addICodeJumpTarget(currentScope, targetIndex1);
			}
		}

		return 1;
	}

	bool parseSubexpression()
	{
		int instructionIndex;
		int operatorType;

		if(!parseTerm())
		{
			return 0;
		}

		while(1)
		{
			if(getNextToken() != ES_TOKEN_TYPE_OPERATOR || (getCurrentOperatorIndex() != ES_OPERATOR_TYPE_ADD && getCurrentOperatorIndex() != ES_OPERATOR_TYPE_SUBTRACT &&
				getCurrentOperatorIndex() != ES_OPERATOR_TYPE_BITWISE_AND && getCurrentOperatorIndex() != ES_OPERATOR_TYPE_BITWISE_OR &&
				getCurrentOperatorIndex() != ES_OPERATOR_TYPE_BITWISE_SHIFT_LEFT && getCurrentOperatorIndex() != ES_OPERATOR_TYPE_BITWISE_SHIFT_RIGHT))
			{
				//message("Current token type : " + INT_TO_STRING(getCurrentToken()));
				//message("Current operator type : " + INT_TO_STRING(getCurrentOperatorIndex()));

				rewindTokenStream();

				break;
			}

			operatorType = getCurrentOperatorIndex();

			if(!parseTerm())
			{
				return 0;
			}

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			int operatorInstruction = 0;

			if(operatorType == ES_OPERATOR_TYPE_ADD)
			{
				operatorInstruction = ES_OPCODE_ADD;
			}
			else if(operatorType == ES_OPERATOR_TYPE_SUBTRACT)
			{
				operatorInstruction = ES_OPCODE_SUB;
			}
			else if(operatorType == ES_OPERATOR_TYPE_BITWISE_AND)
			{
				operatorInstruction = ES_OPCODE_AND;
			}
			else if(operatorType == ES_OPERATOR_TYPE_BITWISE_OR)
			{
				operatorInstruction = ES_OPCODE_OR;
			}
			else if(operatorType == ES_OPERATOR_TYPE_BITWISE_SHIFT_LEFT)
			{
				operatorInstruction = ES_OPCODE_SHL;
			}
			else if(operatorType == ES_OPERATOR_TYPE_BITWISE_SHIFT_RIGHT)
			{
				operatorInstruction = ES_OPCODE_SHR;
			}

			instructionIndex = addICodeInstruction(currentScope, operatorInstruction);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
		}

		return 1;
	}

	bool parseTerm()
	{
		int instructionIndex;

		int operatorType;

		if(!parseSubterm())
		{
			return 0;
		}

		while(1)
		{
			if(getNextToken() != ES_TOKEN_TYPE_OPERATOR || (getCurrentOperatorIndex() != ES_OPERATOR_TYPE_MULTIPLY && getCurrentOperatorIndex() != ES_OPERATOR_TYPE_DIVIDE &&
				getCurrentOperatorIndex() != ES_OPERATOR_TYPE_MOD))
			{
				rewindTokenStream();

				break;
			}

			operatorType = getCurrentOperatorIndex();

			if(!parseSubterm())
			{
				return 0;
			}

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			int operatorInstruction = 0;

			if(operatorType == ES_OPERATOR_TYPE_MULTIPLY)
			{
				operatorInstruction = ES_OPCODE_MUL;
			}
			else if(operatorType == ES_OPERATOR_TYPE_DIVIDE)
			{
				operatorInstruction = ES_OPCODE_DIV;
			}
			else if(operatorType == ES_OPERATOR_TYPE_MOD)
			{
				operatorInstruction = ES_OPCODE_MOD;
			}

			instructionIndex = addICodeInstruction(currentScope, operatorInstruction);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
		}

		return 1;
	}

	bool parseSubterm()
	{
		int instructionIndex;

		int operatorType;

		if(!parseFactor())
		{
			return 0;
		}

		while(1)
		{
			if(getNextToken() != ES_TOKEN_TYPE_OPERATOR || (getCurrentOperatorIndex() != ES_OPERATOR_TYPE_EXPONENT))
			{
				rewindTokenStream();

				break;
			}

			if(!parseFactor())
			{
				return 0;
			}

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			int operatorInstruction = ES_OPCODE_EXP;

			instructionIndex = addICodeInstruction(currentScope, operatorInstruction);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
		}

		return 1;
	}

	bool parseFactor()
	{
		int instructionIndex;
		int operatorType;
		bool isUnaryOperatorPending = 0;

		bool isSymbol = 0;
		int symbolIndex = -1;

		if(getNextToken() == ES_TOKEN_TYPE_OPERATOR && (getCurrentOperatorIndex() == ES_OPERATOR_TYPE_ADD || getCurrentOperatorIndex() == ES_OPERATOR_TYPE_SUBTRACT || 
			getCurrentOperatorIndex() == ES_OPERATOR_TYPE_BITWISE_NOT || getCurrentOperatorIndex() == ES_OPERATOR_TYPE_LOGICAL_NOT || 
			getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT || getCurrentOperatorIndex() == ES_OPERATOR_TYPE_DECREMENT))
		{
			isUnaryOperatorPending = 1;

			operatorType = getCurrentOperatorIndex();
		}
		else
		{
			rewindTokenStream();
		}

		switch(getNextToken())
		{
		case ES_TOKEN_TYPE_INT:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, atoi(getCurrentLexeme()));

			break;

		case ES_TOKEN_TYPE_FLOAT:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addFloatICodeOperand(currentScope, instructionIndex, atof(getCurrentLexeme()));

			break;

		case ES_TOKEN_TYPE_IDENTIFIER:
			{
				int precedingInstruction = -1;

				FunctionNode *function = 0;

				rewindTokenStream();

				if(getCurrentToken() == ES_TOKEN_TYPE_OPERATOR && (getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT || getCurrentOperatorIndex() == ES_OPERATOR_TYPE_DECREMENT))
				{
					precedingInstruction = getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT ? ES_OPCODE_INC : ES_OPCODE_DEC;
				}

				getNextToken();

				SymbolNode *symbol = getSymbolByIdentifier(getCurrentLexeme(), currentScope);

				if(symbol)
				{
					isSymbol = 1;
					symbolIndex = symbol->index;

					if(getLookAheadChar() == '[')
					{
						if(symbol->size == 1)
						{
							throwCodeError("The addressed symbol is not an array");

							return 0;
						}

						if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE))
						{
							return 0;
						}

						if(getLookAheadChar() == ']')
						{
							throwCodeError("Array index expected");

							return 0;
						}

						if(!parseExpression())
						{
							return 0;
						}

						if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE))
						{
							return 0;
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);

						if(precedingInstruction != -1)
						{
							int instruction = precedingInstruction;

							instructionIndex = addICodeInstruction(currentScope, instruction);
							addArrayICodeOperand(currentScope, instructionIndex, symbol->index);
						}

						instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
						addArrayICodeOperand(currentScope, instructionIndex, symbol->index);

						if(getNextToken() == ES_TOKEN_TYPE_OPERATOR && (getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT || getCurrentOperatorIndex() == ES_OPERATOR_TYPE_DECREMENT))
						{
							int instruction = getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT ? ES_OPCODE_INC : ES_OPCODE_DEC;

							instructionIndex = addICodeInstruction(currentScope, instruction);
							addArrayICodeOperand(currentScope, instructionIndex, symbol->index);
						}
						else
						{
							rewindTokenStream();
						}
					}
					else
					{
						if(symbol->size == 1)
						{
							if(precedingInstruction != -1)
							{
								int instruction = precedingInstruction;

								instructionIndex = addICodeInstruction(currentScope, instruction);
								addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
							}

							instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
							addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);

							if(getNextToken() == ES_TOKEN_TYPE_OPERATOR && (getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT || getCurrentOperatorIndex() == ES_OPERATOR_TYPE_DECREMENT))
							{
								int instruction = getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT ? ES_OPCODE_INC : ES_OPCODE_DEC;

								instructionIndex = addICodeInstruction(currentScope, instruction);
								addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
							}
							else
							{
								rewindTokenStream();
							}
						}
						else
						{
							throwCodeError("Array index expected");

							return 0;
						}
					}
				}
				else if(function = getFunctionByName(getCurrentLexeme()))
				{
					if(!parseFunctionCall())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_RETURN_VALUE);
				}
				else if(isHostFunctionPresent(getCurrentLexeme()))
				{
					if(!parseHostFunctionCall())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_HOST_RETURN_VALUE);
				}
				else
				{
					throwCodeError("Use of undeclared identifier \"" + (string)getCurrentLexeme() + "\"");

					return 0;
				}

				break;
			}

		case ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE:
			if(!parseExpression())
			{
				return 0;
			}

			if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
			{
				return 0;
			}

			break;

		case ES_TOKEN_TYPE_RESERVED_TRUE:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, 1);

			break;

		case ES_TOKEN_TYPE_RESERVED_FALSE:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
			addIntegerICodeOperand(currentScope, instructionIndex, 0);

			break;

		case ES_TOKEN_TYPE_STRING:
			{
				int stringIndex = stringTable.addStringNode(getCurrentLexeme());
				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addStringIndexICodeOperand(currentScope, instructionIndex, stringIndex);

				break;
			}

		default:
			throwCodeError("Invalid expression");

			return 0;

			break;
		}

		if(isUnaryOperatorPending)
		{
			int instruction = -1;

			switch(operatorType)
			{
			case ES_OPERATOR_TYPE_SUBTRACT:
				instruction = ES_OPCODE_NEG;

				break;

			case ES_OPERATOR_TYPE_BITWISE_NOT:
				instruction = ES_OPCODE_NOT;

				break;

			case ES_OPERATOR_TYPE_LOGICAL_NOT:
				{
					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					int targetIndex0 = getNextICodeJumpTargetIndex();
					int targetIndex1 = getNextICodeJumpTargetIndex();

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
					addIntegerICodeOperand(currentScope, instructionIndex, 0);
					addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
					addIntegerICodeOperand(currentScope, instructionIndex, 0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
					addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

					addICodeJumpTarget(currentScope, targetIndex0);

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
					addIntegerICodeOperand(currentScope, instructionIndex, 1);

					addICodeJumpTarget(currentScope, targetIndex1);

					break;
				}
			}

			if(instruction != -1)
			{
				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

				instructionIndex = addICodeInstruction(currentScope, instruction);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_PUSH);
				addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			}
		}

		return 1;
	}

	bool parseFunctionCall()
	{
		FunctionNode *function = getFunctionByName(getCurrentLexeme());

		int parameterCount = 0;

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		while(1)
		{
			if(getLookAheadChar() != ')')
			{
				if(!parseExpression())
				{
					return 0;
				}

				/*if(parameterCount > function->parameterCount) 
				{
					throwCodeError("Too many parameters. The function \"" + (string)function->name + "\" only accepts " + INT_TO_STRING(function->parameterCount) + " parameters");

					return 0;
				}*/

				if(getLookAheadChar() != ')')
				{
					if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
					{
						return 0;
					}

					if(getLookAheadChar() == ')')
					{
						throwCodeError("Expected an expression");

						return 0;
					}
				}

				parameterCount++;
			}
			else
			{
				break;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		/*if(parameterCount < function->parameterCount) 
		{
			throwCodeError("Too few parameters. The function \"" + (string)function->name + "\" accepts " + INT_TO_STRING(function->parameterCount) + " parameters");

			return 0;
		}*/

		for(int i = parameterCount - 1; i > -1; --i)
		{
			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addFunctionParameterListICodeOperand(currentScope, instructionIndex, i);
		}

		int callInstruction = ES_OPCODE_CALL;

		int instructionIndex = addICodeInstruction(currentScope, callInstruction);
		addFunctionIndexICodeOperand(currentScope, instructionIndex, function->index);

		for(int i = 0; i < parameterCount; ++i)
		{
			addFunctionParameterListICodeOperand(currentScope, instructionIndex, i);
		}

		return 1;
	}

	bool parseHostFunctionCall()
	{
		int hostFunctionIndex = getHostFunctionIndex(getCurrentLexeme());

		int parameterCount = 0;

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		while(1)
		{
			if(getLookAheadChar() != ')')
			{
				if(!parseExpression())
				{
					return 0;
				}

				if(getLookAheadChar() != ')')
				{
					if(!readToken(ES_TOKEN_TYPE_DELIMITER_COMMA))
					{
						return 0;
					}

					if(getLookAheadChar() == ')')
					{
						throwCodeError("Expected an expression");

						return 0;
					}
				}

				parameterCount++;
			}
			else
			{
				break;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		for(int i = parameterCount - 1; i > -1; --i)
		{
			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addFunctionParameterListICodeOperand(currentScope, instructionIndex, i);
		}

		int callInstruction = ES_OPCODE_CALLHOST;

		int instructionIndex = addICodeInstruction(currentScope, callInstruction);
		addFunctionIndexICodeOperand(currentScope, instructionIndex, (hostFunctionIndex * -1) - 1);

		for(int i = 0; i < parameterCount; ++i)
		{
			addFunctionParameterListICodeOperand(currentScope, instructionIndex, i);
		}

		return 1;
	}

	bool parseAssign(bool noSemicolon, bool ignoreRedifinition)
	{
		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Assignment in global scope detected");

			return 0;
		}

		int instructionIndex;
		int assignmentOperator;

		if(!ignoreRedifinition)
		{
			addICodeSourceLine(currentScope, getCurrentSourceLine());
		}

		SymbolNode *symbol = getSymbolByIdentifier(getCurrentLexeme(), currentScope);

		bool isArray = 0;

		if(getLookAheadChar() == '[')
		{
			if(symbol->size == 1)
			{
				throwCodeError("\"" + (string)getCurrentLexeme() + "\" is not an array");

				return 0;
			}

			if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE))
			{
				return 0;
			}

			if(getLookAheadChar() == ']')
			{
				throwCodeError("Missing array index");

				return 0;
			}

			if(!parseExpression())
			{
				return 0;
			}

			if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE))
			{
				return 0;
			}

			isArray = 1;
		}
		else
		{
			if(symbol->size > 1)
			{
				throwCodeError("Expected an array index");

				return 0;
			}
		}

		getNextToken();

		assignmentOperator = getCurrentOperatorIndex();

		if(getCurrentToken() == ES_TOKEN_TYPE_OPERATOR)
		{
			if(assignmentOperator == ES_OPERATOR_TYPE_INCREMENT)
			{
				if(isArray)
				{
					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				}

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_INC);

				if(isArray)
				{
					addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
				}
				else
				{
					addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
				}

				if(!noSemicolon)
				{
					if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
					{
						return 0;
					}
				}

				return 1;
			}
			else if(assignmentOperator == ES_OPERATOR_TYPE_DECREMENT)
			{
				if(isArray)
				{
					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
				}

				instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_DEC);

				if(isArray)
				{
					addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
				}
				else
				{
					addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
				}

				if(!noSemicolon)
				{
					if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
					{
						return 0;
					}
				}

				return 1;
			}
		}
		else if(getCurrentToken() != ES_TOKEN_TYPE_OPERATOR || 
			(assignmentOperator != ES_OPERATOR_TYPE_ASSIGN && assignmentOperator != ES_OPERATOR_TYPE_ASSIGN_ADD &&
			assignmentOperator != ES_OPERATOR_TYPE_ASSIGN_SUBTRACT && assignmentOperator != ES_OPERATOR_TYPE_ASSIGN_MULTIPLY &&
			assignmentOperator != ES_OPERATOR_TYPE_ASSIGN_DIVIDE && assignmentOperator != ES_OPERATOR_TYPE_ASSIGN_MOD &&
			assignmentOperator != ES_OPERATOR_TYPE_ASSIGN_EXPONENT && assignmentOperator != ES_OPERATOR_TYPE_BITWISE_ASSIGN_AND &&
			assignmentOperator != ES_OPERATOR_TYPE_BITWISE_ASSIGN_OR && assignmentOperator != ES_OPERATOR_TYPE_BITWISE_ASSIGN_XOR &&
			assignmentOperator != ES_OPERATOR_TYPE_BITWISE_ASSIGN_SHIFT_LEFT && assignmentOperator != ES_OPERATOR_TYPE_BITWISE_ASSIGN_SHIFT_RIGHT))
		{
			throwCodeError("Invalid assignment operator");

			return 0;
		}

		if(!parseExpression())
		{
			return 0;
		}

		if(!noSemicolon)
		{
			if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
			{
				return 0;
			}
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

		if(isArray)
		{
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_1);
		}

		switch(assignmentOperator)
		{
		case ES_OPERATOR_TYPE_ASSIGN:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOV);

			break;

		case ES_OPERATOR_TYPE_ASSIGN_ADD:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_ADD);

			break;

		case ES_OPERATOR_TYPE_ASSIGN_SUBTRACT:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_SUB);

			break;

		case ES_OPERATOR_TYPE_ASSIGN_MULTIPLY:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MUL);

			break;

		case ES_OPERATOR_TYPE_ASSIGN_DIVIDE:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_DIV);

			break;

		case ES_OPERATOR_TYPE_ASSIGN_MOD:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_MOD);

			break;

		case ES_OPERATOR_TYPE_ASSIGN_EXPONENT:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_EXP);

			break;

		case ES_OPERATOR_TYPE_BITWISE_ASSIGN_AND:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_AND);

			break;

		case ES_OPERATOR_TYPE_BITWISE_ASSIGN_OR:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_OR);

			break;

		case ES_OPERATOR_TYPE_BITWISE_ASSIGN_XOR:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_XOR);

			break;

		case ES_OPERATOR_TYPE_BITWISE_ASSIGN_SHIFT_LEFT:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_SHL);

			break;

		case ES_OPERATOR_TYPE_BITWISE_ASSIGN_SHIFT_RIGHT:
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_SHR);

			break;
		}

		if(isArray)
		{
			addArrayICodeOperand1(currentScope, instructionIndex, symbol->index);
		}
		else
		{
			addSymbolIndexICodeOperand(currentScope, instructionIndex, symbol->index);
		}

		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

		return 1;
	}

	bool parseReturn()
	{
		int instructionIndex;

		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Return outside function scope");

			return 0;
		}

		addICodeSourceLine(currentScope, getCurrentSourceLine());

		if(getLookAheadChar() != ';')
		{
			if(!parseExpression())
			{
				return 0;
			}

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_RETURN_VALUE);
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_RETURN);

		return 1;
	}

	bool parseWhile()
	{
		int instructionIndex;

		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Statement outside function scope");

			return 0;
		}

		addICodeSourceLine(currentScope, getCurrentSourceLine());

		int targetIndex0 = getNextICodeJumpTargetIndex();
		int targetIndex1 = getNextICodeJumpTargetIndex();

		addICodeJumpTarget(currentScope, targetIndex0);

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		if(!parseExpression())
		{
			return 0;
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
		addIntegerICodeOperand(currentScope, instructionIndex, 0);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

		LoopStackElement *loop = new LoopStackElement();
		loop->type = ES_LOOP_WHILE;
		loop->startTargetIndex = targetIndex0;
		loop->endTargetIndex = targetIndex1;

		loopStack.push(loop);

		if(!parseStatement())
		{
			return 0;
		}

		loopStack.removeTopElement();

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

		addICodeJumpTarget(currentScope, targetIndex1);

		return 1;
	}

	bool parseBreak()
	{
		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Statement outside function scope");

			return 0;
		}

		if(loopStack.isEmpty())
		{
			throwCodeError("Break outside loop");

			return 0;
		}

		addICodeSourceLine(currentScope, getCurrentSourceLine());

		int targetIndex = ((LoopStackElement *)loopStack.peekTopElement())->endTargetIndex;

		int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex);

		return 1;
	}

	bool parseContinue()
	{
		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Statement outside function scope");

			return 0;
		}

		if(loopStack.isEmpty())
		{
			throwCodeError("Continue outside loop");

			return 0;
		}

		addICodeSourceLine(currentScope, getCurrentSourceLine());

		int targetIndex; // = ((LoopStackElement *)loopStack.peekTopElement())->startTargetIndex;

		if(((LoopStackElement *)loopStack.peekTopElement())->type == ES_LOOP_WHILE)
		{
			targetIndex = ((LoopStackElement *)loopStack.peekTopElement())->startTargetIndex;
		}
		else if(((LoopStackElement *)loopStack.peekTopElement())->type == ES_LOOP_FOR)
		{
			targetIndex = ((LoopStackElement *)loopStack.peekTopElement())->startTargetIndex + 1;
		}
		else
		{
			throwCodeError("Illegal usage of the \"continue\" token");

			return 0;
		}

		int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex);

		return 1;
	}

	bool parseIf()
	{
		int instructionIndex;

		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Statement outside function scope");

			return 0;
		}

		addICodeSourceLine(currentScope, getCurrentSourceLine());

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		if(!parseExpression())
		{
			return 0;
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

		int skipFalseTargetIndex = getNextICodeJumpTargetIndex();

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
		addIntegerICodeOperand(currentScope, instructionIndex, 0);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, skipFalseTargetIndex);

		if(!parseStatement())
		{
			return 0;
		}

		if(getNextToken() == ES_TOKEN_TYPE_RESERVED_ELSE)
		{
			int trueTargetIndex = getNextICodeJumpTargetIndex();

			int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, trueTargetIndex);

			addICodeJumpTarget(currentScope, skipFalseTargetIndex);

			if(!parseStatement())
			{
				return 0;
			}

			addICodeJumpTarget(currentScope, trueTargetIndex);
		}
		else
		{
			rewindTokenStream();

			addICodeJumpTarget(currentScope, skipFalseTargetIndex);
		}

		return 1;
	}

	bool parseSwitch()
	{
		int instructionIndex;

		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Statement outside function scope");

			return 0;
		}

		addICodeSourceLine(currentScope, getCurrentSourceLine());

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		if(!parseExpression())
		{
			return 0;
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_2);

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE))
		{
			return 0;
		}

		LexerState initialLexerState = currentLexerState;

		std::vector<int> jumpTargetList;

		int passedOpenCurlyBraceCount = 0;

		bool isDefaultCasePresent = 0;
		int defaultCaseJumpTarget = -1;

		if(!readToken(ES_TOKEN_TYPE_RESERVED_CASE))
		{
			return 0;
		}

		while(1)
		{
			if(getLookAheadChar() == '}' && passedOpenCurlyBraceCount == 0) 
			{
				break;
			}
			else if(getLookAheadChar() == '}')
			{
				passedOpenCurlyBraceCount--;
			}

			if(!parseExpression())
			{
				return 0;
			}

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

			if(!readToken(ES_TOKEN_TYPE_DELIMITER_COLON))
			{
				return 0;
			}

			int jumpTarget = getNextICodeJumpTargetIndex();

			jumpTargetList.push_back(jumpTarget);

			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_2);
			addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, jumpTarget);

			while(getNextToken() != ES_TOKEN_TYPE_RESERVED_CASE)
			{
				Token currentToken = getCurrentToken();

				if(currentToken == ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE) passedOpenCurlyBraceCount++;

				if(currentToken == ES_TOKEN_TYPE_END_OF_STREAM || currentToken == ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE && passedOpenCurlyBraceCount == 0)
				{
					rewindTokenStream();

					break;
				}
				else if(currentToken == ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE)
				{
					passedOpenCurlyBraceCount--;
				}

				if(currentToken == ES_TOKEN_TYPE_RESERVED_DEFAULT)
				{
					if(isDefaultCasePresent) throwCodeError("Duplicate default case");

					isDefaultCasePresent = 1;

					int jumpTarget = getNextICodeJumpTargetIndex();

					jumpTargetList.push_back(jumpTarget);

					defaultCaseJumpTarget = jumpTarget;

					if(!readToken(ES_TOKEN_TYPE_DELIMITER_COLON))
					{
						return 0;
					}
				}
			}
		}

		currentLexerState = initialLexerState;

		int switchEndJumpTarget = getNextICodeJumpTargetIndex();

		if(!isDefaultCasePresent)
		{
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, switchEndJumpTarget);
		}
		else
		{
			instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
			addJumpTargetIndexICodeOperand(currentScope, instructionIndex, defaultCaseJumpTarget);
		}

		LoopStackElement *loop = new LoopStackElement();
		loop->type = ES_LOOP_SWITCH;
		loop->startTargetIndex = -1;
		loop->endTargetIndex = switchEndJumpTarget;

		loopStack.push(loop);

		int currentJumpTargetIndex = 0;

		while(1)
		{
			if(getLookAheadChar() == '}') break;

			while(getNextToken() != ES_TOKEN_TYPE_DELIMITER_COLON)
			{
				Token currentToken = getCurrentToken();

				if(currentToken == ES_TOKEN_TYPE_END_OF_STREAM || currentToken == ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE) break;
			}

			addICodeSourceLine(currentScope, getCurrentSourceLine());

			addICodeJumpTarget(currentScope, jumpTargetList[currentJumpTargetIndex]);

			currentJumpTargetIndex++;

			getNextToken();

			if(getCurrentToken() == ES_TOKEN_TYPE_RESERVED_CASE || getCurrentToken() == ES_TOKEN_TYPE_RESERVED_DEFAULT) continue;
			else rewindTokenStream();

			if(!parseStatement())
			{
				return 0;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE))
		{
			return 0;
		}

		addICodeJumpTarget(currentScope, switchEndJumpTarget);

		loopStack.removeTopElement();

		return 1;
	}

	bool parseFor()
	{
		int instructionIndex;

		if(currentScope == ES_SCOPE_GLOBAL)
		{
			throwCodeError("Statement outside function scope");

			return 0;
		}

		int targetIndex0 = getNextICodeJumpTargetIndex();
		int targetIndex1 = getNextICodeJumpTargetIndex();
		int targetIndex2 = getNextICodeJumpTargetIndex();
		int targetIndex3 = getNextICodeJumpTargetIndex();

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE))
		{
			return 0;
		}

		getNextToken();

		if(getCurrentToken() == ES_TOKEN_TYPE_RESERVED_VARIABLE)
		{
			if(!parseVariable(1, 0))
			{
				return 0;
			}
		}
		else if(getCurrentToken() == ES_TOKEN_TYPE_IDENTIFIER)
		{
			if(!parseAssign(1, 1))
			{
				return 0;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
		{
			return 0;
		}

		addICodeJumpTarget(currentScope, targetIndex0);

		if(getLookAheadChar() != ';')
		{
			if(!parseExpression())
			{
				return 0;
			}
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_SEMICOLON))
		{
			return 0;
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex2);

		addICodeJumpTarget(currentScope, targetIndex1);

		if(getLookAheadChar() != ')')
		{
			do
			{
				getNextToken();

				if(getCurrentToken() == ES_TOKEN_TYPE_IDENTIFIER)
				{
					if(!parseAssign(1))
					{
						return 0;
					}
				}
				/*else if(getCurrentToken() == ES_TOKEN_TYPE_OPERATOR)
				{
					if(getCurrentOperatorIndex() == ES_OPERATOR_TYPE_INCREMENT)
					{
						if(!parseExpression())
						{
							return 0;
						}

						int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
					}
					else if(getCurrentOperatorIndex() == ES_OPERATOR_TYPE_DECREMENT)
					{
						if(!parseExpression())
						{
							return 0;
						}

						int instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
						addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
					}
					else
					{
						throwCodeError("Invalid expression");

						return 0;
					}
				}*/
				else
				{
					if(!parseExpression())
					{
						return 0;
					}

					instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
					addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

					/*throwCodeError("Invalid expression");

					return 0;*/
				}

				getNextToken();
			}
			while(getCurrentToken() == ES_TOKEN_TYPE_DELIMITER_COMMA);

			rewindTokenStream();
		}

		if(!readToken(ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE))
		{
			return 0;
		}

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex0);

		addICodeJumpTarget(currentScope, targetIndex2);

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_POP);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JE);
		addRegisterCodeICodeOperand(currentScope, instructionIndex, ES_REGISTER_CODE_0);
		addIntegerICodeOperand(currentScope, instructionIndex, 0);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex3);

		LoopStackElement *loop = new LoopStackElement();
		loop->type = ES_LOOP_FOR;
		loop->startTargetIndex = targetIndex0;
		loop->endTargetIndex = targetIndex3;

		loopStack.push(loop);

		if(!parseStatement())
		{
			return 0;
		}

		loopStack.removeTopElement();

		instructionIndex = addICodeInstruction(currentScope, ES_OPCODE_JMP);
		addJumpTargetIndexICodeOperand(currentScope, instructionIndex, targetIndex1);

		addICodeJumpTarget(currentScope, targetIndex3);

		return 1;
	}

	bool parseDirective()
	{
		if(currentScope != ES_SCOPE_GLOBAL)
		{
			throwCodeError("Directive inside function scope");

			return 0;
		}

		getNextToken();

		if(getCurrentToken() == ES_TOKEN_TYPE_IDENTIFIER)
		{
			if(_stricmp(getCurrentLexeme(), "SetStackSize") == 0)
			{
				if(!readToken(ES_TOKEN_TYPE_INT))
				{
					return 0;
				}

				scriptHeader.stackSize = atoi(getCurrentLexeme());
			}
			else if(_stricmp(getCurrentLexeme(), "include") == 0)
			{
				if(!readToken(ES_TOKEN_TYPE_STRING))
				{
					return 0;
				}

				if(currentLexerState.currentLineNode) delete(currentLexerState.currentLineNode->data);

				char *emptyLine = new char[1];
				emptyLine[0] = '\0';

				if(currentLexerState.currentLineNode) currentLexerState.currentLineNode->data = emptyLine;

				sourceCode.deleteNode(currentLexerState.currentLineNode);

				string completeAddress = rootAddress + (string)getCurrentLexeme();

				if(!includeSourceFile((char *)completeAddress.c_str(), currentLexerState.currentLineIndex - 1))
				{
					return 0;
				}
			}
			else if(_stricmp(getCurrentLexeme(), "define") == 0)
			{
				if(!readToken(ES_TOKEN_TYPE_IDENTIFIER))
				{
					return 0;
				}

				string identifier = getCurrentLexeme();

				string expressionString = (char *)currentLexerState.currentLineNode->data;
				expressionString = expressionString.substr(currentLexerState.currentLexemeEnd + 1, expressionString.length() - 1);

				if(expressionString.length())
				{
					LinkedListNode *nextLineNode = currentLexerState.currentLineNode->next;
					int directiveLineIndex = currentLexerState.currentLineIndex;

					while(1)
					{
						getNextToken();

						if(getCurrentToken() == ES_TOKEN_TYPE_IDENTIFIER)
						{
							if(strcmp(getCurrentLexeme(), identifier.c_str()) == 0)
							{
								string currentLine = (char *)currentLexerState.currentLineNode->data;

								currentLine.replace(currentLexerState.currentLexemeStart, strlen(currentLexerState.currentLexeme), expressionString);

								int newLineCharacterIndex = currentLexerState.currentLexemeStart + expressionString.length() - 1;
								currentLine.erase(newLineCharacterIndex, 1);

								strcpy((char *)currentLexerState.currentLineNode->data, currentLine.c_str());
							}
						}
						else if(getCurrentToken() == ES_TOKEN_TYPE_END_OF_STREAM)
						{
							break;
						}
					}

					currentLexerState.currentLexemeStart = 0;
					currentLexerState.currentLexemeEnd = 0;
					currentLexerState.currentLineNode = nextLineNode;
					currentLexerState.currentLineIndex = directiveLineIndex + 1;
					currentLexerState.operatorIndex = 0;
					currentLexerState.lexerState = ES_LEXER_STATE_START;
					currentLexerState.currentToken = 0;

					copyLexerState(&previousLexerState, currentLexerState);
				}
			}
			else
			{
				throwCodeError("Invalid directive");

				return 0;
			}
		}
		else
		{
			throwCodeError("Invalid directive");

			return 0;
		}

		return 1;
	}
};
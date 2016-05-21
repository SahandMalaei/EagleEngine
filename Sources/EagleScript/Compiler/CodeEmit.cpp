#include "../EagleScript.h"

#include <time.h>

using namespace EagleScript;

namespace EagleScriptCompiler
{
	FILE *outputFile;

	char mnemonicStringList[][EASM_INSTRUCTION_COUNT] = {"mov", "add", "sub", "mul", "div", "mod", "exp", "neg", "inc", "dec", "and", "or", "xor", "not", "shl", "shr", "concat", "getchar", "setchar", "jmp", "je", "jne", "jg", "jl", "jge", "jle", "push", "pop", "call", "return", "callhost", "pause", "exit", "print", "input", "gethostglobal"};

	void emitEASMCode(char *fileAddress, bool addComments)
	{
		if(!(outputFile = fopen(fileAddress, "wb"))) throwError("Could not create output file " + (string)fileAddress);

		emitEASMHeader(fileAddress, addComments);

		if(addComments) fprintf(outputFile, "; ------- Directives --------------------\n\n");

		emitEASMDirectives(addComments);

		if(addComments) fprintf(outputFile, "; ------- Global Variables --------------\n\n");

		emitEASMScopeVariables(ES_SCOPE_GLOBAL, addComments);

		LinkedListNode *currentNode = functionTable.head;

		FunctionNode *currentFunction, *preloadFunction = 0, *initializeFunction = 0, *updateFunction = 0, *renderFunction = 0, *endFunction = 0;

		if(addComments) fprintf(outputFile, "; ------- Main Functions --------------------------\n\n");

		if(functionTable.nodeCount > 0)
		{
			while(1)
			{
				currentFunction = (FunctionNode *)currentNode->data;

				if(_stricmp(currentFunction->name, ES_PRELOAD_FUNCTION_NAME) == 0)
				{
					preloadFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_INITIALIZE_FUNCTION_NAME) == 0)
				{
					initializeFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_UPDATE_FUNCTION_NAME) == 0)
				{
					updateFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_RENDER_FUNCTION_NAME) == 0)
				{
					renderFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_END_FUNCTION_NAME) == 0)
				{
					endFunction = currentFunction;
				}

				currentNode = currentNode->next;

				if(!currentNode) break;
			}
		}

		if(preloadFunction)
		{
			emitEASMFunction(preloadFunction, addComments);

			fprintf(outputFile, "\n\n");
		}

		if(initializeFunction)
		{
			emitEASMFunction(initializeFunction, addComments);

			fprintf(outputFile, "\n\n");
		}

		if(updateFunction)
		{
			emitEASMFunction(updateFunction, addComments);

			fprintf(outputFile, "\n\n");
		}

		if(renderFunction)
		{
			emitEASMFunction(renderFunction, addComments);

			fprintf(outputFile, "\n\n");
		}

		if(endFunction)
		{
			emitEASMFunction(endFunction, addComments);

			fprintf(outputFile, "\n\n");
		}

		if(addComments) fprintf(outputFile, "; ------- Functions ---------------------");

		currentNode = functionTable.head;

		if(functionTable.nodeCount > 0)
		{
			while(1)
			{
				currentFunction = (FunctionNode *)currentNode->data;

				if(_stricmp(currentFunction->name, ES_PRELOAD_FUNCTION_NAME) == 0)
				{
					//preloadFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_INITIALIZE_FUNCTION_NAME) == 0)
				{
					//initializeFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_UPDATE_FUNCTION_NAME) == 0)
				{
					//updateFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_RENDER_FUNCTION_NAME) == 0)
				{
					//renderFunction = currentFunction;
				}
				else if(_stricmp(currentFunction->name, ES_END_FUNCTION_NAME) == 0)
				{
					//endFunction = currentFunction;
				}
				else
				{
					fprintf(outputFile, "\n\n");

					emitEASMFunction(currentFunction, addComments);
				}

				currentNode = currentNode->next;

				if(!currentNode)
				{
					break;
				}
			}
		}

		fclose(outputFile);
	}

	void emitEASMHeader(char *fileAddress, bool addComments)
	{
		if(!addComments) return;

		time_t currentTimeMilliseconds = time(0);
		tm *currentTime = localtime(&currentTimeMilliseconds);

		string esFileAddress = fileAddress;
		esFileAddress = esFileAddress.substr(0, strlen(fileAddress) - 5);

		fprintf(outputFile, "; %s\n\n", fileAddress);

		fprintf(outputFile, "; Source File : %s\n", esFileAddress.c_str());
		fprintf(outputFile, "; EagleScript Compiler Version %d.%d\n", ES_COMPILER_VERSION_MAJOR, ES_COMPILER_VERSION_MINOR);
		fprintf(outputFile, "; Time : %s\n", asctime(currentTime));
	}

	void emitEASMDirectives(bool addComments)
	{
		if(scriptHeader.stackSize)
		{
			if(addComments) fprintf(outputFile, "\t");
			fprintf(outputFile, "SetStackSize %d\n\n", scriptHeader.stackSize);
		}
	}

	void emitEASMScopeVariables(int scope, bool addComments)
	{
		bool addNewLine = 0;

		SymbolNode *currentSymbol;

		for(int i = 0; i < symbolTable.nodeCount; ++i)
		{
			currentSymbol = getSymbolByIndex(i);

			if(currentSymbol->scope == scope)
			{
				if(currentSymbol->type == ES_SYMBOL_TYPE_VARIABLE)
				{
					if(addComments) fprintf(outputFile, "\t");

					if(scope != ES_SCOPE_GLOBAL) fprintf(outputFile, "\t");

					fprintf(outputFile, "Variable\t%s", currentSymbol->identifier);

					if(currentSymbol->size > 1)
					{
						fprintf(outputFile, "[%d]", currentSymbol->size);
					}

					fprintf(outputFile, "\n");

					addNewLine = 1;
				}
			}
		}

		if(addNewLine) fprintf(outputFile, "\n");
	}

	void emitEASMFunction(FunctionNode *function, bool addComments)
	{
		if(addComments) fprintf(outputFile, "\t");
		fprintf(outputFile, "Function %s(", function->name);

		//function->parameterCount /= 2;

		if(function->parameterCount != 0)
		{
			int currentParameterIndex = 0;

			LinkedListNode *currentNode = symbolTable.head;

			for(int i = 0; i < symbolTable.nodeCount; ++i)
			{
				SymbolNode *currentSymbol = (SymbolNode *)currentNode->data;

				if(currentSymbol->scope == function->index)
				{
					if(currentSymbol->type == ES_SYMBOL_TYPE_PARAMETER)
					{
						fprintf(outputFile, currentSymbol->identifier);

						if(currentParameterIndex < function->parameterCount - 1) fprintf(outputFile, ", ");

						currentParameterIndex++;
					}
				}

				currentNode = currentNode->next;
			}
		}

		fprintf(outputFile, ")\n");
		if(addComments) fprintf(outputFile, "\t");
		fprintf(outputFile, "{\n");

		emitEASMScopeVariables(function->index, addComments);

		if(function->iCodeStream.nodeCount > 0)
		{
			bool isFirstSourceLine = 1;

			for(int i = 0; i < function->iCodeStream.nodeCount; ++i)
			{
				ICodeNode *currentNode = getICodeNodeByImplicitIndex(function->index, i);

				switch(currentNode->type)
				{
				case ES_ICODE_NODE_TYPE_SOURCE_LINE:
					{
						if(!addComments) break;

						char *sourceLineString = currentNode->sourceLineString;

						int lastCharacterIndex = strlen(sourceLineString) - 1;

						if(sourceLineString[lastCharacterIndex] == '\n') sourceLineString[lastCharacterIndex] = '\0';

						if(!isFirstSourceLine) fprintf(outputFile, "\n");

						if(addComments) fprintf(outputFile, "\t");
						fprintf(outputFile, "\t; %s\n\n", sourceLineString);

						break;
					}

				case ES_ICODE_NODE_TYPE_INSTRUCTION:
					{
						if(addComments) fprintf(outputFile, "\t");
						fprintf(outputFile, "\t%s", mnemonicStringList[currentNode->instruction.opcode]);

						int operandCount = currentNode->instruction.operandList.nodeCount;

						if(operandCount > 0)
						{
							fprintf(outputFile, "\t\t");

							if(strlen(mnemonicStringList[currentNode->instruction.opcode]) < ES_CODE_EMIT_TAB_WIDTH) fprintf(outputFile, "\t");
							if(strlen(mnemonicStringList[currentNode->instruction.opcode]) + 5 < ES_CODE_EMIT_TAB_WIDTH) fprintf(outputFile, "\t");

							/*int stringLength = strlen(mnemonicStringList[currentNode->instruction.opcode]);

							for(int i = 0; i < ES_CODE_EMIT_TAB_WIDTH - stringLength; ++i)
							{
							fprintf(outputFile, " ");
							}*/
						}

						for(int i = 0; i < operandCount; ++i)
						{
							ICodeOperand *operand = getICodeOperandByIndex(currentNode, i);

							switch(operand->type)
							{
							case ES_OPERAND_TYPE_INT:
								fprintf(outputFile, "%d", operand->integerLiteral);

								break;

							case ES_OPERAND_TYPE_FLOAT:
								fprintf(outputFile, "%f", operand->floatLiteral);

								break;

							case ES_OPERAND_TYPE_STRING_INDEX:
								fprintf(outputFile, "\"%s\"", stringTable.getStringByIndex(operand->stringIndex));

								break;

							case ES_OPERAND_TYPE_VARIABLE:
								fprintf(outputFile, "%s", getSymbolByIndex(operand->symbolIndex)->identifier);

								break;

								/*case ES_OPERAND_TYPE_ARRAY_ABSOLUTE_INDEX:
								fprintf(outputFile, "%s[%d]", getSymbolByIndex(operand->symbolIndex)->identifier, operand->offset);

								break;

								case ES_OPERAND_TYPE_ARRAY_VARIABLE_INDEX:
								fprintf(outputFile, "%s[%s]", getSymbolByIndex(operand->symbolIndex)->identifier, getSymbolByIndex(operand->offsetSymbolIndex)->identifier);

								break;*/

							case ES_OPERAND_TYPE_ARRAY:
								if(operand->offsetSymbolIndex == -1)
								{
									fprintf(outputFile, "%s[%d]", getSymbolByIndex(operand->symbolIndex)->identifier, operand->offset);
								}
								else if(operand->offset == 0)
								{
									if(operand->offset == ES_REGISTER_CODE_0) fprintf(outputFile, "%s[%s]", getSymbolByIndex(operand->symbolIndex)->identifier, "Register0");
									else fprintf(outputFile, "%s[%s]", getSymbolByIndex(operand->symbolIndex)->identifier, "Register1");
								}
								else
								{
									fprintf(outputFile, "%s[%d]", getSymbolByIndex(operand->symbolIndex)->identifier, operand->offsetSymbolIndex);
								}

								break;

							case ES_OPERAND_TYPE_FUNCTION_INDEX:
								{
									if(operand->functionIndex > -1)
									{
										FunctionNode *function = getFunctionByIndex(operand->functionIndex);

										fprintf(outputFile, "%s", function->name);
									}
									else
									{
										int hostFunctionIndex = (operand->functionIndex * -1) - 1;
										fprintf(outputFile, "%s", (char *)getHostFunctionIdentifier(hostFunctionIndex).c_str());
									}

									break;
								}

							case ES_OPERAND_TYPE_REGISTER:
								if(operand->registerCode == ES_REGISTER_CODE_RETURN_VALUE)
								{
									fprintf(outputFile, "%s", "ReturnValue");
								}
								else if(operand->registerCode == ES_REGISTER_CODE_HOST_RETURN_VALUE)
								{
									fprintf(outputFile, "%s", "HostReturnValue");
								}
								else if(operand->registerCode == ES_REGISTER_CODE_0)
								{
									fprintf(outputFile, "%s", "Register0");
								}
								else if(operand->registerCode == ES_REGISTER_CODE_1)
								{
									fprintf(outputFile, "%s", "Register1");
								}
								else if(operand->registerCode == ES_REGISTER_CODE_2)
								{
									fprintf(outputFile, "%s", "Register2");
								}

								break;

							case ES_OPERAND_TYPE_JUMP_TARGET_INDEX:
								fprintf(outputFile, "%s%d", ES_JUMP_TARGET_PREFIX, operand->jumpTargetIndex);

								break;
							}

							if(i != operandCount - 1) fprintf(outputFile, ", ");
						}

						fprintf(outputFile, "\n");

						break;
					}

				case ES_ICODE_NODE_TYPE_JUMP_TARGET:
					{
						fprintf(outputFile, "%s%d:\n", ES_JUMP_TARGET_PREFIX, currentNode->jumpTargetIndex);

						break;
					}
				}

				if(isFirstSourceLine) isFirstSourceLine = 0;
			}
		}
		else
		{
			if(addComments) fprintf(outputFile, "\t\t; (No code)\n");
		}

		if(addComments) fprintf(outputFile, "\t");
		fprintf(outputFile, "}");
	}

	void optimizeICode()
	{
		LinkedListNode *currentFunctionNode = functionTable.head;

		while(currentFunctionNode)
		{
			FunctionNode *function = (FunctionNode *)currentFunctionNode->data;

			if(function->iCodeStream.nodeCount > 0)
			{
				int passedPushCount = 0;

				LinkedListNode *nodeIterator0 = function->iCodeStream.head;
				LinkedListNode *previousNode = 0;

				/*for(int i = 0; i < function->iCodeStream.nodeCount - 2; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;

					if(!nodeIterator0->next->next)
					{
						break;
					}

					ICodeNode *node2 = (ICodeNode *)nodeIterator0->next->next->data;

					if(node0 && node1 && node2)
					{
						if(node0->instruction.opcode == ES_OPCODE_PUSH &&
							node1->instruction.opcode == ES_OPCODE_POP &&
							node2->instruction.opcode == ES_OPCODE_MOV)
						{
							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;
							ICodeOperand *operand20 = (ICodeOperand *)node2->instruction.operandList.head->data;
							ICodeOperand *operand21 = (ICodeOperand *)node2->instruction.operandList.head->next->data;

							if(operand10->type == ES_OPERAND_TYPE_REGISTER && operand10->registerCode == ES_REGISTER_CODE_0)
							{
								if(operand21->type == ES_OPERAND_TYPE_REGISTER && operand21->registerCode == ES_REGISTER_CODE_0)
								{
									int instructionIndex = insertICodeInstruction(function->index, i + 2, ES_OPCODE_MOV);
									addICodeOperand(function->index, instructionIndex, *operand20);
									addICodeOperand(function->index, instructionIndex, *operand00);

									function->deleteInstruction(i);
									function->deleteInstruction(i);
									function->deleteInstruction(i);

									if(!previousNode)
									{
										nodeIterator0 = function->iCodeStream.head;
									}
									else
									{
										nodeIterator0 = previousNode;
									}

									i--;
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}*/

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 2; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;
					ICodeNode *node2 = (ICodeNode *)nodeIterator0->next->next->data;

					if(node0 && node1 && node2)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV &&
							node1->instruction.opcode == ES_OPCODE_MOV &&
							(node2->instruction.opcode > ES_OPCODE_JE - 1 &&
							node2->instruction.opcode < ES_OPCODE_JLE + 1))
						{
							if(!node0->instruction.operandList.head) continue;
							if(!node1->instruction.operandList.head) continue;
							if(!node2->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;
							ICodeOperand *operand11 = (ICodeOperand *)node1->instruction.operandList.head->next->data;
							ICodeOperand *operand20 = (ICodeOperand *)node2->instruction.operandList.head->data;
							ICodeOperand *operand21 = (ICodeOperand *)node2->instruction.operandList.head->next->data;
							ICodeOperand *operand22 = (ICodeOperand *)node2->instruction.operandList.head->next->next->data;

							if(operand00->type == ES_OPERAND_TYPE_REGISTER && operand00->registerCode == ES_REGISTER_CODE_1)
							{
								if(operand10->type == ES_OPERAND_TYPE_REGISTER && operand10->registerCode == ES_REGISTER_CODE_0)
								{
									if(operand20->type == ES_OPERAND_TYPE_REGISTER && operand20->registerCode == ES_REGISTER_CODE_0)
									{
										if(operand21->type == ES_OPERAND_TYPE_REGISTER && operand21->registerCode == ES_REGISTER_CODE_1)
										{
											int instructionIndex = insertICodeInstruction(function->index, i + 2, node2->instruction.opcode);
											addICodeOperand(function->index, instructionIndex, *operand11);
											addICodeOperand(function->index, instructionIndex, *operand01);
											addICodeOperand(function->index, instructionIndex, *operand22);

											function->deleteInstruction(i);
											function->deleteInstruction(i);
											function->deleteInstruction(i);

											if(!previousNode)
											{
												nodeIterator0 = function->iCodeStream.head;
											}
											else
											{
												nodeIterator0 = previousNode;
											}

											i--;
										}
									}
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}

				/*nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 2; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;
					ICodeNode *node2 = (ICodeNode *)nodeIterator0->next->next->data;

					if(node0 && node1 && node2)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV &&
							node1->instruction.opcode == ES_OPCODE_MOV &&
							(node2->instruction.opcode > ES_OPCODE_ADD - 1 &&
							node2->instruction.opcode < ES_OPCODE_XOR + 1 &&
							node1->instruction.opcode != ES_OPCODE_INC &&
							node1->instruction.opcode != ES_OPCODE_DEC &&
							node1->instruction.opcode != ES_OPCODE_NEG))
						{
							if(!node0->instruction.operandList.head) continue;
							if(!node1->instruction.operandList.head) continue;
							if(!node2->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;
							ICodeOperand *operand11 = (ICodeOperand *)node1->instruction.operandList.head->next->data;
							ICodeOperand *operand20 = (ICodeOperand *)node2->instruction.operandList.head->data;
							ICodeOperand *operand21 = (ICodeOperand *)node2->instruction.operandList.head->next->data;

							if(operand00->type == ES_OPERAND_TYPE_REGISTER && operand00->registerCode == ES_REGISTER_CODE_1)
							{
								if(operand10->type == ES_OPERAND_TYPE_REGISTER && operand10->registerCode == ES_REGISTER_CODE_0)
								{
									if(operand20->type == ES_OPERAND_TYPE_REGISTER && operand20->registerCode == ES_REGISTER_CODE_0)
									{
										if(operand21->type == ES_OPERAND_TYPE_REGISTER && operand21->registerCode == ES_REGISTER_CODE_1)
										{
											int instructionIndex = insertICodeInstruction(function->index, i + 2, node2->instruction.opcode);
											addICodeOperand(function->index, instructionIndex, *operand10);
											addICodeOperand(function->index, instructionIndex, *operand01);

											function->deleteInstruction(i);
											function->deleteInstruction(i + 1);

											if(!previousNode)
											{
												nodeIterator0 = function->iCodeStream.head;
											}
											else
											{
												nodeIterator0 = previousNode;
											}

											i--;
										}
									}
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 2; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;
					ICodeNode *node2 = (ICodeNode *)nodeIterator0->next->next->data;

					if(node0 && node1)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV &&
							(node1->instruction.opcode > ES_OPCODE_ADD - 1 &&
							node1->instruction.opcode < ES_OPCODE_XOR + 1 &&
							node1->instruction.opcode != ES_OPCODE_INC &&
							node1->instruction.opcode != ES_OPCODE_DEC &&
							node1->instruction.opcode != ES_OPCODE_NEG))
						{
							if(!node0->instruction.operandList.head) continue;
							if(!node1->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;
							ICodeOperand *operand11 = (ICodeOperand *)node1->instruction.operandList.head->next->data;

							if(operand00->type == ES_OPERAND_TYPE_REGISTER)
							{
								if(operand11->type == ES_OPERAND_TYPE_REGISTER)
								{
									if(operand00->registerCode == operand11->registerCode)
									{
										int instructionIndex = insertICodeInstruction(function->index, i + 1, node1->instruction.opcode);
										addICodeOperand(function->index, instructionIndex, *operand10);
										addICodeOperand(function->index, instructionIndex, *operand01);

										function->deleteInstruction(i);
										function->deleteInstruction(i);

										if(!previousNode)
										{
											nodeIterator0 = function->iCodeStream.head;
										}
										else
										{
											nodeIterator0 = previousNode;
										}

										i--;
									}
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}*/

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;

					if(node0)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV)
						{
							if(!node0->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;

							if(operand00->type == ES_OPERAND_TYPE_REGISTER && operand01->type == ES_OPERAND_TYPE_REGISTER &&
								operand00->registerCode == operand01->registerCode)
							{
								function->deleteInstruction(i);

								if(!previousNode)
								{
									nodeIterator0 = function->iCodeStream.head;
								}
								else
								{
									nodeIterator0 = previousNode;
								}

								i--;
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 1; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;

					if(node0 && node1)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV &&
							(node1->instruction.opcode > ES_OPCODE_ADD - 1 &&
							node1->instruction.opcode < ES_OPCODE_XOR + 1 &&
							node1->instruction.opcode != ES_OPCODE_INC &&
							node1->instruction.opcode != ES_OPCODE_DEC &&
							node1->instruction.opcode != ES_OPCODE_NEG))
						{
							if(!node0->instruction.operandList.head) continue;
							if(!node1->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;
							ICodeOperand *operand11 = (ICodeOperand *)node1->instruction.operandList.head->next->data;

							if(operand00->type == ES_OPERAND_TYPE_REGISTER)
							{
								if(operand10->type == ES_OPERAND_TYPE_REGISTER)
								{
									if(operand00->registerCode == operand10->registerCode)
									{
										if(operand01->type == ES_OPERAND_TYPE_INT || operand01->type == ES_OPERAND_TYPE_FLOAT)
										{
											if(operand11->type == ES_OPERAND_TYPE_INT || operand11->type == ES_OPERAND_TYPE_FLOAT)
											{
												ICodeOperand newOperand;

												switch(node1->instruction.opcode)
												{
												case ES_OPCODE_ADD:
													newOperand = precomputeAdd(*operand01, *operand11);

													break;

												case ES_OPCODE_SUB:
													newOperand = precomputeSubtract(*operand01, *operand11);

													break;

												case ES_OPCODE_MUL:
													newOperand = precomputeMultiply(*operand01, *operand11);

													break;

												case ES_OPCODE_DIV:
													newOperand = precomputeDivide(*operand01, *operand11);

													break;

												case ES_OPCODE_EXP:
													newOperand = precomputeExponent(*operand01, *operand11);

													break;

												default:
													continue;

													break;
												}


												int instructionIndex = insertICodeInstruction(function->index, i + 1, ES_OPCODE_MOV);
												addICodeOperand(function->index, instructionIndex, *operand10);
												addICodeOperand(function->index, instructionIndex, newOperand);

												function->deleteInstruction(i);
												function->deleteInstruction(i);

												if(!previousNode)
												{
													nodeIterator0 = function->iCodeStream.head;
												}
												else
												{
													nodeIterator0 = previousNode;
												}

												i--;
											}
										}
									}
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 1; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;

					if(node0 && node1)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV &&
							node1->instruction.opcode == ES_OPCODE_MOV)
						{
							if(!node0->instruction.operandList.head) continue;
							if(!node1->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;
							ICodeOperand *operand11 = (ICodeOperand *)node1->instruction.operandList.head->next->data;

							if(operand00->type == ES_OPERAND_TYPE_REGISTER && operand00->registerCode == ES_REGISTER_CODE_0)
							{
								if(operand11->type == ES_OPERAND_TYPE_REGISTER && operand11->registerCode == ES_REGISTER_CODE_0)
								{
									int instructionIndex = insertICodeInstruction(function->index, i + 1, ES_OPCODE_MOV);
									addICodeOperand(function->index, instructionIndex, *operand10);
									addICodeOperand(function->index, instructionIndex, *operand01);

									function->deleteInstruction(i);
									function->deleteInstruction(i);

									if(!previousNode)
									{
										nodeIterator0 = function->iCodeStream.head;
									}
									else
									{
										nodeIterator0 = previousNode;
									}

									i--;
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 1; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;

					if(node0 && node1)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV &&
							node1->instruction.opcode == ES_OPCODE_PUSH)
						{
							if(!node0->instruction.operandList.head) continue;
							if(!node1->instruction.operandList.head) continue;

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;
							ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;

							if(*operand00 == *operand10)
							{
								int instructionIndex = insertICodeInstruction(function->index, i + 1, ES_OPCODE_PUSH);
								addICodeOperand(function->index, instructionIndex, *operand01);

								function->deleteInstruction(i);
								function->deleteInstruction(i);

								if(!previousNode)
								{
									nodeIterator0 = function->iCodeStream.head;
								}
								else
								{
									nodeIterator0 = previousNode;
								}

								i--;
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				bool resetIterator = 0;
				for(int i = 0; i < function->iCodeStream.nodeCount - 1; ++i)
				{
					resetIterator = 0;
					passedPushCount = 0;

					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;

					if(node0)
					{
						if(node0->instruction.opcode == ES_OPCODE_PUSH)
						{
							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;

							bool isOptimizationSafe;

							bool isReturnValue = 0;
							bool isHostReturnValue = 0;

							if(operand00->type == ES_OPERAND_TYPE_INT || operand00->type == ES_OPERAND_TYPE_FLOAT || operand00->type == ES_OPERAND_TYPE_STRING_INDEX)
							{
								isOptimizationSafe = 1;
							}
							else
							{
								isOptimizationSafe = 0;

								if(operand00->type == ES_OPERAND_TYPE_REGISTER && operand00->registerCode == ES_REGISTER_CODE_RETURN_VALUE)
								{
									isReturnValue = 1;
								}
								else if(operand00->type == ES_OPERAND_TYPE_REGISTER && operand00->registerCode == ES_REGISTER_CODE_HOST_RETURN_VALUE)
								{
									isHostReturnValue = 1;
								}
							}

							LinkedListNode *nodeIterator1 = nodeIterator0->next;

							for(int j = i + 1; j < function->iCodeStream.nodeCount; ++j)
							{
								ICodeNode *node1 = (ICodeNode *)nodeIterator1->data;

								nodeIterator1 = nodeIterator1->next;

								if(!node1)
								{
									break;
								}

								if(!isOptimizationSafe)
								{
									if(node1->instruction.opcode > ES_OPCODE_MOV - 1 && node1->instruction.opcode < ES_OPCODE_SETCHAR + 1)
									{
										ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;

										if(*operand10 == *operand00)
										{
											break;
										}
									}
									else if(node1->instruction.opcode == ES_OPCODE_CALL)
									{
										if(isReturnValue)
										{
											break;
										}
									}
									else if(node1->instruction.opcode == ES_OPCODE_CALLHOST)
									{
										if(isHostReturnValue)
										{
											break;
										}
									}
								}

								if(node1->type == ES_ICODE_NODE_TYPE_JUMP_TARGET)
								{
									break;
								}

								if(node1->instruction.opcode > ES_OPCODE_JMP - 1 && node1->instruction.opcode < ES_OPCODE_JLE + 1)
								{
									break;
								}

								if(node1->instruction.opcode == ES_OPCODE_PUSH)
								{
									passedPushCount++;

									continue;
								}

								if(node1->instruction.opcode == ES_OPCODE_POP)
								{
									if(passedPushCount)
									{
										passedPushCount--;

										continue;
									}

									ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.head->data;

									int instructionIndex = insertICodeInstruction(function->index, j, ES_OPCODE_MOV);
									addICodeOperand(function->index, instructionIndex, *operand10);
									addICodeOperand(function->index, instructionIndex, *operand00);

									function->deleteInstruction(i);
									function->deleteInstruction(j - 1);

									nodeIterator0 = function->iCodeStream.head;
									previousNode = 0;
									i = -1;

									resetIterator = 1;

									break;
								}
							}
						}
					}

					if(!resetIterator)
					{
						previousNode = nodeIterator0;

						nodeIterator0 = nodeIterator0->next;
					}
				}

				nodeIterator0 = function->iCodeStream.head;
				previousNode = 0;

				for(int i = 0; i < function->iCodeStream.nodeCount - 1; ++i)
				{
					ICodeNode *node0 = (ICodeNode *)nodeIterator0->data;
					ICodeNode *node1 = (ICodeNode *)nodeIterator0->next->data;

					if(node0 && node1)
					{
						if(node0->instruction.opcode == ES_OPCODE_MOV)
						{
							if(!node0->instruction.operandList.head)
							{
								previousNode = nodeIterator0;

								nodeIterator0 = nodeIterator0->next;

								continue;
							}

							if(!node1->instruction.operandList.head)
							{
								previousNode = nodeIterator0;

								nodeIterator0 = nodeIterator0->next;

								continue;
							}

							ICodeOperand *operand00 = (ICodeOperand *)node0->instruction.operandList.head->data;
							ICodeOperand *operand01 = (ICodeOperand *)node0->instruction.operandList.head->next->data;

							bool change = 0;

							for(int j = 1; j < node1->instruction.operandList.nodeCount; ++j)
							{
								ICodeOperand *operand10 = (ICodeOperand *)node1->instruction.operandList.findNode(j)->data;

								if(*operand00 == *operand10)
								{
									*operand10 = *operand01;

									change = 1;
								}
							}

							if(change)
							{
								if(operand00->symbolIndex == functionParameterListIndex || operand00->type == ES_OPERAND_TYPE_REGISTER)
								{
									function->deleteInstruction(i);

									if(!previousNode)
									{
										nodeIterator0 = function->iCodeStream.head;
									}
									else
									{
										nodeIterator0 = previousNode;
									}

									nodeIterator0 = function->iCodeStream.head;
									previousNode = 0;

									i = -1;

									continue;
								}
							}
						}
					}

					previousNode = nodeIterator0;

					nodeIterator0 = nodeIterator0->next;
				}
			}

			currentFunctionNode = currentFunctionNode->next;
		}
	}

	ICodeOperand precomputeAdd(ICodeOperand operand0, ICodeOperand operand1)
	{
		ICodeOperand operand;

		operand.type = ES_OPERAND_TYPE_NULL;
		operand.integerLiteral = 0;

		if(operand0.type == ES_OPERAND_TYPE_FLOAT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.floatLiteral;
		}
		else if(operand0.type == ES_OPERAND_TYPE_INT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.integerLiteral;
		}

		if(operand1.type == ES_OPERAND_TYPE_FLOAT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral += operand1.floatLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.floatLiteral;
			}*/
		}
		else if(operand1.type == ES_OPERAND_TYPE_INT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral += operand1.integerLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.integerLiteral;
			}*/
		}

		return operand;
	}

	ICodeOperand precomputeSubtract(ICodeOperand operand0, ICodeOperand operand1)
	{
		ICodeOperand operand;

		operand.type = ES_OPERAND_TYPE_NULL;
		operand.integerLiteral = 0;

		if(operand0.type == ES_OPERAND_TYPE_FLOAT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.floatLiteral;
		}
		else if(operand0.type == ES_OPERAND_TYPE_INT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.integerLiteral;
		}

		if(operand1.type == ES_OPERAND_TYPE_FLOAT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral -= operand1.floatLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.floatLiteral;
			}*/
		}
		else if(operand1.type == ES_OPERAND_TYPE_INT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral -= operand1.integerLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.integerLiteral;
			}*/
		}

		return operand;
	}

	ICodeOperand precomputeMultiply(ICodeOperand operand0, ICodeOperand operand1)
	{
		ICodeOperand operand;

		operand.type = ES_OPERAND_TYPE_NULL;
		operand.integerLiteral = 0;

		if(operand0.type == ES_OPERAND_TYPE_FLOAT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.floatLiteral;
		}
		else if(operand0.type == ES_OPERAND_TYPE_INT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.integerLiteral;
		}

		if(operand1.type == ES_OPERAND_TYPE_FLOAT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral *= operand1.floatLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.floatLiteral;
			}*/
		}
		else if(operand1.type == ES_OPERAND_TYPE_INT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral *= operand1.integerLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.integerLiteral;
			}*/
		}

		return operand;
	}

	ICodeOperand precomputeDivide(ICodeOperand operand0, ICodeOperand operand1)
	{
		ICodeOperand operand;

		operand.type = ES_OPERAND_TYPE_NULL;
		operand.integerLiteral = 0;

		if(operand0.type == ES_OPERAND_TYPE_FLOAT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.floatLiteral;
		}
		else if(operand0.type == ES_OPERAND_TYPE_INT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.integerLiteral;
		}

		if(operand1.type == ES_OPERAND_TYPE_FLOAT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral /= operand1.floatLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.floatLiteral;
			}*/
		}
		else if(operand1.type == ES_OPERAND_TYPE_INT)
		{
			//if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral /= operand1.integerLiteral;
			}
			/*else
			{
			operand.integerLiteral += operand1.integerLiteral;
			}*/
		}

		return operand;
	}

	float power(float number, int power)
	{
		float c = 1;

		for(int i = 0; i < power; ++i)
		{
			c *= number;
		}

		return c;
	}

	ICodeOperand precomputeExponent(ICodeOperand operand0, ICodeOperand operand1)
	{
		ICodeOperand operand;

		operand.type = ES_OPERAND_TYPE_NULL;
		operand.integerLiteral = 0;

		if(operand0.type == ES_OPERAND_TYPE_FLOAT)
		{
			operand.type = ES_OPERAND_TYPE_FLOAT;
			operand.floatLiteral = operand0.floatLiteral;
		}
		else if(operand0.type == ES_OPERAND_TYPE_INT)
		{
			operand.type = ES_OPERAND_TYPE_INT;
			operand.integerLiteral = operand0.integerLiteral;
		}

		if(operand1.type == ES_OPERAND_TYPE_FLOAT)
		{
			if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral = power(operand.floatLiteral, operand1.floatLiteral);
			}
			else
			{
				operand.integerLiteral = power(operand.integerLiteral, operand1.floatLiteral);
			}
		}
		else if(operand1.type == ES_OPERAND_TYPE_INT)
		{
			if(operand.type == ES_OPERAND_TYPE_FLOAT)
			{
				operand.floatLiteral = power(operand.floatLiteral, operand1.integerLiteral);
			}
			else
			{
				operand.integerLiteral = power(operand.integerLiteral, operand1.integerLiteral);
			}
		}

		return operand;
	}
};
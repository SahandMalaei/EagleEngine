#include "EagleScript.h"

#include <unordered_map>

using namespace EagleScript;

//#define copyOperandValue(a, b) memcpy(a, b, sizeof(RuntimeValue))
#define RESOLVE_STACK_INDEX(index) (index < 0 ? (index + m_stack.frameIndex) : (index))
#define GET_STACK_VALUE(index) &m_stack.elementList[RESOLVE_STACK_INDEX(index)]
#define RESOLVE_OPERAND_AS_INT(operandIndex) coerceValueToInteger(resolveOperandValue(operandIndex))
#define RESOLVE_OPERAND_AS_FLOAT(operandIndex) coerceValueToFloat(resolveOperandValue(operandIndex))
#define RESOLVE_OPERAND_AS_STRING(operandIndex) coerceValueToString(resolveOperandValue(operandIndex))
#define RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(operandIndex) resolveOperandValue(operandIndex)->instructionIndex

#ifndef PLATFORM_WP8
	const int EAGLE_DEFAULT_FRAME_RATE = 60;
#else
	const int EAGLE_DEFAULT_FRAME_RATE = 60;
#endif

#define EAGLE_DEFAULT_FRAME_TIME (1.0 / EAGLE_DEFAULT_FRAME_RATE)

namespace EagleVirtualMachine
{
	EagleScriptVirtualMachine virtualMachine;

	// Temp

	Instruction *currentInstruction;
	std::vector<RuntimeValue> *currentInstructionOperandList;

	RuntimeValue parameterList[ES_MAX_FUNCTION_PARAMETER_COUNT];

	RuntimeValue::RuntimeValue()
	{
		type = EASM_OPERAND_TYPE_NULL;

		//stringLiteral = "NULL";
		offsetIndex = 0;

		precomputedAddress = 0;
	}

	Script::Script()
	{
		isInitialized = 0;

		m_globalVariableCount = 0;

		m_currentFunctionTopIndex = 0;
	}

	void Script::initialize()
	{
		virtualMachine.addScript((Script *)this);
	}

	Script::~Script()
	{
		virtualMachine.removeScript(this);

		cleanup();
	}

	bool Script::loadEagleScript(char *fileAddress, bool optimize, bool outputEASMAndEVEFiles, bool addComments)
	{
		if(isInitialized)
		{
			cleanup();
		}

		initialize();

		std::string outputAddress = (std::string)fileAddress + ".easm";

		if(!EagleScriptCompiler::compileESFile(this, fileAddress, (char *)outputAddress.c_str(), optimize, outputEASMAndEVEFiles, addComments))
		{
			cleanup();

			return 0;
		}

		return 1;
	}

	bool Script::loadEagleScriptFromString(std::string scriptText, bool optimize)
	{
		if(isInitialized)
		{
			cleanup();
		}

		initialize();

		if(!EagleScriptCompiler::compileESString(this, scriptText, optimize))
		{
			cleanup();

			return 0;
		}

		return 1;
	}

	void Script::loadEASM(char *fileAddress, bool outputEVEFile)
	{
		initialize();

		if(isInitialized) cleanup();

		EagleAssembler::loadEASMFile(fileAddress);
		EagleAssembler::initializeParser();
		EagleAssembler::parseSourceCode();

		char outputAddress[256];
		strcpy(outputAddress, fileAddress);
		strcat(outputAddress, ".eve");

		if(outputEVEFile)
		{
			EagleAssembler::outputAssembledEVE(outputAddress);
		}

		loadAssemblerData();

		EagleAssembler::cleanup();
	}

	void Script::loadEVE(char *fileAddress)
	{
		initialize();

		if(isInitialized)
		{
			cleanup();
		}

		FILE *file;
		if(!(file = fopen(fileAddress, "rb")))
		{
			throwError("Could not load EVE file");
		}

		// Script header

		char idString[sizeof(EASM_ID_STRING)];
		fread(idString, strlen(EASM_ID_STRING), 1, file);
		idString[strlen(EASM_ID_STRING)] = '\0';
		if(strcmp(idString, EVE_ID_STRING) != 0) throwError("Invalid EVE file");

		int versionMajor = 0, versionMinor = 0;
		fread(&versionMajor, 1, 1, file);
		fread(&versionMinor, 1, 1, file);

		if(versionMajor != 0 || versionMinor != 1) throwError("Unsupported EVE version");

		fread(&m_stack.size, 4, 1, file);
		if(m_stack.size == 0)
		{
			m_stack.size = DEFAULT_STACK_SIZE;
		}

		m_stack.elementList.reserve(m_stack.size);

		for(int i = 0; i < m_stack.size; ++i)
		{
			RuntimeValue element;

			m_stack.elementList.push_back(element);
		}

		fread(&m_globalDataSize, 4, 1, file);

		fread(&m_isInitializeFunctionPresent, 1, 1, file);
		fread(&m_initializeFunctionIndex, 4, 1, file);

		fread(&m_isUpdateFunctionPresent, 1, 1, file);
		fread(&m_updateFunctionIndex, 4, 1, file);

		fread(&m_isRenderFunctionPresent, 1, 1, file);
		fread(&m_renderFunctionIndex, 4, 1, file);

		fread(&m_isEndFunctionPresent, 1, 1, file);
		fread(&m_endFunctionIndex, 4, 1, file);

		// Instruction stream

		fread(&m_instructionStream.instructionCount, 4, 1, file);

		m_instructionStream.instructionList.reserve(m_instructionStream.instructionCount);

		for(int i = 0; i < m_instructionStream.instructionCount; ++i)
		{
			Instruction instruction;

			m_instructionStream.instructionList.push_back(instruction);
		}

		for(int i = 0; i < m_instructionStream.instructionCount; ++i)
		{
			m_instructionStream.instructionList[i].opcode = 0;
			fread(&m_instructionStream.instructionList[i].opcode, 2, 1, file);

			m_instructionStream.instructionList[i].operandCount = 0;
			fread(&m_instructionStream.instructionList[i].operandCount, 1, 1, file);

			for(int j = 0; j < m_instructionStream.instructionList[i].operandCount; ++j)
			{
				RuntimeValue operandValue;

				operandValue.type = 0;
				fread(&operandValue.type, 1, 1, file);

				switch(operandValue.type)
				{
				case EASM_OPERAND_TYPE_INT:
					fread(&operandValue.integerLiteral, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					fread(&operandValue.floatLiteral, sizeof(float), 1, file);

					break;

				case EASM_OPERAND_TYPE_STRING:
					fread(&operandValue.stringTableIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_INSTRUCTION_INDEX:
					fread(&operandValue.instructionIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX:
					fread(&operandValue.stackIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_RELATIVE_STACK_INDEX:
					fread(&operandValue.stackIndex, sizeof(int), 1, file);
					fread(&operandValue.offsetIndex, sizeof(int), 1, file);

					operandValue.type = EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX;

					break;

				case EASM_OPERAND_TYPE_REGISTER_STACK_INDEX:
					fread(&operandValue.stackIndex, sizeof(int), 1, file);
					fread(&operandValue.offsetIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_FUNCTION_INDEX:
					fread(&operandValue.functionIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_HOST_FUNCTION_INDEX:
					fread(&operandValue.hostFunctionIndex, sizeof(int), 1, file);

					break;

				case EASM_OPERAND_TYPE_REGISTER:
					fread(&operandValue.registerCode, sizeof(int), 1, file);

					switch(operandValue.registerCode)
					{
					case ES_REGISTER_CODE_RETURN_VALUE:
						operandValue.precomputedAddress = (int)&m_returnValue;

						break;

					case ES_REGISTER_CODE_HOST_RETURN_VALUE:
						operandValue.precomputedAddress = (int)&m_returnValueFromHost;

						break;

					case ES_REGISTER_CODE_0:
						operandValue.precomputedAddress = (int)&m_register0;

						break;

					case ES_REGISTER_CODE_1:
						operandValue.precomputedAddress = (int)&m_register1;

						break;

					case ES_REGISTER_CODE_2:
						operandValue.precomputedAddress = (int)&m_register2;

						break;
					}

					break;
				}

				m_instructionStream.instructionList[i].operandList.push_back(operandValue);
			}
		}

		// String table

		int stringCount = 0;
		fread(&stringCount, sizeof(int), 1, file);

		char **stringTable = new char *[stringCount];

		for(int i = 0; i < stringCount; ++i)
		{
			int currentStringLength = 0;
			fread(&currentStringLength, sizeof(int), 1, file);

			stringTable[i] = new char[currentStringLength + 1];
			fread(stringTable[i], currentStringLength, 1, file);
			stringTable[i][currentStringLength] = '\0';
		}

		// Instruction stream

		for(int i = 0; i < m_instructionStream.instructionCount; ++i)
		{
			std::vector<RuntimeValue>::iterator listIterator;
			for(listIterator = m_instructionStream.instructionList[i].operandList.begin(); listIterator != m_instructionStream.instructionList[i].operandList.end(); ++listIterator)
			{
				if((*listIterator).type == EASM_OPERAND_TYPE_STRING)
				{
					(*listIterator).stringLiteral = stringTable[(*listIterator).stringTableIndex];
				}
			}
		}

		for(int i = 0; i < stringCount; ++i)
		{
			delete[](stringTable[i]);
		}

		delete[](stringTable);

		// Function table

		m_functionCount = 0;
		fread(&m_functionCount, sizeof(int), 1, file);

		for(int i = 0; i < m_functionCount; ++i)
		{
			Function function;

			m_functionTable.push_back(function);
		}

		for(int i = 0; i < m_functionCount; ++i)
		{
			int entryPoint;
			fread(&entryPoint, sizeof(int), 1, file);

			int parameterCount;
			fread(&parameterCount, sizeof(int), 1, file);

			int localDataSize;
			fread(&localDataSize, sizeof(int), 1, file);

			int stackFrameSize = parameterCount + localDataSize + 1;

			m_functionTable[i].entryPoint = entryPoint;
			m_functionTable[i].parameterCount = parameterCount;
			m_functionTable[i].localDataSize = localDataSize;
			m_functionTable[i].stackFrameSize = stackFrameSize;

			int nameLength;
			fread(&nameLength, sizeof(int), 1, file);
			m_functionTable[i].name.resize(nameLength);
			fread((char *)m_functionTable[i].name.c_str(), nameLength, 1, file);
			m_functionTable[i].name += "\0";
		}

		// Global variables

		/*fread(&m_globalVariableCount, sizeof(short), 1, file);
		m_globalVariableList.reserve(m_globalVariableCount);

		for(int i = 0; i < m_globalVariableCount; ++i)
		{
			EagleAssembler::GlobalVariable globalVariable;

			m_globalVariableList.push_back(globalVariable);
		}

		std::vector<EagleAssembler::GlobalVariable>::iterator listIterator;
		for(listIterator = m_globalVariableList.begin(); listIterator != m_globalVariableList.end(); ++listIterator)
		{
			short identifierLength;
			fread(&identifierLength, sizeof(short), 1, file);
			fread((char *)(*listIterator).identifier.c_str(), identifierLength, 1, file);
			(*listIterator).identifier[identifierLength] = '\0';
			fread(&(*listIterator).stackIndex, sizeof(int), 1, file);
		}*/

		fclose(file);

		while(m_globalDataSize > m_stack.size - ES_STACK_REALLOCATION_SIZE)
		{
			m_stack.size += ES_STACK_REALLOCATION_SIZE;

			m_stack.elementList.reserve(m_stack.size);
			for(int i = 0; i < ES_STACK_REALLOCATION_SIZE; ++i)
			{
				RuntimeValue runtimeValue;
				
				m_stack.elementList.push_back(runtimeValue);
			}
		}

		isInitialized = 1;

		reset();
	}

	void decryptStringTable(unsigned long securityCode, char **stringTable, int stringCount)
	{
		unsigned long stringTableSecurityCode = 0;

		for(int i = 0; i < stringCount; ++i)
		{
			char *currentString = (char *)stringTable[i];
			int currentStringLength = strlen(currentString);

			for(int j = 0; j < currentStringLength; ++j)
			{
				currentString[j]--;
				stringTableSecurityCode += currentString[j];
			}
		}

		stringTableSecurityCode /= 7.77;

		if(stringTableSecurityCode != securityCode) throwError("Corrupted EVE file");
	}

	// Structure interfaces

	int Script::getOperandType(int operandIndex)
	{
		return currentInstruction->operandList[operandIndex].type;
	}

	inline RuntimeValue *Script::resolveOperandValue(int operandIndex)
	{
		RuntimeValue *operand = &(*currentInstructionOperandList)[operandIndex];
		int type = operand->type;

		switch(type)
		{
		case EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX:
			{
				int absoluteIndex = operand->stackIndex + operand->offsetIndex;
				return GET_STACK_VALUE(absoluteIndex);
			}

		case EASM_OPERAND_TYPE_REGISTER_STACK_INDEX:
			{
				int absoluteIndex = 0;

				absoluteIndex = operand->stackIndex + coerceValueToInteger(&m_register1);

				return GET_STACK_VALUE(absoluteIndex);
			}

		case EASM_OPERAND_TYPE_REGISTER:
			{
				return (RuntimeValue *)operand->precomputedAddress;

				break;
			}

		default:
			{
				return operand;

				break;
			}
		}
	}

	int Script::resolveOperandAsInteger(int operandIndex)
	{
		return coerceValueToInteger(resolveOperandValue(operandIndex));
	}

	float Script::resolveOperandAsFloat(int operandIndex)
	{
		return coerceValueToFloat(resolveOperandValue(operandIndex));
	}

	std::string Script::resolveOperandAsString(int operandIndex)
	{
		return coerceValueToString(resolveOperandValue(operandIndex));
	}

	int Script::resolveOperandAsInstructionIndex(int operandIndex)
	{
		return resolveOperandValue(operandIndex)->instructionIndex;
	}

	int Script::resolveOperandAsFunctionIndex(int operandIndex)
	{
		RuntimeValue operandValue = *resolveOperandValue(operandIndex);

		if(operandValue.type != EASM_OPERAND_TYPE_FUNCTION_INDEX) return -1;

		return operandValue.functionIndex;
	}

	int Script::coerceValueToInteger(RuntimeValue *value)
	{
		switch(value->type)
		{
		case EASM_OPERAND_TYPE_INT:
			return value->integerLiteral;

			break;

		case EASM_OPERAND_TYPE_FLOAT:
			return (int)value->floatLiteral;

			break;

		case EASM_OPERAND_TYPE_STRING:
			return atoi(value->stringLiteral.c_str());

			break;

		default:
			return 0;

			break;
		}
	}

	float Script::coerceValueToFloat(RuntimeValue *value)
	{
		switch(value->type)
		{
		case EASM_OPERAND_TYPE_INT:
			return (float)value->integerLiteral;

			break;

		case EASM_OPERAND_TYPE_FLOAT:
			return value->floatLiteral;

			break;

		case EASM_OPERAND_TYPE_STRING:
			return (float)atof(value->stringLiteral.c_str());

			break;

		default:
			return 0.0f;

			break;
		}
	}

	std::string Script::coerceValueToString(RuntimeValue *value)
	{
		switch(value->type)
		{
		case EASM_OPERAND_TYPE_INT:
			{
				return INT_TO_STRING(value->integerLiteral);

				break;
			}

		case EASM_OPERAND_TYPE_FLOAT:
			{
				return FLOAT_TO_STRING(value->floatLiteral);

				break;
			}

		case EASM_OPERAND_TYPE_STRING:
			{
				return value->stringLiteral;

				break;
			}

		default:
			{
				return "NULL";

				break;
			}
		}
	}

	int Script::resolveStackIndex(int index)
	{
		return (index < 0 ? (index + m_stack.frameIndex) : (index));
	}

	int Script::resolveOperandAbsoluteStackIndex(int index)
	{
		//return (index < 0 ? (index + m_stack.frameIndex) : (index));
		return currentInstruction->operandList[index].stackIndex;
	}

	int Script::resolveOperandRelativeStackIndex(int index)
	{
		//return (index < 0 ? (index + m_stack.frameIndex) : (index));
		return currentInstruction->operandList[index].stackIndex + currentInstruction->operandList[index].offsetIndex;
	}

	RuntimeValue *Script::getStackValue(int index)
	{
		return &m_stack.elementList[RESOLVE_STACK_INDEX(index)];
	}

	void Script::setStackValue(int index, RuntimeValue *value)
	{
		//m_stack.elementList[RESOLVE_STACK_INDEX(index)] = value;
		//int stackIndex = RESOLVE_STACK_INDEX(index);

		copyOperandValue(&m_stack.elementList[RESOLVE_STACK_INDEX(index)], value);
	}

	void Script::pushOntoStack(RuntimeValue *value)
	{
		//m_stack.elementList[m_stack.topIndex] = value;
		if(m_stack.topIndex == m_stack.size - 1)
		{
			m_stack.size += ES_STACK_REALLOCATION_SIZE;

			m_stack.elementList.reserve(m_stack.size);
			for(int i = 0; i < ES_STACK_REALLOCATION_SIZE; ++i)
			{
				RuntimeValue runtimeValue;
				
				m_stack.elementList.push_back(runtimeValue);
			}
		}

		copyOperandValue(&m_stack.elementList[m_stack.topIndex], value);

		m_stack.topIndex++;
	}

	RuntimeValue *Script::popFromStack()
	{
		m_stack.topIndex--;

		return &m_stack.elementList[m_stack.topIndex];
	}

	void Script::pushStackFrame(int size)
	{
		m_stack.topIndex += size;

		if(m_stack.topIndex > m_stack.size - 2)
		{
			m_stack.size += ES_STACK_REALLOCATION_SIZE;

			m_stack.elementList.reserve(m_stack.size);
			for(int i = 0; i < ES_STACK_REALLOCATION_SIZE; ++i)
			{
				RuntimeValue runtimeValue;
				
				m_stack.elementList.push_back(runtimeValue);
			}
		}

		m_stack.frameIndex = m_stack.topIndex;
	}

	void Script::popStackFrame(int size)
	{
		m_stack.topIndex -= size;
	}

	void Script::copyOperandValue(RuntimeValue *destinationValue, RuntimeValue *sourceValue)
	{
		//*destinationValue = *sourceValue;

		memcpy(destinationValue, sourceValue, sizeof(RuntimeValue) - sizeof(std::string));

		if(destinationValue->type == ES_OPERAND_TYPE_STRING_INDEX)
		{
			destinationValue->stringLiteral = sourceValue->stringLiteral;
		}
	}

	Function Script::getFunction(int index)
	{
		return m_functionTable[index];
	}

	RuntimeValue *Script::getHostFunctionParameter(int index)
	{
		return m_hostParameters[index];
	}

	// Script life cycle

	void Script::reset()
	{
		m_stack.topIndex = 0;
		m_stack.frameIndex = 0;

		for(int i = 0; i < m_stack.size; ++i)
		{
			m_stack.elementList[i].type = EASM_OPERAND_TYPE_NULL;
		}

		//pushStackFrame(m_globalDataSize);

		if(m_functionTable.size())
		{
			call(ES_PRELOAD_FUNCTION_NAME, 0);

			if(m_isInitializeFunctionPresent)
			{
				call(ES_INITIALIZE_FUNCTION_NAME, 0);
			}
			else
			{
				m_instructionStream.currentInstruction = 0;
			}
		}

		//m_currentFunctionTopIndex = m_stack.topIndex;
	}

	int Script::runOneStep()
	{
		int initialInstructionIndex = m_instructionStream.currentInstruction;

		handleCurrentInstruction();

		if(m_instructionStream.currentInstruction == initialInstructionIndex)
		{
			m_instructionStream.currentInstruction++;
		}

		return 0;
	}

	void Script::handleCurrentInstruction()
	{
		currentInstruction = &m_instructionStream.instructionList[m_instructionStream.currentInstruction];
		currentInstructionOperandList = &currentInstruction->operandList;

		int opcode = currentInstruction->opcode;

		switch(opcode)
		{
			// Binary operations

		case ES_OPCODE_MOV:
			{
				copyOperandValue(resolveOperandValue(0), resolveOperandValue(1));

				break;
			}

		case ES_OPCODE_ADD:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);
				RuntimeValue *sourceValue = resolveOperandValue(OPERAND_POSITION_SOURCE);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					if(sourceValue->type == EASM_OPERAND_TYPE_STRING)
					{
						destinationValue->stringLiteral += sourceValue->stringLiteral;
					}
					else if(sourceValue->type == EASM_OPERAND_TYPE_INT)
					{
						destinationValue->stringLiteral += INT_TO_STRING(sourceValue->integerLiteral);
					}
					else if(sourceValue->type == EASM_OPERAND_TYPE_FLOAT)
					{
						destinationValue->stringLiteral += FLOAT_TO_STRING(sourceValue->floatLiteral);
					}

					break;
				}

				if(sourceValue->type == EASM_OPERAND_TYPE_STRING)
				{
					if(destinationValue->type == EASM_OPERAND_TYPE_INT)
					{
						destinationValue->stringLiteral = INT_TO_STRING(destinationValue->integerLiteral) + sourceValue->stringLiteral;
					}
					else if(destinationValue->type == EASM_OPERAND_TYPE_FLOAT)
					{
						destinationValue->stringLiteral = FLOAT_TO_STRING(destinationValue->integerLiteral) + sourceValue->stringLiteral;
					}

					destinationValue->type = EASM_OPERAND_TYPE_STRING;

					break;
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT && sourceValue->type == EASM_OPERAND_TYPE_INT)
				{
					//message("Operand0 : " + INT_TO_STRING(destinationValue->integerLiteral) + ", Operand1 : " + INT_TO_STRING(RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE)));
					destinationValue->integerLiteral += sourceValue->integerLiteral;
				}
				else
				{
					//message("Operand0 : " + FLOAT_TO_STRING(destinationValue->floatLiteral) + ", Operand1 : " + FLOAT_TO_STRING(RESOLVE_OPERAND_AS_FLOAT(OPERAND_POSITION_SOURCE)));
					if(destinationValue->type == EASM_OPERAND_TYPE_INT)
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = destinationValue->integerLiteral;
						destinationValue->floatLiteral += sourceValue->floatLiteral;
					}
					else if(sourceValue->type == EASM_OPERAND_TYPE_INT)
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = sourceValue->integerLiteral;
						destinationValue->floatLiteral += sourceValue->floatLiteral;
					}
					else
					{
						destinationValue->floatLiteral += sourceValue->floatLiteral;
					}
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;
				//message("Result : " + INT_TO_STRING(resolveOperandValue(OPERAND_POSITION_DESTINATION)->floatLiteral));

				break;
			}

		case ES_OPCODE_SUB:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);
				RuntimeValue *sourceValue = resolveOperandValue(OPERAND_POSITION_SOURCE);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(sourceValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(sourceValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(sourceValue);

						sourceValue->type = EASM_OPERAND_TYPE_INT;
						sourceValue->integerLiteral = result;
					}
					else
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT && sourceValue->type == EASM_OPERAND_TYPE_INT)
				{
					//message("Operand0 : " + INT_TO_STRING(destinationValue->integerLiteral) + ", Operand1 : " + INT_TO_STRING(RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE)));
					destinationValue->integerLiteral -= sourceValue->integerLiteral;
				}
				else
				{
					//message("Operand0 : " + FLOAT_TO_STRING(destinationValue->floatLiteral) + ", Operand1 : " + FLOAT_TO_STRING(RESOLVE_OPERAND_AS_FLOAT(OPERAND_POSITION_SOURCE)));
					if(destinationValue->type == EASM_OPERAND_TYPE_INT)
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = destinationValue->integerLiteral;
						destinationValue->floatLiteral -= sourceValue->floatLiteral;
					}
					else if(sourceValue->type == EASM_OPERAND_TYPE_INT)
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = sourceValue->integerLiteral;
						destinationValue->floatLiteral -= sourceValue->floatLiteral;
					}
					else
					{
						destinationValue->floatLiteral -= sourceValue->floatLiteral;
					}
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;
				//message("Result : " + INT_TO_STRING(resolveOperandValue(OPERAND_POSITION_DESTINATION)->floatLiteral));

				break;
			}

		case ES_OPCODE_MUL:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);
				RuntimeValue *sourceValue = resolveOperandValue(OPERAND_POSITION_SOURCE);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(sourceValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(sourceValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(sourceValue);

						sourceValue->type = EASM_OPERAND_TYPE_INT;
						sourceValue->integerLiteral = result;
					}
					else
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT && sourceValue->type == EASM_OPERAND_TYPE_INT)
				{
					//message("Operand0 : " + INT_TO_STRING(destinationValue->integerLiteral) + ", Operand1 : " + INT_TO_STRING(RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE)));
					destinationValue->integerLiteral *= sourceValue->integerLiteral;
				}
				else
				{
					//message("Operand0 : " + FLOAT_TO_STRING(destinationValue->floatLiteral) + ", Operand1 : " + FLOAT_TO_STRING(RESOLVE_OPERAND_AS_FLOAT(OPERAND_POSITION_SOURCE)));
					if(destinationValue->type == EASM_OPERAND_TYPE_INT)
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = destinationValue->integerLiteral;
						destinationValue->floatLiteral *= sourceValue->floatLiteral;
					}
					else if(sourceValue->type == EASM_OPERAND_TYPE_INT)
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = sourceValue->integerLiteral;
						destinationValue->floatLiteral *= sourceValue->floatLiteral;
					}
					else
					{
						destinationValue->floatLiteral *= sourceValue->floatLiteral;
					}
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;
				//message("Result : " + INT_TO_STRING(resolveOperandValue(OPERAND_POSITION_DESTINATION)->floatLiteral));

				break;
			}

		case ES_OPCODE_DIV:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);
				RuntimeValue *sourceValue = resolveOperandValue(OPERAND_POSITION_SOURCE);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(sourceValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(sourceValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(sourceValue);

						sourceValue->type = EASM_OPERAND_TYPE_INT;
						sourceValue->integerLiteral = result;
					}
					else
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT && sourceValue->type == EASM_OPERAND_TYPE_INT)
				{
					//message("Operand0 : " + INT_TO_STRING(destinationValue->integerLiteral) + ", Operand1 : " + INT_TO_STRING(RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE)));
					destinationValue->integerLiteral /= sourceValue->integerLiteral;
				}
				else
				{
					//message("Operand0 : " + FLOAT_TO_STRING(destinationValue->floatLiteral) + ", Operand1 : " + FLOAT_TO_STRING(RESOLVE_OPERAND_AS_FLOAT(OPERAND_POSITION_SOURCE)));
					if(destinationValue->type == EASM_OPERAND_TYPE_INT)
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = destinationValue->integerLiteral;
						destinationValue->floatLiteral /= sourceValue->floatLiteral;
					}
					else if(sourceValue->type == EASM_OPERAND_TYPE_INT)
					{
						sourceValue->type = EASM_OPERAND_TYPE_FLOAT;
						sourceValue->floatLiteral = sourceValue->integerLiteral;
						destinationValue->floatLiteral /= sourceValue->floatLiteral;
					}
					else
					{
						destinationValue->floatLiteral /= sourceValue->floatLiteral;
					}
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;
				//message("Result : " + INT_TO_STRING(resolveOperandValue(OPERAND_POSITION_DESTINATION)->floatLiteral));

				break;
			}

		case ES_OPCODE_MOD:
			{
				int result = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);
				result %= RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE);

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = result;

				break;
			}

		case ES_OPCODE_EXP:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT)
				{
					int initialValue = destinationValue->integerLiteral;

					for(int i = 0; i < RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE) - 1; ++i)
					{
						destinationValue->integerLiteral *= initialValue;
					}
				}
				else
				{
					float initialValue = destinationValue->floatLiteral;

					for(int i = 0; i < RESOLVE_OPERAND_AS_FLOAT(OPERAND_POSITION_SOURCE) - 1; ++i)
					{
						destinationValue->floatLiteral *= initialValue;
					}
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;

				break;
			}

		case ES_OPCODE_NEG:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);
						//free(destinationValue->stringLiteral);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						//free(destinationValue->stringLiteral);

						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT)
				{
					destinationValue->integerLiteral *= -1;
				}
				else
				{
					destinationValue->floatLiteral *= -1;
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;

				break;
			}

		case ES_OPCODE_INC:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);
						//free(destinationValue->stringLiteral);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						//free(destinationValue->stringLiteral);

						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT)
				{
					destinationValue->integerLiteral++;
				}
				else
				{
					destinationValue->floatLiteral++;
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;

				break;
			}

		case ES_OPCODE_DEC:
			{
				RuntimeValue *destinationValue = resolveOperandValue(OPERAND_POSITION_DESTINATION);

				if(destinationValue->type == EASM_OPERAND_TYPE_STRING)
				{
					float coercionResult = coerceValueToFloat(destinationValue);

					if(coercionResult == (int)coercionResult)
					{
						int result = coerceValueToInteger(destinationValue);
						//free(destinationValue->stringLiteral);

						destinationValue->type = EASM_OPERAND_TYPE_INT;
						destinationValue->integerLiteral = result;
					}
					else
					{
						//free(destinationValue->stringLiteral);

						destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
						destinationValue->floatLiteral = coercionResult;
					}
				}

				if(destinationValue->type == EASM_OPERAND_TYPE_INT)
				{
					destinationValue->integerLiteral--;
				}
				else
				{
					destinationValue->floatLiteral--;
				}

				//*resolveOperandValue(OPERAND_POSITION_DESTINATION) = destinationValue;

				break;
			}

			// Bitwise

		case ES_OPCODE_AND:
			{
				int destinationValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);
				int sourceValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE);

				destinationValue &= sourceValue;

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = destinationValue;

				break;
			}

		case ES_OPCODE_OR:
			{
				int destinationValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);
				int sourceValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE);

				destinationValue |= sourceValue;

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = destinationValue;

				break;
			}

		case ES_OPCODE_XOR:
			{
				int destinationValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);
				int sourceValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE);

				destinationValue ^= sourceValue;

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = destinationValue;

				break;
			}

		case ES_OPCODE_NOT:
			{
				int destinationValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);

				destinationValue = ~destinationValue;

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = destinationValue;

				break;
			}

		case ES_OPCODE_SHL:
			{
				int destinationValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);
				int sourceValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE);

				destinationValue <<= sourceValue;

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = destinationValue;

				break;
			}

		case ES_OPCODE_SHR:
			{
				int destinationValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_DESTINATION);
				int sourceValue = RESOLVE_OPERAND_AS_INT(OPERAND_POSITION_SOURCE);

				destinationValue >>= sourceValue;

				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).type = EASM_OPERAND_TYPE_INT;
				(*resolveOperandValue(OPERAND_POSITION_DESTINATION)).integerLiteral = destinationValue;

				break;
			}

			// String processing

		/*case ES_OPCODE_CONCAT:
			{
				RuntimeValue destinationValue = *resolveOperandValue(0);
				char *destinationString = coerceValueToString(destinationValue);
				RuntimeValue sourceValue = *resolveOperandValue(1);
				char *sourceString = coerceValueToString(sourceValue);

				char *result = new char[strlen(destinationString) + strlen(sourceString) + 1];
				strcpy(result, destinationString);
				strcat(result, sourceString);
				result[strlen(destinationString) + strlen(sourceString)] = '\0';

				free(destinationString);
				if(sourceValue.type != EASM_OPERAND_TYPE_STRING) free(sourceString);

				(*resolveOperandValue(0)).type = EASM_OPERAND_TYPE_STRING;
				(*resolveOperandValue(0)).stringLiteral = result;

				break;
			}

		case ES_OPCODE_GETCHAR:
			{
				RuntimeValue sourceValue = *resolveOperandValue(1);
				char *sourceString = coerceValueToString(sourceValue);

				char *result = new char[2];

				result[0] = sourceString[RESOLVE_OPERAND_AS_INT(2)];
				result[1] = '\0';

				RuntimeValue destinationValue = *resolveOperandValue(0);
				if(destinationValue.type == EASM_OPERAND_TYPE_STRING)
				{
					free(destinationValue.stringLiteral);
				}
				else
				{
					(*resolveOperandValue(0)).type = EASM_OPERAND_TYPE_STRING;
				}

				(*resolveOperandValue(0)).stringLiteral = result;

				if(sourceValue.type != EASM_OPERAND_TYPE_STRING) (sourceString);

				break;
			}

		case ES_OPCODE_SETCHAR:
			{
				RuntimeValue destinationValue = *resolveOperandValue(0);
				char *destinationString = coerceValueToString(destinationValue);

				char *result = new char[strlen(destinationString) + 1];
				strcpy(result, destinationString);

				RuntimeValue sourceValue = *resolveOperandValue(2);
				char *sourceString = coerceValueToString(sourceValue);

				result[RESOLVE_OPERAND_AS_INT(1)] = sourceString[0];

				free(destinationString);
				if(sourceValue.type != EASM_OPERAND_TYPE_STRING) free(sourceString);

				(*resolveOperandValue(0)).type = EASM_OPERAND_TYPE_STRING;
				(*resolveOperandValue(0)).stringLiteral = result;

				break;
			}*/

			// Conditional branching

		case ES_OPCODE_JMP:
			{
				//m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(0);
				m_instructionStream.currentInstruction = (*currentInstructionOperandList)[0].instructionIndex;
				//debugMessage(INT_TO_STRING(RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(0)));

				break;
			}

		case ES_OPCODE_JE:
			{
				RuntimeValue *value0 = resolveOperandValue(0);
				RuntimeValue *value1 = resolveOperandValue(1);

				bool jump = 0;

				switch(value0->type)
				{
				case EASM_OPERAND_TYPE_INT:
					if(value0->integerLiteral == coerceValueToInteger(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					if(approximateEquals(value0->floatLiteral, coerceValueToFloat(value1))) jump = 1;

					break;

				case EASM_OPERAND_TYPE_STRING:
					if(strcmp(value0->stringLiteral.c_str(), coerceValueToString(value1).c_str()) == 0) jump = 1;

					break;
				}

				if(jump) m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(2);

				break;
			}

		case ES_OPCODE_JNE:
			{
				RuntimeValue *value0 = resolveOperandValue(0);
				RuntimeValue *value1 = resolveOperandValue(1);

				bool jump = 0;

				switch(value0->type)
				{
				case EASM_OPERAND_TYPE_INT:
					if(value0->integerLiteral != coerceValueToInteger(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					if(!approximateEquals(value0->floatLiteral, coerceValueToFloat(value1))) jump = 1;

					break;

				case EASM_OPERAND_TYPE_STRING:
					if(strcmp(value0->stringLiteral.c_str(), coerceValueToString(value1).c_str()) != 0) jump = 1;

					break;
				}

				if(jump) m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(2);

				break;
			}

		case ES_OPCODE_JG:
			{
				RuntimeValue *value0 = resolveOperandValue(0);
				RuntimeValue *value1 = resolveOperandValue(1);

				bool jump = 0;

				switch(value0->type)
				{
				case EASM_OPERAND_TYPE_INT:
					if(value0->integerLiteral > coerceValueToInteger(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					if(value0->floatLiteral > coerceValueToFloat(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_STRING:
					if(strcmp(value0->stringLiteral.c_str(), coerceValueToString(value1).c_str()) > 0) jump = 1;

					break;
				}

				if(jump) m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(2);

				break;
			}

		case ES_OPCODE_JL:
			{
				RuntimeValue *value0 = resolveOperandValue(0);
				RuntimeValue *value1 = resolveOperandValue(1);

				bool jump = 0;

				switch(value0->type)
				{
				case EASM_OPERAND_TYPE_INT:
					if(value0->integerLiteral < coerceValueToInteger(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					if(value0->floatLiteral < coerceValueToFloat(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_STRING:
					if(strcmp(value0->stringLiteral.c_str(), coerceValueToString(value1).c_str()) < 0) jump = 1;

					break;
				}

				if(jump) m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(2);

				break;
			}

		case ES_OPCODE_JGE:
			{
				RuntimeValue *value0 = resolveOperandValue(0);
				RuntimeValue *value1 = resolveOperandValue(1);

				bool jump = 0;

				switch(value0->type)
				{
				case EASM_OPERAND_TYPE_INT:
					if(value0->integerLiteral >= coerceValueToInteger(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					if(value0->floatLiteral >= coerceValueToFloat(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_STRING:
					if(strcmp(value0->stringLiteral.c_str(), coerceValueToString(value1).c_str()) >= 0) jump = 1;

					break;
				}

				if(jump) m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(2);

				break;
			}

		case ES_OPCODE_JLE:
			{
				RuntimeValue *value0 = resolveOperandValue(0);
				RuntimeValue *value1 = resolveOperandValue(1);

				bool jump = 0;

				switch(value0->type)
				{
				case EASM_OPERAND_TYPE_INT:
					if(value0->integerLiteral <= coerceValueToInteger(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					if(value0->floatLiteral <= coerceValueToFloat(value1)) jump = 1;

					break;

				case EASM_OPERAND_TYPE_STRING:
					if(strcmp(value0->stringLiteral.c_str(), coerceValueToString(value1).c_str()) <= 0) jump = 1;

					break;
				}

				if(jump) m_instructionStream.currentInstruction = RESOLVE_OPERAND_AS_INSTRUCTION_INDEX(2);

				break;
			}

			// Stack interface

		case ES_OPCODE_PUSH:
			{
				pushOntoStack(resolveOperandValue(0));

				break;
			}

		case ES_OPCODE_POP:
			{
				//RuntimeValue *operandPointer = resolveOperandValue(0);

				/*if(operandPointer->type == EASM_OPERAND_TYPE_STRING)
				{
					free(operandPointer->stringLiteral);
				}*/

				copyOperandValue(resolveOperandValue(0), popFromStack());

				break;
			}

			// Function interface

		case ES_OPCODE_CALL:
			{
				int functionIndex = resolveOperandAsFunctionIndex(0);

				/*if(functionIndex == -1)
				{
					throwError("Undefined function being called");
				}*/

				Function destinationFunction = getFunction(functionIndex);

				for(int i = 0; i < destinationFunction.parameterCount; ++i)
				{
					RuntimeValue operand = *resolveOperandValue(i + 1);

					parameterList[i] = operand;
				}

				int frameIndex = m_stack.frameIndex;
				int topIndex = m_stack.topIndex;

				RuntimeValue returnAddress;
				returnAddress.type = EASM_OPERAND_TYPE_INSTRUCTION_INDEX;
				returnAddress.instructionIndex = m_instructionStream.currentInstruction + 1;
				pushOntoStack(&returnAddress);

				pushStackFrame(destinationFunction.stackFrameSize + 1);

				m_currentFunctionTopIndex = m_stack.topIndex;

				RuntimeValue functionIndexValue;
				functionIndexValue.type = EASM_OPERAND_TYPE_FUNCTION_INDEX;
				functionIndexValue.functionIndex = functionIndex;
				functionIndexValue.offsetIndex = frameIndex;
				setStackValue(m_stack.topIndex - 1, &functionIndexValue);

				//Debug::throwMessage("Call : " + destinationFunction.name + ", " + INT_TO_STRING(destinationFunction.entryPoint));

				m_functionDepth++;

				for(int i = 0; i < destinationFunction.parameterCount; ++i)
				{
					setStackValue(m_stack.topIndex - 2 - i, &parameterList[i]);
				}

				m_instructionStream.currentInstruction = destinationFunction.entryPoint;

				break;
			}

		case ES_OPCODE_RETURN:
			{
				m_stack.topIndex = m_currentFunctionTopIndex;

				RuntimeValue *functionIndexValue = popFromStack();

				Function currentFunction = getFunction(functionIndexValue->functionIndex);
				int frameIndex = functionIndexValue->offsetIndex;

				popStackFrame(currentFunction.stackFrameSize);

				RuntimeValue *returnAddress = popFromStack();

				m_stack.frameIndex = frameIndex;

				m_instructionStream.currentInstruction = returnAddress->instructionIndex;

				//Debug::throwMessage("Return to : " + INT_TO_STRING(returnAddress->instructionIndex));

				m_functionDepth--;

				break;
			}

		case ES_OPCODE_CALLHOST:
			{
				RuntimeValue *value = resolveOperandValue(0);

				for(int i = 1; i < currentInstruction->operandCount; ++i)
				{
					m_hostParameters[i - 1] = resolveOperandValue(i);
				}

				for(int i = currentInstruction->operandCount; i < MAX_CALL_HOST_PARAMTETER_COUNT; ++i)
				{
					m_hostParameters[i - 1] = 0;
				}

				callHostFunction(value->hostFunctionIndex);

				break;
			}

			// Script system

		case ES_OPCODE_GETHOSTGLOBAL:
			{
				RuntimeValue *destinationValue = resolveOperandValue(0);
				RuntimeValue *sourceValue = resolveOperandValue(1);
				HostGlobalVariable *hostGlobal;

				if(sourceValue->type != EASM_OPERAND_TYPE_STRING)
				{
					char *identifier = (char *)coerceValueToString(sourceValue).c_str();
					hostGlobal = getHostGlobalVariable(identifier);

					if(!hostGlobal) throwError("\"" + (string)identifier + "\" is not a registered host global variable");

					//free(identifier);
				}
				else
				{
					hostGlobal = getHostGlobalVariable((char *)sourceValue->stringLiteral.c_str());

					if(!hostGlobal) throwError("\"" + (string)sourceValue->stringLiteral + "\" is not a registered host global variable");
				}

				/*if(destinationValue.type == EASM_OPERAND_TYPE_STRING)
				{
					free(destinationValue.stringLiteral);
				}*/

				switch(hostGlobal->type)
				{
				case HostGlobalVariable_Int:
					destinationValue->type = EASM_OPERAND_TYPE_INT;
					destinationValue->integerLiteral = *(int *)hostGlobal->variablePointer;

					break;

				case HostGlobalVariable_Float:
					destinationValue->type = EASM_OPERAND_TYPE_FLOAT;
					destinationValue->floatLiteral = *(float *)hostGlobal->variablePointer;

					break;

				case HostGlobalVariable_String:
					destinationValue->type = EASM_OPERAND_TYPE_STRING;
					destinationValue->stringLiteral = (char *)hostGlobal->variablePointer;

					break;
				}

				//*resolveOperandValue(0) = destinationValue;

				break;
			}
		}
	}

	void Script::cleanup()
	{
		isInitialized = 0;

		for(int i = 0; i < m_instructionStream.instructionCount; ++i)
		{	
			m_instructionStream.instructionList[i].operandList.clear();
		}

		m_instructionStream.instructionList.clear();
		m_instructionStream.instructionCount = 0;

		m_stack.elementList.clear();
		m_stack.size = 0;
		m_stack.topIndex = 0;
		m_stack.frameIndex = 0;

		m_functionCount = 0;
		m_functionDepth = 0;

		m_functionTable.clear();

		m_globalDataSize = 0;

		m_isInitializeFunctionPresent = 0;
		m_initializeFunctionIndex = 0;

		m_isUpdateFunctionPresent = 0;
		m_updateFunctionIndex = 0;

		m_isRenderFunctionPresent = 0;
		m_renderFunctionIndex = 0;

		m_isEndFunctionPresent = 0;
		m_endFunctionIndex = 0;

		m_currentFunctionTopIndex = 0;

		m_globalVariableList.clear();
		m_globalVariableCount = 0;
	}

	EagleScriptVirtualMachine::EagleScriptVirtualMachine()
	{
		//Debug::throwMessage("Creating threads");

		/*for(int i = 0; i < ES_THREAD_COUNT; ++i)
		{
			//threadRangeList[i].head = (ES_MAX_SCRIPT_COUNT / ES_THREAD_COUNT) * i;
			//threadRangeList[i].tail = threadRangeList[i].head;

			//threadHandleList[i] = CreateThread(0, 0, (PTHREAD_START_ROUTINE)eagleScriptThreadFunction, new int(i), 0, 0);
		}*/

		//mutexHandle = CreateMutex(0, 0, 0);

		//scriptCount = 0;

		EagleAssembler::initializeInstructionTable();
	}

	EagleScriptVirtualMachine::~EagleScriptVirtualMachine()
	{
		/*for(int i = 0; i < ES_THREAD_COUNT; ++i)
		{
			CloseHandle(threadHandleList[i]);
		}*/

		//CloseHandle(mutexHandle);
	}

	void EagleScriptVirtualMachine::addScript(Script *script)
	{
		std::vector<Script *>::iterator listIterator;
		for(listIterator = m_scriptList.begin(); listIterator != m_scriptList.end(); ++listIterator)
		{
			if((*listIterator) == script)
			{
				return;
			}
		}

		m_scriptList.push_back(script);
	}

	void EagleScriptVirtualMachine::removeScript(Script *script)
	{
		std::vector<Script *>::iterator listIterator;
		for(listIterator = m_scriptList.begin(); listIterator != m_scriptList.end(); ++listIterator)
		{
			if((*listIterator) == script)
			{
				m_scriptList.erase(listIterator);

				return;
			}
		}
	}

	void EagleScriptVirtualMachine::updateScripts()
	{
		float frameTime = EAGLE_DEFAULT_FRAME_TIME;

		ScriptFunctionParameter parameterList[1];
		parameterList[0].type = SCRIPT_PARAMETER_TYPE_FLOAT;
		parameterList[0].floatValue = frameTime;

		std::vector<Script *>::iterator listIterator;
		for(listIterator = m_scriptList.begin(); listIterator != m_scriptList.end(); ++listIterator)
		{
			(*listIterator)->call("Update", 0, parameterList, 1);
		}
	}

	// Host interface

	int Script::getGlobalVariableAsInt(char *identifier)
	{
		bool matchFound = 0;
		int stackIndex = 0;
		std::vector<EagleAssembler::GlobalVariable>::iterator listIterator;
		for(listIterator = m_globalVariableList.begin(); listIterator != m_globalVariableList.end(); ++listIterator)
		{
			if(strcmp(identifier, (*listIterator).identifier.c_str()) == 0)
			{
				matchFound = 1;
				stackIndex = (*listIterator).stackIndex;

				break;
			}
		}

		if(!matchFound)
		{
			return 0;
		}

		return coerceValueToInteger(GET_STACK_VALUE(stackIndex));
	}

	float Script::getGlobalVariableAsFloat(char *identifier)
	{
		bool matchFound = 0;
		int stackIndex = 0;
		std::vector<EagleAssembler::GlobalVariable>::iterator listIterator;
		for(listIterator = m_globalVariableList.begin(); listIterator != m_globalVariableList.end(); ++listIterator)
		{
			if(strcmp(identifier, (*listIterator).identifier.c_str()) == 0)
			{
				matchFound = 1;
				stackIndex = (*listIterator).stackIndex;

				break;
			}
		}

		if(!matchFound) return 0;

		return coerceValueToFloat(GET_STACK_VALUE(stackIndex));
	}

	std::string Script::getGlobalVariableAsString(char *identifier)
	{
		bool matchFound = 0;
		int stackIndex = 0;
		std::vector<EagleAssembler::GlobalVariable>::iterator listIterator;
		for(listIterator = m_globalVariableList.begin(); listIterator != m_globalVariableList.end(); ++listIterator)
		{
			if(strcmp(identifier, (*listIterator).identifier.c_str()) == 0)
			{
				matchFound = 1;
				stackIndex = (*listIterator).stackIndex;

				break;
			}
		}

		if(!matchFound)
		{
			//message("Match not found");

			return 0;
		}

		return coerceValueToString(&m_stack.elementList[(stackIndex)]);
	}

	HostGlobalVariable::HostGlobalVariable()
	{
		variablePointer = 0;
		//isValid = 0;
		identifier = "";
		type = HostGlobalVariable_Int;
	}

	HostFunction::HostFunction()
	{
		functionPointer = 0;
		identifier = "";
	}

	int registerGlobalVariable(void *globalVariablePointer, char *identifier, HostGlobalVariableType type)
	{
		HostGlobalVariable globalVariable;
		globalVariable.variablePointer = globalVariablePointer;
		globalVariable.identifier = identifier;
		globalVariable.type = type;

		virtualMachine.m_hostGlobalVariableList.push_back(globalVariable);

		return 0;
	}

	void unregisterGlobalVariable(char *identifier)
	{
		std::vector<HostGlobalVariable>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostGlobalVariableList.begin(); listIterator != virtualMachine.m_hostGlobalVariableList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				virtualMachine.m_hostGlobalVariableList.erase(listIterator);

				return;
			}
		}
	}

	HostGlobalVariable *Script::getHostGlobalVariable(char *identifier)
	{
		std::vector<HostGlobalVariable>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostGlobalVariableList.begin(); listIterator != virtualMachine.m_hostGlobalVariableList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				return &(*listIterator);
			}
		}

		return 0;
	}

	int Script::getHostGlobalVariableAsInt(char *identifier)
	{
		std::vector<HostGlobalVariable>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostGlobalVariableList.begin(); listIterator != virtualMachine.m_hostGlobalVariableList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				switch((*listIterator).type)
				{
				case HostGlobalVariable_Int:
					return *(int *)((*listIterator).variablePointer);

					break;

				case HostGlobalVariable_Float:
					return (int)(*(float *)((*listIterator).variablePointer));

					break;

				case HostGlobalVariable_String:
					return atoi((char *)((*listIterator).variablePointer));

					break;
				}
			}
		}

		return 0;
	}

	float Script::getHostGlobalVariableAsFloat(char *identifier)
	{
		std::vector<HostGlobalVariable>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostGlobalVariableList.begin(); listIterator != virtualMachine.m_hostGlobalVariableList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				switch((*listIterator).type)
				{
				case HostGlobalVariable_Int:
					return (float)(*(int *)((*listIterator).variablePointer));

					break;

				case HostGlobalVariable_Float:
					return *(float *)((*listIterator).variablePointer);

					break;

				case HostGlobalVariable_String:
					return atof((char *)((*listIterator).variablePointer));

					break;
				}
			}
		}

		return 0;
	}

	std::string Script::getHostGlobalVariableAsString(char *identifier)
	{
		std::vector<HostGlobalVariable>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostGlobalVariableList.begin(); listIterator != virtualMachine.m_hostGlobalVariableList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				switch((*listIterator).type)
				{
				case HostGlobalVariable_Int:
					return INT_TO_STRING(*(int *)((*listIterator).variablePointer));

					break;

				case HostGlobalVariable_Float:
					return (char *)(FLOAT_TO_STRING(*(float *)((*listIterator).variablePointer)).c_str());

					break;

				case HostGlobalVariable_String:
					return (char *)((*listIterator).variablePointer);

					break;
				}
			}
		}
	}

	int registerHostFunction(void (*functionPointer)(Script *), char *identifier)
	{
		HostFunction hostFunction;
		hostFunction.functionPointer = functionPointer;
		hostFunction.identifier = identifier;

		virtualMachine.m_hostFunctionList.push_back(hostFunction);

		return 0;
	}

	void unregisterHostFunction(char *identifier)
	{
		std::vector<HostFunction>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostFunctionList.begin(); listIterator != virtualMachine.m_hostFunctionList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				virtualMachine.m_hostFunctionList.erase(listIterator);

				return;
			}

		}
	}

	void Script::callHostFunction(int index)
	{
		virtualMachine.m_hostFunctionList[index].functionPointer(this);
	}

	int coerceValueToInteger(RuntimeValue value)
	{
		switch(value.type)
		{
		case EASM_OPERAND_TYPE_INT:
			return value.integerLiteral;

			break;

		case EASM_OPERAND_TYPE_FLOAT:
			return (int)value.floatLiteral;

			break;

		case EASM_OPERAND_TYPE_STRING:
			return atoi(value.stringLiteral.c_str());

			break;

		default:
			return 0;

			break;
		}
	}

	float coerceValueToFloat(RuntimeValue value)
	{
		switch(value.type)
		{
		case EASM_OPERAND_TYPE_INT:
			return (float)value.integerLiteral;

			break;

		case EASM_OPERAND_TYPE_FLOAT:
			return value.floatLiteral;

			break;

		case EASM_OPERAND_TYPE_STRING:
			return (float)atof(value.stringLiteral.c_str());

			break;

		default:
			return 0.0f;

			break;
		}
	}

	std::string coerceValueToString(RuntimeValue value)
	{
		switch(value.type)
		{
		case EASM_OPERAND_TYPE_INT:
			{
				return INT_TO_STRING(value.integerLiteral);

				break;
			}

		case EASM_OPERAND_TYPE_FLOAT:
			{
				return FLOAT_TO_STRING(value.floatLiteral);

				break;
			}

		case EASM_OPERAND_TYPE_STRING:
			{
				return value.stringLiteral;

				break;
			}

		default:
			{
				return "NULL";

				break;
			}
		}
	}

	int getScriptParameterAsInt(int index, Script *caller)
	{
		if(index > MAX_CALL_HOST_PARAMTETER_COUNT - 2)
		{
			throwError("Script cannot pass more than " + INT_TO_STRING(MAX_CALL_HOST_PARAMTETER_COUNT - 1) + " parameters to the host program function");
		}

		RuntimeValue *value = caller->getHostFunctionParameter(index);

		if(!value)
		{
			return 0;
		}

		return coerceValueToInteger(*value);
	}

	float getScriptParameterAsFloat(int index, Script *caller)
	{
		if(index > MAX_CALL_HOST_PARAMTETER_COUNT - 2)
		{
			throwError("Script cannot pass more than " + INT_TO_STRING(MAX_CALL_HOST_PARAMTETER_COUNT - 1) + " parameters to the host program function");
		}

		RuntimeValue *value = caller->getHostFunctionParameter(index);

		if(!value)
		{
			return 0.0f;
		}

		return coerceValueToFloat(*value);
	}

	std::string getScriptParameterAsString(int index, Script *caller)
	{
		if(index > MAX_CALL_HOST_PARAMTETER_COUNT - 2)
		{
			throwError("Script cannot pass more than " + INT_TO_STRING(MAX_CALL_HOST_PARAMTETER_COUNT - 1) + " parameters to the host program function");
		}

		RuntimeValue *value = caller->getHostFunctionParameter(index);

		if(!value)
		{
			return "";
		}

		return coerceValueToString(*value);
	}

	void returnIntToScript(int value, Script *caller)
	{
		virtualMachine.returnIntToScript(value, caller);
	}

	void returnFloatToScript(float value, Script *caller)
	{
		virtualMachine.returnFloatToScript(value, caller);
	}

	void returnStringToScript(std::string value, Script *caller)
	{
		virtualMachine.returnStringToScript(value, caller);
	}

	void EagleScriptVirtualMachine::returnIntToScript(int value, void *caller)
	{
		Script *script = (Script *)caller;

		/*if(script->m_returnValueFromHost.type == EASM_OPERAND_TYPE_STRING)
		{
			if(strlen(script->m_returnValueFromHost.stringLiteral)) free(script->m_returnValueFromHost.stringLiteral);
		}*/

		script->m_returnValueFromHost.type = EASM_OPERAND_TYPE_INT;
		script->m_returnValueFromHost.integerLiteral = value;
	}

	void EagleScriptVirtualMachine::returnFloatToScript(float value, void *caller)
	{
		Script *script = (Script *)caller;

		/*if(script->m_returnValueFromHost.type == EASM_OPERAND_TYPE_STRING)
		{
			if(strlen(script->m_returnValueFromHost.stringLiteral)) free(script->m_returnValueFromHost.stringLiteral);
		}*/

		script->m_returnValueFromHost.type = EASM_OPERAND_TYPE_FLOAT;
		script->m_returnValueFromHost.floatLiteral = value;
	}

	void EagleScriptVirtualMachine::returnStringToScript(std::string value, void *caller)
	{
		Script *script = (Script *)caller;

		/*if(script->m_returnValueFromHost.type == EASM_OPERAND_TYPE_STRING)
		{
			if(strlen(script->m_returnValueFromHost.stringLiteral)) free(script->m_returnValueFromHost.stringLiteral);
		}*/

		script->m_returnValueFromHost.type = EASM_OPERAND_TYPE_STRING;
		script->m_returnValueFromHost.stringLiteral = value;
	}

	void callScriptFunction(Script *script, char *functionName, ScriptFunctionParameter *parameterList, int parameterCount)
	{
		virtualMachine.callScriptFunction(script, functionName, parameterList, parameterCount);
	}

	void EagleScriptVirtualMachine::callScriptFunction(void *script, char *functionName, ScriptFunctionParameter *parameterList, int parameterCount)
	{
		Script *target = (Script *)script;

		RuntimeValue initialReturnValue, initialHostReturnValue, initialRegister0, initialRegister1;
		initialReturnValue = target->m_returnValue;
		initialHostReturnValue = target->m_returnValueFromHost;
		initialRegister0 = target->m_register0;
		initialRegister1 = target->m_register1;
		int initialStackTopIndex = target->m_stack.topIndex;
		int initialStackFrameIndex = target->m_stack.frameIndex;

		int functionIndex = target->getFunctionIndexByName(functionName);

		Function destinationFunction = target->getFunction(functionIndex);

		int frameIndex = target->m_stack.frameIndex;

		RuntimeValue returnAddress;
		returnAddress.type = EASM_OPERAND_TYPE_INSTRUCTION_INDEX;
		returnAddress.instructionIndex = target->m_instructionStream.currentInstruction;
		target->pushOntoStack(&returnAddress);

		target->pushStackFrame(destinationFunction.stackFrameSize + 1);

		target->m_currentFunctionTopIndex = target->m_stack.topIndex;

		RuntimeValue functionIndexValue;
		functionIndexValue.type = EASM_OPERAND_TYPE_FUNCTION_INDEX;
		functionIndexValue.functionIndex = functionIndex;
		functionIndexValue.offsetIndex = frameIndex;
		target->setStackValue(target->m_stack.topIndex - 1, &functionIndexValue);

		int initialFunctionDepth = target->m_functionDepth;

		target->m_functionDepth++;

		if(parameterList)
		{
			for(int i = 0; i < destinationFunction.parameterCount; ++i)
			{
				RuntimeValue parameter;

				switch(parameterList[i].type)
				{
				case SCRIPT_PARAMETER_TYPE_INT:
					parameter.type = EASM_OPERAND_TYPE_INT;
					parameter.integerLiteral = parameterList[i].intValue;

					break;

				case SCRIPT_PARAMETER_TYPE_FLOAT:
					parameter.type = EASM_OPERAND_TYPE_FLOAT;
					parameter.floatLiteral = parameterList[i].floatValue;

					break;

				case SCRIPT_PARAMETER_TYPE_STRING:
					parameter.type = EASM_OPERAND_TYPE_STRING;
					parameter.stringLiteral = parameterList[i].stringValue;

					break;
				}

				target->setStackValue(target->m_stack.topIndex - 2 - i, &parameter);
			}
		}

		target->m_instructionStream.currentInstruction = destinationFunction.entryPoint;

		int counter = 0;

		while(target->m_functionDepth != initialFunctionDepth)
		{
			message("Loop : " + INT_TO_STRING(counter));

			counter++;

			int initialInstructionIndex = target->m_instructionStream.currentInstruction;

			target->handleCurrentInstruction();

			if(target->m_instructionStream.currentInstruction == initialInstructionIndex)
			{
				target->m_instructionStream.currentInstruction++;
			}
		}

		target->m_returnValueToHost = target->m_returnValue;

		target->m_returnValue = initialReturnValue;
		target->m_returnValueFromHost = initialHostReturnValue;
		target->m_register0 = initialRegister0;
		target->m_register1 = initialRegister1;

		Function currentFunction = target->getFunction(functionIndexValue.functionIndex);
		frameIndex = functionIndexValue.offsetIndex;

		target->m_stack.frameIndex = initialStackFrameIndex;
		target->m_stack.topIndex = initialStackTopIndex;

		target->m_instructionStream.currentInstruction = returnAddress.instructionIndex;
	}

	int getScriptReturnValueAsInt(Script *target)
	{
		return virtualMachine.getScriptReturnValueAsInt(target);
	}

	float getScriptReturnValueAsFloat(Script *target)
	{
		return virtualMachine.getScriptReturnValueAsFloat(target);
	}

	std::string getScriptReturnValueAsString(Script *target)
	{
		return virtualMachine.getScriptReturnValueAsString(target);
	}

	int EagleScriptVirtualMachine::getScriptReturnValueAsInt(void *target)
	{
		Script *script = (Script *)target;

		int m_returnValue = coerceValueToInteger(script->m_returnValueToHost);

		return m_returnValue;
	}

	float EagleScriptVirtualMachine::getScriptReturnValueAsFloat(void *target)
	{
		Script *script = (Script *)target;

		float m_returnValue = coerceValueToFloat(script->m_returnValueToHost);

		return m_returnValue;
	}

	std::string EagleScriptVirtualMachine::getScriptReturnValueAsString(void *target)
	{
		Script *script = (Script *)target;

		if(script->m_returnValueToHost.type == EASM_OPERAND_TYPE_NULL)
		{
			return "NULL";
		}

		return coerceValueToString(script->m_returnValueToHost);
	}

	int Script::getFunctionIndexByName(char *name)
	{
		for(int i = 0; i < m_functionCount; ++i)
		{
			//message(INT_TO_STRING(i) + " : " + (string)functionTable[i].name);
			if(strcmp(m_functionTable[i].name.c_str(), name) == 0) return i;
		}

		return -1;
	}

	int Script::getFunctionIndexByNameCaseInsensitive(char *name)
	{
		int index = 0;

		std::vector<Function>::iterator listIterator;
		for(listIterator = m_functionTable.begin(); listIterator != m_functionTable.end(); ++listIterator)
		{
			//message(INT_TO_STRING(i) + " : " + (string)functionTable[i].name);
			if(_stricmp((*listIterator).name.c_str(), name) == 0) return index;

			index++;
		}

		return -1;
	}

	void Script::call(char *functionName, bool isCaseSensitive, ScriptFunctionParameter *parameterList, int parameterCount)
	{
		if(!isInitialized)
		{
			return;
		}

		int functionIndex;

		if(isCaseSensitive)
		{
			functionIndex = getFunctionIndexByName(functionName);
		}
		else
		{
			functionIndex = getFunctionIndexByNameCaseInsensitive(functionName);
		}

		if(functionIndex == -1)
		{
			return;
		}

		m_stack.topIndex = m_globalDataSize;
		m_stack.frameIndex = m_globalDataSize;

		Function destinationFunction = getFunction(functionIndex);

		int frameIndex = m_stack.frameIndex;

		pushStackFrame(destinationFunction.stackFrameSize + 1);

		m_currentFunctionTopIndex = m_stack.topIndex;

		RuntimeValue functionIndexValue;
		functionIndexValue.type = EASM_OPERAND_TYPE_FUNCTION_INDEX;
		functionIndexValue.functionIndex = functionIndex;
		functionIndexValue.offsetIndex = frameIndex;
		setStackValue(m_stack.topIndex - 1, &functionIndexValue);

		int initialFunctionDepth = m_functionDepth;

		m_functionDepth++;

		m_instructionStream.currentInstruction = destinationFunction.entryPoint;

		/*if(parameterCount != destinationFunction.parameterCount)
		{
			throwError("Incorrect parameter count passed to function " + (string)functionName + "; Required parameter count : " + INT_TO_STRING(destinationFunction.parameterCount));
		}*/

		if(parameterList)
		{
			for(int i = 0; i < destinationFunction.parameterCount; ++i)
			{
				RuntimeValue parameter;

				switch(parameterList[i].type)
				{
				case SCRIPT_PARAMETER_TYPE_INT:
					parameter.type = EASM_OPERAND_TYPE_INT;
					parameter.integerLiteral = parameterList[i].intValue;

					break;

				case SCRIPT_PARAMETER_TYPE_FLOAT:
					parameter.type = EASM_OPERAND_TYPE_FLOAT;
					parameter.floatLiteral = parameterList[i].floatValue;

					break;

				case SCRIPT_PARAMETER_TYPE_STRING:
					parameter.type = EASM_OPERAND_TYPE_STRING;
					parameter.stringLiteral = parameterList[i].stringValue;

					break;
				}

				setStackValue(m_stack.topIndex - 2 - i, &parameter);
			}
		}

		while(m_functionDepth != initialFunctionDepth && m_instructionStream.currentInstruction < m_instructionStream.instructionCount)
		{
			runOneStep();
		}

		m_instructionStream.currentInstruction = 0;

		m_stack.topIndex = m_globalDataSize;
		m_stack.frameIndex = m_globalDataSize;
	}

	void Script::loadAssemblerData()
	{
		initialize();

		if(isInitialized)
		{
			cleanup();
		}

		m_stack.size = EagleAssembler::scriptHeader.stackSize;
		m_stack.elementList.reserve(m_stack.size);

		for(int i = 0; i < m_stack.size; ++i)
		{
			RuntimeValue runtimeValue;
			m_stack.elementList.push_back(runtimeValue);
		}

		m_globalDataSize = EagleAssembler::scriptHeader.globalDataSize;

		m_isInitializeFunctionPresent = EagleAssembler::scriptHeader.isInitializeFunctionPresent;
		m_initializeFunctionIndex = EagleAssembler::scriptHeader.initializeFunctionIndex;

		m_isUpdateFunctionPresent = EagleAssembler::scriptHeader.isUpdateFunctionPresent;
		m_updateFunctionIndex = EagleAssembler::scriptHeader.updateFunctionIndex;

		m_isRenderFunctionPresent = EagleAssembler::scriptHeader.isRenderFunctionPresent;
		m_renderFunctionIndex = EagleAssembler::scriptHeader.renderFunctionIndex;

		m_isEndFunctionPresent = EagleAssembler::scriptHeader.isEndFunctionPresent;
		m_endFunctionIndex = EagleAssembler::scriptHeader.endFunctionIndex;

		m_instructionStream.instructionCount = EagleAssembler::instructionStreamSize;
		m_instructionStream.instructionList.reserve(m_instructionStream.instructionCount);

		for(int i = 0; i < EagleAssembler::instructionStreamSize; ++i)
		{
			Instruction instruction;

			instruction.opcode = EagleAssembler::instructionStream[i].opcode;
			instruction.operandCount = EagleAssembler::instructionStream[i].operandCount;
			instruction.operandList.reserve(instruction.operandCount);

			EagleAssembler::Operand currentOperand;

			for(int j = 0; j < instruction.operandCount; ++j)
			{
				currentOperand = EagleAssembler::instructionStream[i].operandList[j];

				RuntimeValue runtimeOperand;
				runtimeOperand.type = currentOperand.type;

				char operandType = runtimeOperand.type;

				switch(operandType)
				{
				case EASM_OPERAND_TYPE_INT:
					runtimeOperand.integerLiteral = currentOperand.integerLiteral;

					break;

				case EASM_OPERAND_TYPE_FLOAT:
					runtimeOperand.floatLiteral = currentOperand.floatLiteral;

					break;

				case EASM_OPERAND_TYPE_STRING:
					runtimeOperand.stringLiteral = EagleAssembler::stringTable.getStringByIndex(currentOperand.stringIndex);

					break;

				case EASM_OPERAND_TYPE_INSTRUCTION_INDEX:
					runtimeOperand.instructionIndex = currentOperand.instructionIndex;

					break;

				case EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX:
					runtimeOperand.stackIndex = currentOperand.stackIndex;
					runtimeOperand.offsetIndex = 0;

					break;

				case EASM_OPERAND_TYPE_RELATIVE_STACK_INDEX:
					runtimeOperand.stackIndex = currentOperand.stackIndex;
					runtimeOperand.offsetIndex = currentOperand.offsetIndex;

					runtimeOperand.type = EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX;

					break;

				case EASM_OPERAND_TYPE_REGISTER_STACK_INDEX:
					runtimeOperand.stackIndex = currentOperand.stackIndex;
					runtimeOperand.offsetIndex = currentOperand.offsetIndex;

					break;

				case EASM_OPERAND_TYPE_FUNCTION_INDEX:
					runtimeOperand.functionIndex = currentOperand.functionIndex;

					break;

				case EASM_OPERAND_TYPE_HOST_FUNCTION_INDEX:
					runtimeOperand.hostFunctionIndex = currentOperand.hostFunctionIndex;

					break;

				case EASM_OPERAND_TYPE_REGISTER:
					runtimeOperand.registerCode = currentOperand.registerCode;

					switch(runtimeOperand.registerCode)
					{
					case ES_REGISTER_CODE_RETURN_VALUE:
						runtimeOperand.precomputedAddress = (int)&m_returnValue;

						break;

					case ES_REGISTER_CODE_HOST_RETURN_VALUE:
						runtimeOperand.precomputedAddress = (int)&m_returnValueFromHost;

						break;

					case ES_REGISTER_CODE_0:
						runtimeOperand.precomputedAddress = (int)&m_register0;

						break;

					case ES_REGISTER_CODE_1:
						runtimeOperand.precomputedAddress = (int)&m_register1;

						break;

					case ES_REGISTER_CODE_2:
						runtimeOperand.precomputedAddress = (int)&m_register2;

						break;
					}

					break;
				}

				instruction.operandList.push_back(runtimeOperand);
			}

			m_instructionStream.instructionList.push_back(instruction);
		}

		m_functionCount = EagleAssembler::functionTable.nodeCount;
		m_functionTable.reserve(m_functionCount);

		LinkedListNode *node;

		node = EagleAssembler::functionTable.head;
		for(int i = 0; i < EagleAssembler::functionTable.nodeCount; ++i)
		{
			EagleAssembler::FunctionNode *assemblerFunction = (EagleAssembler::FunctionNode *)node->data;
			Function function;

			function.entryPoint = assemblerFunction->entryPoint;
			function.parameterCount = assemblerFunction->parameterCount;
			function.localDataSize = assemblerFunction->localDataSize;
			function.name = assemblerFunction->name;
			function.stackFrameSize = function.parameterCount + function.localDataSize + 1;

			m_functionTable.push_back(function);

			node = node->next;
		}

		while(m_globalDataSize > m_stack.size - ES_STACK_REALLOCATION_SIZE)
		{
			m_stack.size += ES_STACK_REALLOCATION_SIZE;

			m_stack.elementList.reserve(m_stack.size);
			for(int i = 0; i < ES_STACK_REALLOCATION_SIZE; ++i)
			{
				RuntimeValue runtimeValue;
				
				m_stack.elementList.push_back(runtimeValue);
			}
		}

		isInitialized = 1;

		reset();
	}

	void Script::loadCompilerData()
	{
		initialize();

		if(isInitialized)
		{
			cleanup();
		}

		m_stack.size = EagleScriptCompiler::scriptHeader.stackSize;
		m_stack.elementList.reserve(m_stack.size);

		for(int i = 0; i < m_stack.size; ++i)
		{
			RuntimeValue runtimeValue;
			m_stack.elementList.push_back(runtimeValue);
		}

		m_globalDataSize = EagleScriptCompiler::scriptHeader.globalDataSize;

		m_isInitializeFunctionPresent = EagleScriptCompiler::scriptHeader.isInitializeFunctionPresent;
		m_initializeFunctionIndex = EagleScriptCompiler::scriptHeader.initializeFunctionIndex;

		m_isUpdateFunctionPresent = EagleScriptCompiler::scriptHeader.isUpdateFunctionPresent;
		m_updateFunctionIndex = EagleScriptCompiler::scriptHeader.updateFunctionIndex;

		m_isRenderFunctionPresent = EagleScriptCompiler::scriptHeader.isRenderFunctionPresent;
		m_renderFunctionIndex = EagleScriptCompiler::scriptHeader.renderFunctionIndex;

		m_isEndFunctionPresent = EagleScriptCompiler::scriptHeader.isEndFunctionPresent;
		m_endFunctionIndex = EagleScriptCompiler::scriptHeader.endFunctionIndex;

		m_functionCount = EagleScriptCompiler::functionTable.nodeCount;
		m_functionTable.reserve(m_functionCount);

		std::unordered_map<int, int> jumpTargetMap;

		int instructionCounter = 0;

		LinkedListNode *node;

		node = EagleScriptCompiler::functionTable.head;

		for(int i = 0; i < EagleScriptCompiler::functionTable.nodeCount; ++i)
		{
			EagleScriptCompiler::FunctionNode *iCodeFunction = (EagleScriptCompiler::FunctionNode *)node->data;

			Function function;

			function.entryPoint = instructionCounter;
			function.parameterCount = iCodeFunction->parameterCount;
			function.localDataSize = iCodeFunction->localDataSize;
			function.name = iCodeFunction->name;
			function.stackFrameSize = function.parameterCount + function.localDataSize + 1;

			m_functionTable.push_back(function);

			m_functionCount++;

			int lastInstructionCounter = instructionCounter;

			LinkedListNode *node0 = iCodeFunction->iCodeStream.head;

			for(int j = 0; j < iCodeFunction->iCodeStream.nodeCount; ++j)
			{
				EagleScriptCompiler::ICodeNode *currentICodeNode = (EagleScriptCompiler::ICodeNode *)node0->data;

				if(currentICodeNode->type == ES_ICODE_NODE_TYPE_SOURCE_LINE)
				{
					node0 = node0->next;
					if(!node0)
					{
						break;
					}

					continue;
				}
				else if(currentICodeNode->type == ES_ICODE_NODE_TYPE_JUMP_TARGET)
				{
					jumpTargetMap[currentICodeNode->jumpTargetIndex] = instructionCounter;

					node0 = node0->next;
					if(!node0)
					{
						break;
					}

					continue;
				}

				instructionCounter++;

				node0 = node0->next;
				if(!node0)
				{
					break;
				}
			}

			instructionCounter = lastInstructionCounter;

			node0 = iCodeFunction->iCodeStream.head;

			for(int j = 0; j < iCodeFunction->iCodeStream.nodeCount; ++j)
			{
				EagleScriptCompiler::ICodeNode *currentICodeNode = (EagleScriptCompiler::ICodeNode *)node0->data;

				if(currentICodeNode->type == ES_ICODE_NODE_TYPE_SOURCE_LINE)
				{
					node0 = node0->next;
					if(!node0)
					{
						break;
					}

					continue;
				}
				else if(currentICodeNode->type == ES_ICODE_NODE_TYPE_JUMP_TARGET)
				{
					//jumpTargetMap[currentICodeNode->jumpTargetIndex] = instructionCounter;

					node0 = node0->next;
					if(!node0)
					{
						break;
					}

					continue;
				}

				Instruction instruction;

				instruction.opcode = currentICodeNode->instruction.opcode;
				instruction.operandCount = currentICodeNode->instruction.operandList.nodeCount;
				instruction.operandList.reserve(instruction.operandCount);

				LinkedListNode *node1 = currentICodeNode->instruction.operandList.head;

				EagleScriptCompiler::ICodeOperand *currentOperand;

				for(int k = 0; k < instruction.operandCount; ++k)
				{
					currentOperand = (EagleScriptCompiler::ICodeOperand *)node1->data;

					RuntimeValue runtimeOperand;

					char operandType = currentOperand->type;

					switch(operandType)
					{
					case ES_OPERAND_TYPE_INT:
						runtimeOperand.type = EASM_OPERAND_TYPE_INT;
						runtimeOperand.integerLiteral = currentOperand->integerLiteral;

						break;

					case ES_OPERAND_TYPE_FLOAT:
						runtimeOperand.type = EASM_OPERAND_TYPE_FLOAT;
						runtimeOperand.floatLiteral = currentOperand->floatLiteral;

						break;

					case ES_OPERAND_TYPE_STRING_INDEX:
						
						runtimeOperand.type = EASM_OPERAND_TYPE_STRING;
						runtimeOperand.stringLiteral = EagleScriptCompiler::stringTable.getStringByIndex(currentOperand->stringIndex);

						break;

					case ES_OPERAND_TYPE_JUMP_TARGET_INDEX:
						runtimeOperand.type = EASM_OPERAND_TYPE_INSTRUCTION_INDEX;
						runtimeOperand.instructionIndex = jumpTargetMap[currentOperand->jumpTargetIndex];

						break;

					case ES_OPERAND_TYPE_VARIABLE:
						{
							runtimeOperand.type = EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX;
							runtimeOperand.stackIndex = currentOperand->symbolIndex;

							EagleScriptCompiler::SymbolNode *symbolNode = EagleScriptCompiler::getSymbolByIndex(currentOperand->symbolIndex);
							if(symbolNode->type == ES_SYMBOL_TYPE_PARAMETER)
							{
								//int stackIndex = -(function.localDataSize - function.parameterCount + 2 + symbolNode->parameterIndex);

								//runtimeOperand.stackIndex = stackIndex;
								runtimeOperand.stackIndex = symbolNode->stackIndex;
							}
							else
							{
								runtimeOperand.stackIndex = symbolNode->stackIndex;
							}

							break;
						}

					case ES_OPERAND_TYPE_ARRAY:
						if(currentOperand->offsetSymbolIndex == -1)
						{
							runtimeOperand.stackIndex = currentOperand->symbolIndex;
							runtimeOperand.offsetIndex = currentOperand->offset;

							runtimeOperand.type = EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX;
						}
						else
						{
							runtimeOperand.stackIndex = currentOperand->symbolIndex;
							runtimeOperand.offsetIndex = currentOperand->offsetSymbolIndex;

							runtimeOperand.type = EASM_OPERAND_TYPE_REGISTER_STACK_INDEX;
						}

						break;

						case ES_OPERAND_TYPE_FUNCTION_INDEX:
						{
							if(currentOperand->functionIndex < 0)
							{
								int hostFunctionIndex = (currentOperand->functionIndex * -1) - 1;

								runtimeOperand.type = EASM_OPERAND_TYPE_HOST_FUNCTION_INDEX;
								runtimeOperand.hostFunctionIndex = hostFunctionIndex;
							}
							else
							{
								EagleScriptCompiler::FunctionNode *operandFunction = EagleScriptCompiler::getFunctionByIndex(currentOperand->functionIndex);

								runtimeOperand.type = EASM_OPERAND_TYPE_FUNCTION_INDEX;
								runtimeOperand.functionIndex = currentOperand->functionIndex - 1;
							}

							break;
						}

					case ES_OPERAND_TYPE_REGISTER:
						runtimeOperand.type = EASM_OPERAND_TYPE_REGISTER;
						runtimeOperand.registerCode = currentOperand->registerCode;

						switch(runtimeOperand.registerCode)
						{
						case ES_REGISTER_CODE_RETURN_VALUE:
							runtimeOperand.precomputedAddress = (int)&m_returnValue;

							break;

						case ES_REGISTER_CODE_HOST_RETURN_VALUE:
							runtimeOperand.precomputedAddress = (int)&m_returnValueFromHost;

							break;

						case ES_REGISTER_CODE_0:
							runtimeOperand.precomputedAddress = (int)&m_register0;

							break;

						case ES_REGISTER_CODE_1:
							runtimeOperand.precomputedAddress = (int)&m_register1;

							break;

						case ES_REGISTER_CODE_2:
							runtimeOperand.precomputedAddress = (int)&m_register2;

							break;
						}

						break;
					}

					instruction.operandList.push_back(runtimeOperand);

					node1 = node1->next;
					if(!node1) break;
				}

				m_instructionStream.instructionList.push_back(instruction);

				instructionCounter++;

				node0 = node0->next;
				if(!node0) break;
			}

			node = node->next;
			if(!node) break;
		}

		m_instructionStream.instructionCount = instructionCounter;

		while(m_globalDataSize > m_stack.size - ES_STACK_REALLOCATION_SIZE)
		{
			m_stack.size += ES_STACK_REALLOCATION_SIZE;
		}

		m_stack.elementList.resize(m_stack.size);

		node = EagleScriptCompiler::symbolTable.head;

		while(1)
		{
			if(!node) break;

			EagleScriptCompiler::SymbolNode *symbolNode = (EagleScriptCompiler::SymbolNode *)node->data;

			if(symbolNode->scope == ES_SCOPE_GLOBAL)
			{
				EagleAssembler::GlobalVariable globalVariable;
				globalVariable.identifier = symbolNode->identifier;
				globalVariable.stackIndex = symbolNode->stackIndex;

				m_globalVariableList.push_back(globalVariable);
				m_globalVariableCount++;
			}

			node = node->next;
		}

		isInitialized = 1;

		reset();
	}

	std::string EagleVirtualMachine::EagleScriptVirtualMachine::getHostFunctionName(int index)
	{
		int arraySize = m_hostFunctionList.size();

		if(index < 0 || index > arraySize - 1)
		{
			return "";
		}

		return m_hostFunctionList[index].identifier;
	}

	int EagleVirtualMachine::EagleScriptVirtualMachine::getHostFunctionIndex(std::string identifier)
	{
		int arraySize = m_hostFunctionList.size();

		for(int i = 0; i < arraySize; ++i)
		{
			if(identifier == m_hostFunctionList[i].identifier)
			{
				return i;
			}
		}

		return -1;
	}
};
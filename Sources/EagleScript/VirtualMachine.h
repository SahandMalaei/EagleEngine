#ifndef _VIRUALMACHINE_H
#define _VIRUALMACHINE_H 1

#include "EagleScript.h"

#define EVE_ID_STRING "EagleAssembler"
#define DEFAULT_STACK_SIZE 1024
#define MAX_COERCION_STRING_LENGTH 64

#define PAUSE_END_TIME_INFINITE -1

#define SCRIPT_PARAMETER_TYPE_INT 0x0000
#define SCRIPT_PARAMETER_TYPE_FLOAT 0x0001
#define SCRIPT_PARAMETER_TYPE_STRING 0x0002

#define ES_STACK_REALLOCATION_SIZE 256

namespace EagleVirtualMachine
{
	class Script;
	class EagleScriptVirtualMachine;

	struct RuntimeValue
	{
		char type;

		short offsetIndex;

		union
		{
			int integerLiteral;
			float floatLiteral;
			int stringTableIndex;
			int stackIndex;
			int instructionIndex;
			int functionIndex;
			int hostFunctionIndex;
			int registerCode;
			int precomputedAddress;
		};

		std::string stringLiteral;

		RuntimeValue();
	};

	struct Instruction
	{
		char opcode;
		char operandCount;
		//RuntimeValue *operandList;
		std::vector<RuntimeValue> operandList;
	};

	struct InstructionStream
	{
		//Instruction *instructionList;
		std::vector<Instruction> instructionList;
		short instructionCount;

		short currentInstruction;
	};

	struct RuntimeStack
	{
		//RuntimeValue *elementList;
		std::vector<RuntimeValue> elementList;

		int size;

		int topIndex;
		int frameIndex;
	};

	struct Function
	{
		short entryPoint;
		char parameterCount;
		int localDataSize;
		int stackFrameSize;
		std::string name;
	};

	enum HostGlobalVariableType
	{
		HostGlobalVariable_Int,
		HostGlobalVariable_Float,
		HostGlobalVariable_String
	};

	struct HostGlobalVariable
	{
		void *variablePointer;
		std::string identifier;
		HostGlobalVariableType type;
		//bool isValid;

		HostGlobalVariable();
	};

	struct ScriptFunctionParameter
	{
		short type;

		union
		{
			int intValue;
			float floatValue;
		};

		std::string stringValue;
	};

	class Script
	{
	private:
		friend class EagleScriptVirtualMachine;
		friend class EagleScriptManipulator;

		int m_globalDataSize;

		bool m_isInitializeFunctionPresent;
		short m_initializeFunctionIndex;

		bool m_isUpdateFunctionPresent;
		short m_updateFunctionIndex;

		bool m_isRenderFunctionPresent;
		short m_renderFunctionIndex;

		bool m_isEndFunctionPresent;
		short m_endFunctionIndex;

		int m_functionCount;
		short m_functionDepth;

		bool isInitialized;

		RuntimeValue m_returnValue;

		InstructionStream m_instructionStream;
		RuntimeStack m_stack;
		std::vector<Function> m_functionTable;

		int m_currentFunctionTopIndex;

		// Host interface

		std::vector<EagleAssembler::GlobalVariable> m_globalVariableList;
		int m_globalVariableCount;

		// Structure interfaces

		int getOperandType(int operandIndex);

		inline RuntimeValue *resolveOperandValue(int operandIndex);

		int resolveOperandAsInteger(int operandIndex);
		float resolveOperandAsFloat(int operandIndex);
		std::string resolveOperandAsString(int operandIndex);
		int resolveOperandAsInstructionIndex(int operandIndex);
		int resolveOperandAsFunctionIndex(int operandIndex);

		int coerceValueToInteger(RuntimeValue *value);
		float coerceValueToFloat(RuntimeValue *value);
		std::string coerceValueToString(RuntimeValue *value);

		int resolveStackIndex(int index);
		int resolveOperandAbsoluteStackIndex(int index);
		int resolveOperandRelativeStackIndex(int index);

		RuntimeValue *getStackValue(int index);
		void setStackValue(int index, RuntimeValue *value);

		void pushOntoStack(RuntimeValue *value);
		RuntimeValue *popFromStack();

		void pushStackFrame(int size);
		void popStackFrame(int size);

		void copyOperandValue(RuntimeValue *destinationValue, RuntimeValue *sourceValue);

		Function getFunction(int index);

		// Life cycle

		void initialize();
		void reset();

		int runOneStep();
		void handleCurrentInstruction();

		void cleanup();

		// Host interface

		HostGlobalVariable *getHostGlobalVariable(char *identifier);
		int getHostGlobalVariableAsInt(char *identifier);
		float getHostGlobalVariableAsFloat(char *identifier);
		std::string getHostGlobalVariableAsString(char *identifier);

		RuntimeValue m_returnValueFromHost, m_returnValueToHost;

		void callHostFunction(int index);

		RuntimeValue *m_hostParameters[MAX_CALL_HOST_PARAMTETER_COUNT - 1];

		int getFunctionIndexByName(char *name);
		int getFunctionIndexByNameCaseInsensitive(char *name);

		RuntimeValue m_register0, m_register1, m_register2;

	public:
		Script();
		~Script();

		bool loadEagleScript(char *fileAddress, bool optimize = 1, bool outputEASMAndEVEFiles = 0, bool addComments = 1);
		void loadEASM(char *fileAddress, bool outputEVEFile = 1);
		void loadEVE(char *fileAddress);
		bool loadEagleScriptFromString(std::string scriptText, bool optimize = 1);

		void loadAssemblerData();
		void loadCompilerData();

		// Host interface

		int getGlobalVariableAsInt(char *identifier);
		float getGlobalVariableAsFloat(char *identifier);
		std::string getGlobalVariableAsString(char *identifier);

		RuntimeValue *getHostFunctionParameter(int index);

		void call(char *functionName, bool isCaseSensitive, ScriptFunctionParameter *parameterList = 0, int parameterCount = 0);
	};

	struct HostFunction
	{
		void (*functionPointer)(Script *);
		string identifier;
		//bool isValid;

		HostFunction();
	};

	class EagleScriptVirtualMachine
	{
	public:
		EagleScriptVirtualMachine();
		~EagleScriptVirtualMachine();

		void initialize();

		void addScript(Script *script);
		void removeScript(Script *script);
		void updateScripts();

		std::vector<Script *>m_scriptList;
		
		std::vector<HostGlobalVariable> m_hostGlobalVariableList;

		std::vector<HostFunction> m_hostFunctionList;

		std::string getHostFunctionName(int index);
		int EagleScriptVirtualMachine::getHostFunctionIndex(std::string identifier);

		void returnIntToScript(int value, void *caller);
		void returnFloatToScript(float value, void *caller);
		void returnStringToScript(std::string value, void *caller);

		void callScriptFunction(void *script, char *functionName, ScriptFunctionParameter *parameterList, int parameterCount);

		int getScriptReturnValueAsInt(void *caller);
		float getScriptReturnValueAsFloat(void *caller);
		std::string getScriptReturnValueAsString(void *caller);
	};

	void eagleScriptThreadFunction(void *parameters);

	void decryptStringTable(unsigned long securityCode, char **stringTable, int stringCount);

	extern EagleScriptVirtualMachine virtualMachine;

	int registerGlobalVariable(void *globalVariablePointer, char *identifier, HostGlobalVariableType type);
	void unregisterGlobalVariable(char *identifier);

	int registerHostFunction(void (*functionPointer)(Script *), char *identifier);
	void unregisterHostFunction(char *identifier);

	int getScriptParameterAsInt(int index, Script *caller);
	float getScriptParameterAsFloat(int index, Script *caller);
	std::string getScriptParameterAsString(int index, Script *caller);

	void returnIntToScript(int value, Script *caller);
	void returnFloatToScript(float value, Script *caller);
	void returnStringToScript(std::string value, Script *caller);

	void callScriptFunction(Script *script, char *functionName, ScriptFunctionParameter *parameterList, int parameterCount);

	int getScriptReturnValueAsInt(Script *caller);
	float getScriptReturnValueAsFloat(Script *caller);
	std::string getScriptReturnValueAsString(Script *caller);
};

#endif
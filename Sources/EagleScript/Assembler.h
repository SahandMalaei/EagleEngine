#ifndef _EAGLE_ASSEMBLER_H
#define _EAGLE_ASSEMBLER_H 1

#include "EagleScript.h"

#define EASM_ID_STRING "EagleAssembler"
#define EASM_VERSION_MAJOR 0
#define EASM_VERSION_MINOR 1

#define EASM_MAX_LINE_LENGTH 256
#define EASM_MAX_IDENTIFIER_LENGTH 64
#define MAX_INSTRUCTION_LOOKUP_COUNT 256
#define MAX_INSTRUCTION_MNEMONIC_LENGTH 16

#define MAIN_FUNCTION_NAME "Main"

#define MAX_ARRAY_DIMENSION_COUNT 32

#define MAX_OPERAND_COUNT 64

#define ES_MAX_FUNCTION_PARAMETER_COUNT 16
#define MAX_CALL_HOST_PARAMTETER_COUNT 16

#define EASM_OPERAND_TYPE_NULL 0x0000
#define EASM_OPERAND_TYPE_INT 0x0001
#define EASM_OPERAND_TYPE_FLOAT 0x0002
#define EASM_OPERAND_TYPE_STRING 0x0003
#define EASM_OPERAND_TYPE_ABSOLUTE_STACK_INDEX 0x0004
#define EASM_OPERAND_TYPE_RELATIVE_STACK_INDEX 0x0005
#define EASM_OPERAND_TYPE_INSTRUCTION_INDEX 0x0006
#define EASM_OPERAND_TYPE_FUNCTION_INDEX 0x0007
#define EASM_OPERAND_TYPE_HOST_FUNCTION_INDEX 0x0008
#define EASM_OPERAND_TYPE_REGISTER 0x0009
#define EASM_OPERAND_TYPE_REGISTER_STACK_INDEX 0x000A

#define OPERAND_FLAG_TYPE_INT 1
#define OPERAND_FLAG_TYPE_FLOAT 2
#define OPERAND_FLAG_TYPE_STRING 4
#define OPERAND_FLAG_TYPE_MEMORY_REFRENCE 8
#define OPERAND_FLAG_TYPE_LINELABEL 16
#define OPERAND_FLAG_TYPE_FUNCTION_NAME 32
#define OPERAND_FLAG_TYPE_HOST_API_CALL 64
#define OPERAND_FLAG_TYPE_REGISTER 128

#define END_OF_TOKEN_STREAM -0x0001
#define TOKEN_TYPE_INVALID -0x0002

// Revision 0 token types

#define TOKEN_TYPE_INT 0x0000
#define TOKEN_TYPE_FLOAT 0x0001
#define TOKEN_TYPE_STRING 0x0002
#define TOKEN_TYPE_DOUBLE_QUOTE 0x0003
#define TOKEN_TYPE_IDENTIFIER 0x0004
#define TOKEN_TYPE_COLON 0x0005
#define TOKEN_TYPE_OPEN_BRACKET 0x0006
#define TOKEN_TYPE_CLOSE_BRACKET 0x0007
#define TOKEN_TYPE_COMMA 0x0008
#define TOKEN_TYPE_OPEN_BRACE 0x0009
#define TOKEN_TYPE_CLOSE_BRACE 0x000A
#define TOKEN_TYPE_NEW_LINE 0x000B
#define TOKEN_TYPE_INSTRUCTION 0x000C
#define TOKEN_TYPE_SET_STACK_SIZE 0x000D
#define TOKEN_TYPE_VARIABLE 0x000E
#define TOKEN_TYPE_FUNCTION 0x000F
#define TOKEN_TYPE_RETURN_VALUE_REGISTER 0x0010

// Revision 1 token types

#define TOKEN_TYPE_REGISTER_HOST_RETURN_VALUE 0x0011
#define TOKEN_TYPE_REGISTER_0 0x0012
#define TOKEN_TYPE_REGISTER_1 0x0013
#define TOKEN_TYPE_REGISTER_2 0x0014

#define ES_REGISTER_CODE_RETURN_VALUE 0x0000
#define ES_REGISTER_CODE_HOST_RETURN_VALUE 0x0001
#define ES_REGISTER_CODE_0 0x0002
#define ES_REGISTER_CODE_1 0x0003
#define ES_REGISTER_CODE_2 0x0004

#define LEXER_STATE_NO_STRING 0x0000
#define LEXER_STATE_IN_STRING 0x0001
#define LEXER_STATE_END_STRING 0x0002

#define OPERAND_POSITION_SOURCE 1
#define OPERAND_POSITION_DESTINATION 0

#define EASM_INSTRUCTION_COUNT 45

// Revision 0 instructions

#define ES_OPCODE_MOV 0x0000
#define ES_OPCODE_ADD 0x0001
#define ES_OPCODE_SUB 0x0002
#define ES_OPCODE_MUL 0x0003
#define ES_OPCODE_DIV 0x0004
#define ES_OPCODE_MOD 0x0005
#define ES_OPCODE_EXP 0x0006
#define ES_OPCODE_NEG 0x0007
#define ES_OPCODE_INC 0x0008
#define ES_OPCODE_DEC 0x0009
#define ES_OPCODE_AND 0x000A
#define ES_OPCODE_OR 0x000B
#define ES_OPCODE_XOR 0x000C
#define ES_OPCODE_NOT 0x000D
#define ES_OPCODE_SHL 0x000E
#define ES_OPCODE_SHR 0x000F
#define ES_OPCODE_CONCAT 0x0010
#define ES_OPCODE_GETCHAR 0x0011
#define ES_OPCODE_SETCHAR 0x0012
#define ES_OPCODE_JMP 0x0013
#define ES_OPCODE_JE 0x0014
#define ES_OPCODE_JNE 0x0015
#define ES_OPCODE_JG 0x0016
#define ES_OPCODE_JL 0x0017
#define ES_OPCODE_JGE 0x0018
#define ES_OPCODE_JLE 0x0019
#define ES_OPCODE_PUSH 0x001A
#define ES_OPCODE_POP 0x001B
#define ES_OPCODE_CALL 0x001C
#define ES_OPCODE_RETURN 0x001D
#define ES_OPCODE_CALLHOST 0x001E

#define ES_OPCODE_GETHOSTGLOBAL 0x0023

namespace EagleAssembler
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

	struct Operand
	{
		char type;

		union
		{
			int integerLiteral;
			float floatLiteral;
			int stringIndex;
			int stackIndex;
			int instructionIndex;
			int functionIndex;
			int hostFunctionIndex;
			int registerCode;
		};

		short offsetIndex;
	};

	struct Instruction
	{
		int opcode;
		int operandCount;
		Operand *operandList;
	};

	typedef int OperandType;

	struct InstructionLookup
	{
		char mnemonic[MAX_INSTRUCTION_MNEMONIC_LENGTH + 1];
		int opcode;
		int operandCount;
		OperandType *operandList;
	};

	struct FunctionNode
	{
		int index;
		char name[EASM_MAX_IDENTIFIER_LENGTH];
		int entryPoint;
		int parameterCount;
		int localDataSize;
	};

	struct SymbolNode
	{
		int index;
		char identifier[EASM_MAX_IDENTIFIER_LENGTH];
		int size;
		int stackIndex;
		int functionIndex;
	};

	struct LabelNode
	{
		int index;
		char identifier[EASM_MAX_IDENTIFIER_LENGTH];
		int targetInstructionIndex;
		int functionIndex;
	};

	struct GlobalVariable
	{
		std::string identifier;
		int stackIndex;
	};

	int addToStringTable(char *s);

	FunctionNode *getFunctionByName(char *name);
	int addToFunctionTable(char *name, int entryPoint);
	void setFunctionInformation(char *name, int parameterCount, int localDataSize);

	SymbolNode *getSymbolByIdentifier(char *identifier, int functionIndex);
	int addToSymbolTable(char *identifier, int size, int stackIndex, int functionIndex);
	int getSymbolStackIndexByIdentifier(char *identifier, int functionIndex);
	int getSymbolSizeByIdentifier(char *identifier, int functionIndex);

	LabelNode *getLabelByIdentifier(char *identifier, int functionIndex);
	int addToLabelTable(char *identifier, int targetInstructionIndex, int functionIndex);

	int addInstructionLookup(char *mnemonic, int opcode, int operandCount);
	void setOperandType(int instructionIndex, int operandIndex, OperandType operandType);
	InstructionLookup *getInstructionByMnemonic(char *mnemonic);
	void initializeInstructionTable();

	void initializeSecondPass();

	// Lexing and Tokenizing

	typedef int Token;

	void stripComments(char *sourceLine);
	void trimWhitespace(char *s);
	void replaceEmptyStrings(char *s);
	void removeParentheses(char *s);

	struct Lexer
	{
		int index0, index1;
		int currentLine;
		int stringState;
		Token currentToken;
		char currentLexeme[EASM_MAX_IDENTIFIER_LENGTH];
	};

	Token getNextToken();
	int skipToNextLine();
	int goToPreviousLine();
	void resetLexer();
	char getLookAheadChar();

	// Debugging and Error handling

	void throwCodeError(std::string text, int line);

	void outputStatistics();

	// Parsing

	void initializeParser();
	void parseSourceCode();

	// File output

	void encryptStringTable();
	void outputAssembledEVE(char *outputFileAddress);
	void loadEASMFile(char *fileAddress);

	// Cleaning up

	void cleanup();

	// External variables

	extern char **sourceCode;
	extern int sourceCodeLineCount;

	extern InstructionLookup instructionTable[MAX_INSTRUCTION_LOOKUP_COUNT];

	extern Instruction *instructionStream;
	extern int instructionStreamSize;

	extern ScriptHeader scriptHeader;

	extern EagleScript::LinkedList stringTable;
	extern EagleScript::LinkedList functionTable;
	extern EagleScript::LinkedList symbolTable;
	extern EagleScript::LinkedList labelTable;

	extern Lexer lexer;

	extern bool isSetStackSizeFound;

	extern bool isLocalScopeActive;

	extern int currentFunctionIndex;
	extern char currentFunctionName[EASM_MAX_IDENTIFIER_LENGTH];
	extern int currentFunctionParameterCount;
	extern int currentFunctionLocalDataSize;

	extern InstructionLookup currentInstruction;
	extern int currentInstructionIndex;

	extern std::vector<GlobalVariable> globalVariableList;
	extern short globalVariableCount;
};

#endif
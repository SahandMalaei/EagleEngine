#ifndef _ICODE_H
#define _ICODE_H 1

#include "../EagleScript.h"

using namespace EagleScript;

#define ES_OPERAND_TYPE_NULL 0x0000
#define ES_OPERAND_TYPE_INT 0x0001
#define ES_OPERAND_TYPE_FLOAT 0x0002
#define ES_OPERAND_TYPE_STRING_INDEX 0x0003
#define ES_OPERAND_TYPE_VARIABLE 0x0004
#define ES_OPERAND_TYPE_ARRAY 0x0005
#define ES_OPERAND_TYPE_JUMP_TARGET_INDEX 0x0006
#define ES_OPERAND_TYPE_FUNCTION_INDEX 0x0007
#define ES_OPERAND_TYPE_REGISTER 0x0008

#define ES_ICODE_NODE_TYPE_INSTRUCTION 0x0000
#define ES_ICODE_NODE_TYPE_JUMP_TARGET 0x0001
#define ES_ICODE_NODE_TYPE_SOURCE_LINE 0x0002

namespace EagleScriptCompiler
{
	struct ICodeInstruction
	{
		int opcode;
		LinkedList operandList;

		ICodeInstruction();
	};

	struct ICodeOperand
	{
		int type;

		union
		{
			int integerLiteral;
			float floatLiteral;
			int stringIndex;
			int symbolIndex;
			int jumpTargetIndex;
			int functionIndex;
			int registerCode;
		};

		int offset;
		int offsetSymbolIndex;

		bool operator == (const ICodeOperand &operand);
	};

	struct ICodeNode
	{
		int type;

		ICodeInstruction instruction;

		union
		{
			int jumpTargetIndex;

			char *sourceLineString;
		};
	};

	int addICodeInstruction(int functionIndex, int opcode);
	int insertICodeInstruction(int functionIndex, int instructionIndex, int opcode);
	int deleteICodeInstruction(int functionIndex, int instructionIndex);
	void addICodeOperand(int functionIndex, int instructionIndex, ICodeOperand operandValue);

	void addIntegerICodeOperand(int functionIndex, int instructionIndex, int value);
	void addFloatICodeOperand(int functionIndex, int instructionIndex, float value);
	void addStringIndexICodeOperand(int functionIndex, int instructionIndex, int value);
	void addSymbolIndexICodeOperand(int functionIndex, int instructionIndex, int value);
	void addJumpTargetIndexICodeOperand(int functionIndex, int instructionIndex, int index);
	void addFunctionIndexICodeOperand(int functionIndex, int instructionIndex, int value);
	void addRegisterCodeICodeOperand(int functionIndex, int instructionIndex, int code);
	void addArrayICodeOperand(int functionIndex, int instructionIndex, int symbolIndex);
	void addArrayICodeOperand1(int functionIndex, int instructionIndex, int symbolIndex);
	void addArrayIndexICodeOperand(int functionIndex, int instructionIndex, int symbolIndex, int arrayIndex);
	void addFunctionParameterListICodeOperand(int functionIndex, int instructionIndex, int index);

	ICodeOperand *getICodeOperandByIndex(ICodeNode *instruction, int operandIndex);

	void addICodeJumpTarget(int functionIndex, int targetIndex);
	int getNextICodeJumpTargetIndex();

	void addICodeSourceLine(int functionIndex, char *sourceLineString);

	ICodeNode *getICodeNodeByImplicitIndex(int functionIndex, int instructionIndex);
};

#endif
#include "../EagleScript.h"

using namespace EagleScript;

namespace EagleScriptCompiler
{
	int currentJumpTargetIndex = 0;

	ICodeInstruction::ICodeInstruction()
	{
		opcode = 0;
	}

	int addICodeInstruction(int functionIndex, int opcode)
	{
		FunctionNode *function = getFunctionByIndex(functionIndex);

		ICodeNode *instructionNode = new ICodeNode();
		instructionNode->type = ES_ICODE_NODE_TYPE_INSTRUCTION;
		instructionNode->instruction.opcode = opcode;
		instructionNode->instruction.operandList.nodeCount = 0;

		return function->iCodeStream.addNode(instructionNode);
	}

	int insertICodeInstruction(int functionIndex, int instructionIndex, int opcode)
	{
		FunctionNode *function = getFunctionByIndex(functionIndex);

		ICodeNode *instructionNode = new ICodeNode();
		instructionNode->type = ES_ICODE_NODE_TYPE_INSTRUCTION;
		instructionNode->instruction.opcode = opcode;
		instructionNode->instruction.operandList.nodeCount = 0;

		return function->iCodeStream.insertNode(instructionNode, instructionIndex);
	}

	int deleteICodeInstruction(int functionIndex, int instructionIndex)
	{
		FunctionNode *function = getFunctionByIndex(functionIndex);
		ICodeNode *node = (ICodeNode *)function->iCodeStream.findNode(instructionIndex)->data;

		node->instruction.operandList.cleanup();

		function->iCodeStream.deleteNode(instructionIndex);

		return 1;
	}

	void addICodeOperand(int functionIndex, int instructionIndex, ICodeOperand operandValue)
	{
		ICodeNode *instruction = getICodeNodeByImplicitIndex(functionIndex, instructionIndex);

		ICodeOperand *operand = new ICodeOperand();
		*operand = operandValue;

		instruction->instruction.operandList.addNode(operand);
	}

	void addIntegerICodeOperand(int functionIndex, int instructionIndex, int value)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_INT;
		operand.integerLiteral = value;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addFloatICodeOperand(int functionIndex, int instructionIndex, float value)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_FLOAT;
		operand.floatLiteral = value;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addStringIndexICodeOperand(int functionIndex, int instructionIndex, int value)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_STRING_INDEX;
		operand.stringIndex = value;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addSymbolIndexICodeOperand(int functionIndex, int instructionIndex, int value)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_VARIABLE;
		operand.symbolIndex = value;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addJumpTargetIndexICodeOperand(int functionIndex, int instructionIndex, int index)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_JUMP_TARGET_INDEX;
		operand.jumpTargetIndex = index;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addFunctionIndexICodeOperand(int functionIndex, int instructionIndex, int value)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_FUNCTION_INDEX;
		operand.functionIndex = value;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addRegisterCodeICodeOperand(int functionIndex, int instructionIndex, int code)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_REGISTER;
		operand.registerCode = code;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addArrayICodeOperand(int functionIndex, int instructionIndex, int symbolIndex)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_ARRAY;
		operand.symbolIndex = symbolIndex;
		operand.offsetSymbolIndex = ES_REGISTER_CODE_0;
		operand.offset = 0;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addArrayICodeOperand1(int functionIndex, int instructionIndex, int symbolIndex)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_ARRAY;
		operand.symbolIndex = symbolIndex;
		operand.offsetSymbolIndex = ES_REGISTER_CODE_1;
		operand.offset = 0;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	
	void addArrayIndexICodeOperand(int functionIndex, int instructionIndex, int symbolIndex, int arrayIndex)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_ARRAY;
		operand.symbolIndex = symbolIndex;
		operand.offsetSymbolIndex = -1;
		operand.offset = arrayIndex;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	void addFunctionParameterListICodeOperand(int functionIndex, int instructionIndex, int index)
	{
		ICodeOperand operand;
		operand.type = ES_OPERAND_TYPE_ARRAY;
		operand.symbolIndex = functionParameterListIndex;
		operand.offsetSymbolIndex = -1;
		operand.offset = index;

		addICodeOperand(functionIndex, instructionIndex, operand);
	}

	ICodeOperand *getICodeOperandByIndex(ICodeNode *instruction, int operandIndex)
	{
		if(instruction->instruction.operandList.nodeCount == 0) return 0;

		LinkedListNode *currentNode = instruction->instruction.operandList.head;

		for(int i = 0; i < instruction->instruction.operandList.nodeCount; ++i)
		{
			if(i == operandIndex)
			{
				return (ICodeOperand *)currentNode->data;
			}

			currentNode = currentNode->next;
		}

		return 0;
	}

	void addICodeJumpTarget(int functionIndex, int targetIndex)
	{
		FunctionNode *function = getFunctionByIndex(functionIndex);

		ICodeNode *sourceLineNode = new ICodeNode();
		sourceLineNode->type = ES_ICODE_NODE_TYPE_JUMP_TARGET;
		sourceLineNode->jumpTargetIndex = targetIndex;

		function->iCodeStream.addNode(sourceLineNode);
	}

	int getNextICodeJumpTargetIndex()
	{
		return currentJumpTargetIndex++;
	}

	void addICodeSourceLine(int functionIndex, char *sourceLineString)
	{
		if(functionIndex < 1 || functionIndex > functionTable.nodeCount)
		{
			return;
		}

		FunctionNode *function = getFunctionByIndex(functionIndex);

		ICodeNode *sourceLineNode = new ICodeNode();
		sourceLineNode->type = ES_ICODE_NODE_TYPE_SOURCE_LINE;
		sourceLineNode->sourceLineString = sourceLineString;

		//Debug::throwMessage("Function index : " + INT_TO_STRING(functionIndex) + ", Source line string : " + (string)sourceLineString);

		function->iCodeStream.addNode(sourceLineNode);
	}

	ICodeNode *getICodeNodeByImplicitIndex(int functionIndex, int instructionIndex)
	{
		FunctionNode *function = getFunctionByIndex(functionIndex);

		if(function->iCodeStream.nodeCount == 0)
		{
			return 0;
		}

		LinkedListNode *currentNode = function->iCodeStream.head;

		for(int i = 0; i < function->iCodeStream.nodeCount; ++i)
		{
			if(i == instructionIndex)
			{
				return (ICodeNode *)currentNode->data;
			}

			currentNode = currentNode->next;
		}
		
		return 0;
	}

	bool ICodeOperand::operator == (const ICodeOperand &operand)
	{
		if(type != operand.type)
		{
			return 0;
		}

		switch(type)
		{
		case ES_OPERAND_TYPE_INT:
			if(integerLiteral != operand.integerLiteral)
			{
				return 0;
			}

			return 1;

			break;

		case ES_OPERAND_TYPE_FLOAT:
			if(!approximateEquals(floatLiteral, operand.floatLiteral))
			{
				return 0;
			}

			return 1;

			break;

		case ES_OPERAND_TYPE_REGISTER:
			if(registerCode != operand.registerCode)
			{
				return 0;
			}

			return 1;

			break;

		case ES_OPERAND_TYPE_ARRAY:
			if(offsetSymbolIndex == -1)
			{
				if(symbolIndex != operand.symbolIndex)
				{
					return 0;
				}
				else if(offset != operand.offset)
				{
					return 0;
				}
			}
			else
			{
				if(symbolIndex != operand.symbolIndex)
				{
					return 0;
				}
				else if(offsetSymbolIndex != operand.offsetSymbolIndex)
				{
					return 0;
				}
			}

			return 1;

			break;

		case ES_OPERAND_TYPE_VARIABLE:
			if(symbolIndex != operand.symbolIndex)
			{
				return 0;
			}

			return 1;

			break;

		default:
			return 0;

			break;
		}
	}
};
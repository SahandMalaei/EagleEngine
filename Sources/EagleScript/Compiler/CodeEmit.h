#ifndef _CODEEMIT_H
#define _CODEEEMIT_H 1

#include "../EagleScript.h"

using namespace EagleScript;

#define ES_CODE_EMIT_TAB_WIDTH 8

namespace EagleScriptCompiler
{
	void optimizeICode();

	ICodeOperand precomputeAdd(ICodeOperand operand0, ICodeOperand operand1);
	ICodeOperand precomputeSubtract(ICodeOperand operand0, ICodeOperand operand1);
	ICodeOperand precomputeMultiply(ICodeOperand operand0, ICodeOperand operand1);
	ICodeOperand precomputeDivide(ICodeOperand operand0, ICodeOperand operand1);
	ICodeOperand precomputeExponent(ICodeOperand operand0, ICodeOperand operand1);

	void emitEASMCode(char *fileAddress, bool addComments);

	void emitEASMHeader(char *fileAddress, bool addComments);
	void emitEASMDirectives(bool addComments);
	void emitEASMScopeVariables(int scope, bool addComments);
	void emitEASMFunction(FunctionNode *function, bool addComments);
};

#endif
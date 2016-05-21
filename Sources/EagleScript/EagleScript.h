#ifndef _EAGLESCRIPT_H
#define _EAGLESCRIPT_H 1

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#include "../Debug.h"

#include "Generic.h"
#include "Assembler.h"
#include "VirtualMachine.h"
#include "Compiler/Compiler.h"
#include "Compiler/Tables.h"
#include "Compiler/Lexer.h"
#include "Compiler/DefaultFunctions.h"
#include "Compiler/Parser.h"
#include "Compiler/ICode.h"
#include "Compiler/CodeEmit.h"

using namespace EagleVirtualMachine;

namespace EagleScript
{
	struct ScriptHeader
	{
		int stackSize;
		int globalDataSize;
		bool isMainFunctionPresent;
		short mainFunctionIndex;
	};

	int initializeEagleScript();
};

#endif
#ifndef _LEXER_H
#define _LEXER_H 1

#define ES_LEXER_STATE_START 0x0000
#define ES_LEXER_STATE_INT 0x0001
#define ES_LEXER_STATE_FLOAT 0x0002
#define ES_LEXER_STATE_IDENTIFIER 0x0003
#define ES_LEXER_STATE_DELIMITER 0x0004
#define ES_LEXER_STATE_STRING 0x0005
#define ES_LEXER_STATE_STRING_ESCAPE 0x0006
#define ES_LEXER_STATE_STRING_CLOSE_QUOTE 0x0007
#define ES_LEXER_STATE_OPERATOR 0x0008
#define ES_LEXER_STATE_DIRECTIVE 0x0009

#define ES_TOKEN_TYPE_END_OF_STREAM -0x0001
#define ES_TOKEN_TYPE_INVALID -0x0002

#define ES_TOKEN_TYPE_INT 0x0000
#define ES_TOKEN_TYPE_FLOAT 0x0001
#define ES_TOKEN_TYPE_IDENTIFIER 0x0002
#define ES_TOKEN_TYPE_RESERVED_VARIABLE 0x0003
#define ES_TOKEN_TYPE_RESERVED_TRUE 0x0004
#define ES_TOKEN_TYPE_RESERVED_FALSE 0x0005
#define ES_TOKEN_TYPE_RESERVED_IF 0x0006
#define ES_TOKEN_TYPE_RESERVED_ELSE 0x0007
#define ES_TOKEN_TYPE_RESERVED_SWITCH 0x001A
#define ES_TOKEN_TYPE_RESERVED_CASE 0x001B
#define ES_TOKEN_TYPE_RESERVED_DEFAULT 0x001C
#define ES_TOKEN_TYPE_RESERVED_BREAK 0x0008
#define ES_TOKEN_TYPE_RESERVED_CONTINUE 0x0009
#define ES_TOKEN_TYPE_RESERVED_FOR 0x000A
#define ES_TOKEN_TYPE_RESERVED_WHILE 0x000B
#define ES_TOKEN_TYPE_RESERVED_FUNCTION 0x000C
#define ES_TOKEN_TYPE_RESERVED_RETURN 0x000D
#define ES_TOKEN_TYPE_RESERVED_GOTO 0x0001D
#define ES_TOKEN_TYPE_DELIMITER_COMMA 0x000E
#define ES_TOKEN_TYPE_DELIMITER_OPEN_PARENTHESE 0x000F
#define ES_TOKEN_TYPE_DELIMITER_CLOSE_PARENTHESE 0x0010
#define ES_TOKEN_TYPE_DELIMITER_OPEN_BRACE 0x0011
#define ES_TOKEN_TYPE_DELIMITER_CLOSE_BRACE 0x0012
#define ES_TOKEN_TYPE_DELIMITER_OPEN_CURLY_BRACE 0x0013
#define ES_TOKEN_TYPE_DELIMITER_CLOSE_CURLY_BRACE 0x0014
#define ES_TOKEN_TYPE_DELIMITER_SEMICOLON 0x0015
#define ES_TOKEN_TYPE_DELIMITER_COLON 0x0018
#define ES_TOKEN_TYPE_STRING 0x0016
#define ES_TOKEN_TYPE_OPERATOR 0x0017

//#define ES_TOKEN_TYPE_RESERVED_PRINT 0x0018
//#define ES_TOKEN_TYPE_RESERVED_INPUT 0x0019

//#define ES_TOKEN_TYPE_DIRECTIVE_INCLUDE 0x001B

#define ES_MAX_DELIMITER_COUNT 32

#define ES_MAX_OPERATOR_STATE_COUNT 32

#define ES_OPERATOR_TYPE_ADD 0x0000 // +
#define ES_OPERATOR_TYPE_SUBTRACT 0x0001 // -
#define ES_OPERATOR_TYPE_MULTIPLY 0x0002 // *
#define ES_OPERATOR_TYPE_DIVIDE 0x0003 // /
#define ES_OPERATOR_TYPE_MOD 0x0004 // %
#define ES_OPERATOR_TYPE_EXPONENT 0x0005 // ^
#define ES_OPERATOR_TYPE_INCREMENT 0x000F // ++
#define ES_OPERATOR_TYPE_DECREMENT 0x0011 // --

#define ES_OPERATOR_TYPE_ASSIGN 0x000B // =
#define ES_OPERATOR_TYPE_ASSIGN_ADD 0x000E // +=
#define ES_OPERATOR_TYPE_ASSIGN_SUBTRACT 0x0010 // -=
#define ES_OPERATOR_TYPE_ASSIGN_MULTIPLY 0x0012 // *=
#define ES_OPERATOR_TYPE_ASSIGN_DIVIDE 0x0013 // /=
#define ES_OPERATOR_TYPE_ASSIGN_MOD 0x0014 // %=
#define ES_OPERATOR_TYPE_ASSIGN_EXPONENT 0x0015 // ^=

#define ES_OPERATOR_TYPE_BITWISE_AND 0x0006 // &
#define ES_OPERATOR_TYPE_BITWISE_OR 0x0007 // |
#define ES_OPERATOR_TYPE_BITWISE_XOR 0x0008 // #
#define ES_OPERATOR_TYPE_BITWISE_NOT 0x0009 // ~
#define ES_OPERATOR_TYPE_BITWISE_SHIFT_LEFT 0x001E // <<
#define ES_OPERATOR_TYPE_BITWISE_SHIFT_RIGHT 0x0020 // >>

#define ES_OPERATOR_TYPE_BITWISE_ASSIGN_AND 0x0016 // &=
#define ES_OPERATOR_TYPE_BITWISE_ASSIGN_OR 0x0018 // |=
#define ES_OPERATOR_TYPE_BITWISE_ASSIGN_XOR 0x001A // #=
#define ES_OPERATOR_TYPE_BITWISE_ASSIGN_SHIFT_LEFT 0x0021 // <<=
#define ES_OPERATOR_TYPE_BITWISE_ASSIGN_SHIFT_RIGHT 0x0022 // >>=

#define ES_OPERATOR_TYPE_LOGICAL_AND 0x0017 // &&
#define ES_OPERATOR_TYPE_LOGICAL_OR 0x0019 // ||
#define ES_OPERATOR_TYPE_LOGICAL_NOT 0x000A // !

#define ES_OPERATOR_TYPE_EQUAL 0x001C // ==
#define ES_OPERATOR_TYPE_NOT_EQUAL 0x001B // !=
#define ES_OPERATOR_TYPE_LESS 0x000C // <
#define ES_OPERATOR_TYPE_GREATER 0x000D // >
#define ES_OPERATOR_TYPE_LESS_EQUAL 0x001D // <=
#define ES_OPERATOR_TYPE_GREATER_EQUAL 0x001F // >=

namespace EagleScriptCompiler
{
	typedef int Token;

	struct LexerState
	{
		char currentLexeme[128];
		int currentLexemeStart;
		int currentLexemeEnd;

		int lexerState;

		int operatorIndex;

		int currentLineIndex;
		LinkedListNode *currentLineNode;

		Token currentToken;

		LexerState();
	};

	extern LexerState currentLexerState;
	extern LexerState previousLexerState;

	struct OperatorState
	{
		char character;

		int substateIndex;
		int substateCount;

		int index;

		void setProperties(char character, int substateIndex, int substateCount, int index){this->character = character; this->substateIndex = substateIndex; this->substateCount = substateCount; this->index = index;}
	};

	void resetEagleScriptLexer();
	int loadESFile(char *fileAddress);
	Token getNextToken();
	char getNextCharacter();
	char getLookAheadChar();
	char *getCurrentLexeme();
	void copyCurrentLexeme(char *destinationString);
	Token getCurrentToken();

	char *getCurrentSourceLine();
	int getCurrentSourceLineIndex();
	int getCurrentLexemeStartIndex();
	int getCurrentLexemeEndIndex();

	void throwCodeError(std::string errorMessage);

	bool ES_isCharWhitespace(char c);
	bool ES_isCharDelimiter(char c);

	bool ES_isStringWhitespace(char *s);

	int getCurrentOperatorIndex();
	bool ES_isCharOperator(char c, int index);
	int getOperatorStateIndex(char c, int characterIndex, int substateIndex, int substateCount);
	OperatorState getOperatorState(int characterIndex, int stateIndex);
	string getCurrentOperatorString();

	void copyLexerState(LexerState *destinationState, LexerState sourceState);
	void rewindTokenStream();
	extern int sourceCodeStreamSize;
};

#endif
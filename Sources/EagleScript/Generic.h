#ifndef _GENERIC_H
#define _GENERIC_H 1

#include <string>

#define FLOAT_TO_STRING(s) std::to_string((long float)s)
#define LONG_TO_STRING(s) std::to_string((long)s)
#define INT_TO_STRING(s) std::to_string((int)s)

#define IS_STRING_VALID(s) (std::string(s).length())

namespace EagleScript
{
	struct LinkedListNode
	{
		void *data;
		LinkedListNode *next;
	};

	class LinkedList
	{
	public:
		LinkedList();
		~LinkedList();

		int nodeCount;
		LinkedListNode *head, *tail;

		int addNode(void *data);
		int insertNode(void *data, int index);
		void deleteNode(LinkedListNode *node);
		void deleteNode(int index);

		LinkedListNode *findNode(int index);

		int addStringNode(char *stringData);
		char *getStringByIndex(int index);

		void insertLinkedListAfterIndex(LinkedList *list, int index);

		bool isEmpty(){return !(bool)nodeCount;}

		void cleanup();
	};

	class Stack
	{
	public:
		Stack();
		~Stack();

		LinkedList elementList;

		bool isEmpty(){return !(bool)elementList.nodeCount;}

		void push(void *data);
		void removeTopElement();
		void *peekTopElement();

		void cleanup();
	};

	template <typename type> struct Range
	{
		type head, tail;
	};

	bool isCharNumeric(char c);
	bool isCharAlphabetic(char c);
	bool isCharWhitespace(char c);
	bool isCharDelimiter(char c);

	bool isStringInt(std::string s);
	bool isStringFloat(std::string s);
	bool isStringAlphabetic(std::string s);
	bool isStringWhitespace(std::string s);
	bool isStringValidIdentifier(std::string s);

	bool approximateEquals(float x, float y, float threshold = 0.0001);

	// Debugging and Error handling

	void message(std::string text);
	void debugMessage(std::string text);
	void throwError(std::string text);
};

#endif
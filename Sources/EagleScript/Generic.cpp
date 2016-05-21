#include "Generic.h"

#include "../Debug.h"

namespace EagleScript
{
	LinkedList::LinkedList()
	{
		head = 0;
		tail = 0;

		nodeCount = 0;
	}

	LinkedList::~LinkedList()
	{
		cleanup();
	}

	void LinkedList::cleanup()
	{
		if(nodeCount)
		{
			LinkedListNode *currentNode, *nextNode;

			currentNode = head;

			while(1)
			{
				if(!currentNode)
				{
					break;
				}

				nextNode = currentNode->next;

				if(currentNode->data)
				{
					delete(currentNode->data);
				}

				delete(currentNode);

				if(nextNode)
				{
					currentNode = nextNode;
				}
				else
				{
					break;
				}
			}
		}

		nodeCount = 0;
	}

	int LinkedList::addNode(void *data)
	{
		LinkedListNode *newNode = new LinkedListNode();
		newNode->data = data;
		newNode->next = 0;

		if(!nodeCount)
		{
			head = newNode;
			tail = newNode;
		}
		else
		{
			tail->next = newNode;
			tail = newNode;
		}

		nodeCount++;

		return nodeCount - 1;
	}

	int LinkedList::insertNode(void *data, int index)
	{
		if(index > nodeCount - 1 || index < 0) return -1;

		LinkedListNode *previousNode = findNode(index);
		LinkedListNode *newNode = new LinkedListNode();
		newNode->data = data;
		newNode->next = previousNode->next;

		previousNode->next = newNode;

		if(index == nodeCount - 1)
		{
			tail = newNode;
			tail->next = 0;
		}

		nodeCount++;

		return index + 1;
	}

	/*int LinkedList::insertNode(void *data, int index)
	{
		if(index > nodeCount || index < 0) return -1;

		LinkedListNode *newNode = new LinkedListNode();
		newNode->data = data;

		if(index != 0)
		{
			LinkedListNode *previousNode = findNode(index - 1);
			newNode->next = previousNode->next;

			previousNode->next = newNode;
		}
		else
		{
			newNode->next = head;

			head = newNode;
		}

		if(index == nodeCount)
		{
			tail->next = newNode;

			tail = newNode;
			tail->next = 0;
		}

		if(nodeCount == 0)
		{
			head = newNode;
			head->next = 0;
		}

		nodeCount++;

		return index;
	}*/

	void LinkedList::deleteNode(LinkedListNode *node)
	{
		if(nodeCount == 0 || !node) return;

		if(node == head)
		{
			LinkedListNode *nextNode = head->next;

			if(head->data)
			{
				delete(head->data);
			}

			if(head)
			{
				delete(head);
			}

			head = nextNode;

			if(nodeCount == 1)
			{
				head = 0;
				tail = 0;
			}
		}
		else
		{
			LinkedListNode *currentNode = head;

			for(int i = 0; i < nodeCount; ++i)
			{
				if(currentNode->next == node)
				{
					if(tail == node)
					{
						currentNode->next = 0;
						tail = currentNode;
					}
					else
					{
						currentNode->next = node->next;
					}

					break;
				}

				currentNode = currentNode->next;
			}

			if(node->data)
			{
				delete(node->data);
			}

			if(node)
			{
				delete(node);
			}
		}

		nodeCount--;
	}

	void LinkedList::deleteNode(int index)
	{
		deleteNode(findNode(index));
	}

	int LinkedList::addStringNode(char *stringData)
	{
		LinkedListNode *currentNode = head;

		for(int i = 0; i < nodeCount; ++i)
		{
			if(strcmp((char *)currentNode->data, stringData) == 0) return i;

			currentNode = currentNode->next;
		}

		int stringLength = strlen(stringData);

		char *nodeData = new char[stringLength + 1];

		strcpy(nodeData, stringData);
		nodeData[stringLength] = '\0';

		return addNode(nodeData);
	}

	char *LinkedList::getStringByIndex(int index)
	{
		LinkedListNode *currentNode = head;

		for(int i = 0; i < nodeCount; ++i)
		{
			if(i == index) return (char *)currentNode->data;

			currentNode = currentNode->next;
		}

		return 0;
	}

	LinkedListNode *LinkedList::findNode(int index)
	{
		LinkedListNode *currentNode = head;

		for(int i = 0; i < nodeCount; ++i)
		{
			if(i == index) return currentNode;

			currentNode = currentNode->next;
		}

		return 0;
	}

	void LinkedList::insertLinkedListAfterIndex(LinkedList *list, int index)
	{
		if(!list) return;
		if(list->nodeCount == 0) return;
		if(index > nodeCount - 1 || index < 0) return;

		LinkedListNode *currentNode = head;

		for(int i = 0; i < nodeCount; ++i)
		{
			if(i == index)
			{
				list->tail->next = currentNode->next;
				currentNode->next = list->head;

				nodeCount += list->nodeCount;

				break;
			}

			if(!currentNode->next) break;

			currentNode = currentNode->next;
		}
	}

	Stack::Stack()
	{
	}

	Stack::~Stack()
	{
		elementList.cleanup();
	}

	void Stack::push(void *data)
	{
		elementList.addNode(data);
	}

	void Stack::removeTopElement()
	{
		//message("Count : " + INT_TO_STRING(elementList.nodeCount));
		if(!elementList.tail) return;

		elementList.deleteNode(elementList.tail);
	}

	void *Stack::peekTopElement()
	{
		return elementList.tail->data;
	}

	void Stack::cleanup()
	{
		elementList.cleanup();
	}

	bool isCharNumeric(char c)
	{
		if(c > '0' - 1 && c < '9' + 1) return 1;

		return 0;
	}

	bool isCharAlphabetic(char c)
	{
		if((c > 'a' - 1 && c < 'z' + 1) || (c > 'A' - 1 && c < 'Z' + 1)) return 1;

		return 0;
	}

	bool isCharWhitespace(char c)
	{
		if(c == ' ' || c == '\t' || c == '\n') return 1;

		return 0;
	}

	bool isCharDelimiter(char c)
	{
		if(c == ';' || c == ':' || c == ',' || c == '"' || c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')' || isCharWhitespace(c)) return 1;

		return 0;
	}

	bool isStringInt(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		if(!isCharNumeric(s[0]) && s[0] != '-')
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 1; i < stringLength; ++i)
		{
			if(!isCharNumeric(s[i]))
			{
				return 0;
			}
		}

		return 1;
	}

	bool isStringFloat(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		if(!isCharNumeric(s[0]) && s[0] != '-')
		{
			return 0;
		}

		bool radixPointFound = 0;

		int stringLength = s.length();

		for(int i = 1; i < stringLength; ++i)
		{
			if(isCharNumeric(s[i]))
			{
				continue;
			}
			else if(s[i] == '.' && !radixPointFound)
			{
				radixPointFound = 1;
				continue;
			}

			return 0;
		}

		if(radixPointFound)
		{
			return 1;
		}
		
		return 0;
	}

	bool isStringAlphabetic(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 0; i < stringLength; ++i)
		{
			if(!isCharAlphabetic(s[i])) return 0;
		}

		return 1;
	}

	bool isStringWhitespace(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 0; i < stringLength; ++i)
		{
			if(!isCharWhitespace(s[i]))
			{
				return 0;
			}
		}

		return 1;
	}

	bool isStringValidIdentifier(std::string s)
	{
		if(!IS_STRING_VALID(s))
		{
			return 0;
		}

		if(!isCharAlphabetic(s[0]))
		{
			return 0;
		}

		int stringLength = s.length();

		for(int i = 1; i < stringLength; ++i)
		{
			if(!isCharNumeric(s[i]) && !isCharAlphabetic(s[i]) && s[i] != '_')
			{
				return 0;
			}
		}

		return 1;
	}

	bool approximateEquals(float x, float y, float threshold)
	{
		if(x + threshold > y && x - threshold < y)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	// Debugging and Error handling

	void message(std::string text)
	{
		/*if(text.length() > 0)
		{
			_sleep(300);
		}*/

		//cout << text << "\n";

		Debug::throwMessage(text);
	}

	void debugMessage(std::string text)
	{
		//cout << "Debug : " << text << "\n";

		Debug::outputLogEvent(text);
	}

	void throwError(std::string text)
	{
		//console.print("Error : " + text, ConsoleOutput_Warning);

		Debug::throwError(text);
	}
};
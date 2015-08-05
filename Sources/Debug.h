#ifndef _DEBUG_H
#define _DEBUG_H 1

#include <string>

#include <WinSock2.h>
#include <Windows.h>

namespace Debug
{
	void throwMessage(std::string messageString, std::string title = "Message");
	void throwError(std::string messageString, std::string title = "Error");

	void initializeLoggingSystem();
	void shutdownLoggingSystem();

	void outputLogEvent(std::string eventMessage);

	void disableLoggingSystem();
	void enableLoggingSystem();

	bool isLoggingSystemEnabled();
};

#endif
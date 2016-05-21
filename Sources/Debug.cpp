#include "Debug.h"

#include <vector>

#include "Helpers.h"

const int LOG_STRING_LIST_INITIAL_SIZE = 512;

FILE *logFile;
std::vector<std::string> logStringList;

bool loggingSystemEnabled = 1;
bool loggingSystemInitialized = 0;

namespace Debug
{
	void throwMessage(std::string messageString, std::string title)
	{

#ifdef PLATFORM_WP8

		OutputDebugString((messageString + "\n").c_str());

#endif

		if(messageString != "")
		{
			outputLogEvent((char *)(messageString).c_str());
		}
	}

	void throwError(std::string messageString, std::string title)
	{
#ifdef PLATFORM_WP8

		OutputDebugString((messageString + "\n").c_str());

#endif
		outputLogEvent("");
		if(messageString != "") outputLogEvent((char *)(messageString).c_str());
		if(messageString != "") outputLogEvent("Shutting down...");

		shutdownLoggingSystem();

		exit(EXIT_FAILURE);
	}

	// Logging system

	void initializeLoggingSystem()
	{
		logStringList.reserve(LOG_STRING_LIST_INITIAL_SIZE);

		loggingSystemInitialized = 1;
	}

	void shutdownLoggingSystem()
	{
		if(!loggingSystemEnabled || !loggingSystemInitialized)
		{
			return;
		}

#ifndef PLATFORM_WP8

		if(!(logFile = fopen("log.txt", "wt")))
		{
			return;
		}

		for(int i = 0; i < logStringList.size(); ++i)
		{
			fwrite(logStringList[i].c_str(), strlen(logStringList[i].c_str()), 1, logFile);
		}

		fclose(logFile);
#endif

		loggingSystemInitialized = 0;
	}

	void outputLogEvent(std::string eventMessage)
	{
		if(!loggingSystemEnabled || !loggingSystemInitialized)
		{
			return;
		}

#ifdef PLATFORM_WP8

		this->message(eventMessage);

#else

		//OutputDebugStringA((char *)(eventString + "\n").c_str());

#endif

		eventMessage += '\n';

		logStringList.push_back(eventMessage);
	}

	void disableLoggingSystem()
	{
		loggingSystemEnabled = 1;
	}

	void enableLoggingSystem()
	{
		loggingSystemEnabled = 0;
	}

	bool isLoggingSystemEnabled()
	{
		return loggingSystemEnabled;
	}
};
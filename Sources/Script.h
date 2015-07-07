#include "Eagle.h"
#pragma once
namespace ProjectEagle
{
	class Script
    {
    private:
		lua_State *luaState;
	public:
		Script();
		Script(std::string scriptFileAdress);
		virtual ~Script();

		bool loadScript(std::string scriptFileAdress);

		std::string getGlobalString(std::string name);
		void setGlobalString(std::string name, std::string value);

		float getGlobalNumber(std::string name);
		void setGlobalNumber(std::string name, float value);

		bool getGlobalBoolean(std::string name);
		void setGlobalBoolean(std::string name, bool value);

		void runFunction(std::string name);
	};
};
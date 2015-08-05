#ifndef _RESOURCE_H
#define _RESOURCE_H 1

#include <string>

#include <WinSock2.h>
#include <Windows.h>

#ifndef PLATFORM_WP8
	#include <d3d11.h>
#else
	#include <d3d11_1.h>
#endif

#include "Helpers.h"

enum ResourceType
{
	RESOURCE_NONE,
	RESOURCE_TEXTURE
};

namespace ProjectEagle
{
	class Resource
	{
		friend class ResourceManagerClass;

	protected:
		int m_type;
		std::string m_fileAddress;
		bool m_loaded;

	public:
		Resource();
		~Resource();

		int getType();

		std::string getAddress();

		virtual void releaseData();

		bool isLoaded();
	};
};

#endif
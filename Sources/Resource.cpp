#include "Resource.h"

namespace ProjectEagle
{
	Resource::Resource()
	{
		m_type = RESOURCE_NONE;
		m_fileAddress = "";

		m_loaded = 0;
	}

	Resource::~Resource()
	{
	}

	void Resource::releaseData()
	{
	}

	int Resource::getType()
	{
		return m_type;
	}

	std::string Resource::getAddress()
	{
		return m_fileAddress;
	}

	bool Resource::isLoaded()
	{
		return m_loaded;
	}
};
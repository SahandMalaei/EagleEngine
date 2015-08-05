#ifndef _RESOURCEMANAGER_H
#define _RESOURCEMANAGER_H 1

#include <vector>
#include <string>
#include <thread>

#include "Resource.h"
#include "Graphics.h"

enum ResourceEventType
{
	RESOURCE_EVENT_LOADED,
	RESOURCE_EVENT_FAILED
};

namespace ProjectEagle
{
	typedef std::vector<Resource *> ResourceList;

	struct ResourceEventProperties;

	class ResourceManagerClass
	{
		friend class EagleEngine;
		friend class GraphicsSystem;

	private:
		ResourceList m_resourceList;

		std::vector<std::thread> m_threadList;

		std::vector<ResourceEventProperties> m_eventQueue;

		void requestFile(std::string fileAddress, ResourceType resourceType);

		void initialize();

	public:
		ResourceManagerClass();
		~ResourceManagerClass();

		void update();

		Texture *loadTexture(std::string fileAddress, bool asynchronous = 0);

		void removeResource(std::string fileAddress);

		Resource *findResource(std::string fileAddress);
		int resourceExists(std::string fileAddress);

		ID3D11Device *getD3DDevice();
	};
};

extern ProjectEagle::ResourceManagerClass resourceManager;

#endif
#include "Eagle.h"
#include <Include/DirectX/TK/DDSTextureLoader.h>

static ID3D11Device *d3dDevice11;

namespace ProjectEagle
{
	struct FileLoadInformation
	{
		void *data;
		int size;
	};

	FileLoadInformation loadFile(std::string fileAddress)
	{
		FILE *file;
		file = fopen(fileAddress.c_str(), "r");

		/*if(!file)
		{
			eagle.error("Failed to open \"" + fileAddress + "\"");
		}*/

		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);

		fseek(file, 0, SEEK_SET);

		char *dataBuffer = new char[fileSize];
		fread(dataBuffer, fileSize, 1, file);

		FileLoadInformation fileInformation;
		fileInformation.data = dataBuffer;
		fileInformation.size = fileSize;

		return fileInformation;
	}

	struct ResourceEventProperties
	{
		ResourceEventType eventType;
		ResourceType resourceType;
		std::string fileAddress;

		void *data;
	};

	struct ResourceRequestProperties
	{
		ResourceType type;
		std::string fileAddress;
	};

	struct TextureEventData
	{
		ID3D11Texture2D *m_d3dTexture11;
		ID3D11ShaderResourceView *resourceView;

		short m_width, m_height;

		void *m_fileData;
		int m_fileSize;
	};

	std::vector<ResourceRequestProperties> sharedResourceRequestQueue;
	static std::vector<ResourceEventProperties> sharedEventQueue;

	std::mutex requestMutex;
	std::mutex eventMutex;
	std::mutex gameOverMutex;

	bool sharedGameOverFlag = 0;

	void resourceManagerAsynchronousFunction();

	ResourceManagerClass::ResourceManagerClass()
	{
	}

	ResourceManagerClass::~ResourceManagerClass()
	{
		for(int i = 0; i < m_resourceList.size(); ++i)
		{
			m_resourceList[i]->releaseData();
		}

		m_resourceList.clear();
	}

	void ResourceManagerClass::initialize()
	{
		d3dDevice11 = graphics.getD3DDevice11();

		m_threadList.push_back(std::thread(resourceManagerAsynchronousFunction));
		m_threadList[0].detach();
	}

	Resource *ResourceManagerClass::findResource(std::string fileAddress)
	{
		for(int i = 0; i < m_resourceList.size(); ++i)
		{
			if(fileAddress == m_resourceList[i]->getAddress())
			{
				return m_resourceList[i];

				break;
			}
		}

		return 0;
	}

	int ResourceManagerClass::resourceExists(std::string fileAddress)
	{
		int returnValue = -1;

		for(int i = 0; i < m_resourceList.size(); ++i)
		{
			if(fileAddress == m_resourceList[i]->m_fileAddress)
			{
				returnValue = i;

				break;
			}
			else
			{
			}
		}

		return returnValue;
	}

	Texture *ResourceManagerClass::loadTexture(std::string fileAddress, bool asynchronous)
	{
		int resourceExistID = resourceExists(fileAddress);
		if(resourceExistID != -1)
		{
			return (Texture *)m_resourceList[resourceExistID];
		}

		if(asynchronous)
		{
			Texture *returnTexture;

			m_resourceList.push_back(new Texture());
			returnTexture = (Texture *)m_resourceList[m_resourceList.size() - 1];
			returnTexture->m_fileAddress = fileAddress;

			requestFile(fileAddress, RESOURCE_TEXTURE);

			return returnTexture;
		}

		switch(graphics.getGraphicsAPIType())
		{
		case GraphicsAPI_Direct3D11:
			{
				wchar_t addressBuffer[512];
				AnsiToUnicode((char *)fileAddress.c_str(), addressBuffer, 1024);

				ID3D11Texture2D *texture;
				ID3D11ShaderResourceView *resourceView;

				HRESULT result;

				//FileLoadInformation fileInformation = loadFile(fileAddress);
				//eagle.message(INT_TO_STRING(fileInformation.size));

#ifndef PLATFORM_WP8
				D3DX11_IMAGE_LOAD_INFO loadInfo;
				//ZeroMemory(&loadInfo, sizeof(loadInfo));

				result = D3DX11CreateTextureFromFile(graphics.m_d3dDevice11, addressBuffer, &loadInfo, 0, (ID3D11Resource **)(&texture), 0);
				//result = D3DX11CreateTextureFromMemory(graphics.m_d3dDevice11, fileInformation.data, fileInformation.size, &loadInfo, 0, (ID3D11Resource **)(&texture), 0);
				//HRESULT result = D3DX11CreateShaderResourceViewFromFileA(graphics.getD3DDevice11(), address, /*&loadInfo*/ 0, 0, &resourceView, 0);

				if(FAILED(result))
				{
					eagle.error("Error loading \"" + (std::string)fileAddress + "\" Error code : " + INT_TO_STRING(result));

					return 0;
				}

				result = graphics.m_d3dDevice11->CreateShaderResourceView((ID3D11Resource *)(texture), 0, &resourceView);

				if(FAILED(result))
				{
					eagle.error("Error loading \"" + (std::string)fileAddress + "\" Error code : " + INT_TO_STRING(result));

					return 0;
				}
#else
				wchar_t buffer[1024];
				AnsiToUnicode((char *)fileAddress.c_str(), buffer, 2048);
				result = DirectX::CreateDDSTextureFromFile(graphics.m_d3dDevice11.Get(), buffer, (ID3D11Resource **)(&texture), &resourceView, MAXSIZE_T);

				if(FAILED(result))
				{
					eagle.error("Error loading \"" + (std::string)fileAddress + "\" Error code : " + INT_TO_STRING(result));

					return 0;
				}
#endif

				Texture *returnTexture;

				m_resourceList.push_back(new Texture());
				returnTexture = (Texture *)m_resourceList[m_resourceList.size() - 1];
				returnTexture->m_fileAddress = fileAddress;

				returnTexture->m_d3dTexture11 = texture;
				returnTexture->m_shaderResourceView = resourceView;

				D3D11_TEXTURE2D_DESC textureDesc;
				texture->GetDesc(&textureDesc);

				returnTexture->m_width = textureDesc.Width;
				returnTexture->m_height = textureDesc.Height;

				returnTexture->m_loaded = 1;

				eagle.outputLogEvent("Texture \"" + ((std::string)fileAddress) + "\" successfully loaded");
				eagle.outputLogEvent("");

				//console.print("Texture \"" + (std::string)address + "\" loaded", ConsoleOutput_Success);

				return returnTexture;
			}
		}

		return 0;
	}

	void ResourceManagerClass::removeResource(std::string fileAddress)
	{
		ResourceList::iterator listIterator;

		for(listIterator = m_resourceList.begin(); listIterator != m_resourceList.end(); ++listIterator)
		{
			if(fileAddress == (*listIterator)->m_fileAddress)
			{
				(*listIterator)->releaseData();

				delete(*listIterator);

				m_resourceList.erase(listIterator);

				break;
			}
		}
	}

	void ResourceManagerClass::requestFile(std::string fileAddress, ResourceType resourceType)
	{
		requestMutex.lock();

		std::vector<ResourceRequestProperties>::iterator listIterator;
		for(listIterator = sharedResourceRequestQueue.begin(); listIterator != sharedResourceRequestQueue.end(); ++listIterator)
		{
			if((*listIterator).fileAddress == fileAddress)
			{
				requestMutex.unlock();

				return;
			}
		}

		ResourceRequestProperties requestProperties;
		requestProperties.fileAddress = fileAddress;
		requestProperties.type = resourceType;

		sharedResourceRequestQueue.push_back(requestProperties);

		requestMutex.unlock();
	}

	ID3D11Device *ResourceManagerClass::getD3DDevice()
	{
#ifndef PLATFORM_WP8
		return graphics.m_d3dDevice11;
#else
		return graphics.m_d3dDevice11.Get();
#endif
	}

	void resourceManagerAsynchronousFunction()
	{
		while(1)
		{
			while(!requestMutex.try_lock())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			if(sharedResourceRequestQueue.size() > 0)
			{
				std::vector<ResourceRequestProperties>::iterator listIterator = sharedResourceRequestQueue.begin();
				ResourceType resourceType = (*listIterator).type;
				std::string fileAddress = (*listIterator).fileAddress;

				sharedResourceRequestQueue.erase(listIterator);

				requestMutex.unlock();

				//FileLoadInformation fileInformation = loadFile(fileAddress);

				switch(resourceType)
				{
				case RESOURCE_NONE:
					{
						break;
					}
				case RESOURCE_TEXTURE:
					{
						ID3D11Texture2D *texture = 0;
						HRESULT result;

						wchar_t addressBuffer[512];
						AnsiToUnicode((char *)fileAddress.c_str(), addressBuffer, 1024);

#ifndef PLATFORM_WP8
						D3DX11_IMAGE_LOAD_INFO loadInfo;

						result = D3DX11CreateTextureFromFile(d3dDevice11, addressBuffer, &loadInfo, 0, (ID3D11Resource **)(&texture), 0);
						//result = D3DX11CreateTextureFromMemory(d3dDevice11, fileInformation.data, fileInformation.size, &loadInfo, 0, (ID3D11Resource **)(&texture), 0);

						if(FAILED(result))
						{
							eagle.error("Error loading \"" + (std::string)fileAddress + "\" Error code : " + INT_TO_STRING(result));

							//delete(fileInformation.data);

							continue;
						}

#else
						//FileLoadInformation fileInformation = loadFile(fileAddress);
						//result = DirectX::CreateDDSTextureFromMemory(d3dDevice11, (uint8_t *)fileInformation.data, fileInformation.size, (ID3D11Resource **)(&texture), &resourceView, MAXSIZE_T);

						/*if(FAILED(result))
						{
							eagle.error("Error loading \"" + (std::string)address + "\" Error code : " + INT_TO_STRING(result));

							return 0;
						}*/
#endif

						D3D11_TEXTURE2D_DESC textureDesc;
						texture->GetDesc(&textureDesc);

						TextureEventData *textureData = new TextureEventData();
						textureData->m_d3dTexture11 = texture;
						textureData->m_width = textureDesc.Width;
						textureData->m_height = textureDesc.Height;
						//textureData->m_fileData = fileInformation.data;
						//textureData->m_fileSize = fileInformation.size;

						ResourceEventProperties eventProperties;
						eventProperties.eventType = RESOURCE_EVENT_LOADED;
						eventProperties.resourceType = RESOURCE_TEXTURE;
						eventProperties.fileAddress = fileAddress;
						eventProperties.data = textureData;

						while(!eventMutex.try_lock())
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
						}

						sharedEventQueue.push_back(eventProperties);
						eventMutex.unlock();

						break;
					}
				}

				//delete(fileInformation.data);
			}
			else
			{
				requestMutex.unlock();

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}

	void ResourceManagerClass::update()
	{
		eventMutex.lock();

		if(sharedEventQueue.size())
		{
			std::vector<ResourceEventProperties>::iterator listIterator = sharedEventQueue.begin();
			ResourceEventProperties eventProperties = (*listIterator);
			sharedEventQueue.erase(listIterator);
			eventMutex.unlock();

			switch(eventProperties.resourceType)
			{
			case RESOURCE_NONE:
				{
					break;
				}
			case RESOURCE_TEXTURE:
				{
					ID3D11ShaderResourceView *resourceView;

					TextureEventData *textureData = (TextureEventData *)eventProperties.data;
					Texture *texture = (Texture *)findResource(eventProperties.fileAddress);
					if(texture)
					{
						HRESULT result = d3dDevice11->CreateShaderResourceView((ID3D11Resource *)(textureData->m_d3dTexture11), 0, &resourceView);

						if(FAILED(result))
						{
							if(result == DXGI_ERROR_DEVICE_REMOVED)
							{
								result = d3dDevice11->GetDeviceRemovedReason();

								eagle.outputLogEvent(INT_TO_STRING(result));
							}

							eagle.error("Error loading \"" + (std::string)eventProperties.fileAddress + "\" Error code : " + INT_TO_STRING(result));

							return;
						}

						texture->m_d3dTexture11 = textureData->m_d3dTexture11;
						texture->m_shaderResourceView = resourceView;
						texture->m_width = textureData->m_width;
						texture->m_height = textureData->m_height;

						texture->m_loaded = 1;
					}

					textureData->m_d3dTexture11->Release();

					delete(textureData);

					break;
				}
			}
		}
		else
		{
			eventMutex.unlock();
		}
	}
};
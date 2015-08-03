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

	Texture::Texture()
	{
		m_type = RESOURCE_TEXTURE;

		m_d3dTexture11 = 0;
	}

	Texture::~Texture()
	{
	}

	void Texture::lock()
	{
	}

	void Texture::unlock()
	{
	}

	DWORD Texture::getPixelColor(int x, int y)
	{

		return 0;
	}

	void Texture::releaseData()
	{
		if(m_loaded)
		{
			if(m_shaderResourceView)
			{
				m_shaderResourceView->Release();
				m_shaderResourceView = 0;
			}

			m_loaded = 0;
		}
	}

	int Texture::getWidth()
	{
		return m_width;
	}

	int Texture::getHeight()
	{
		return m_height;
	}

	Vector2 Texture::getDimensions()
	{
		return Vector2(m_width, m_height);
	}
};
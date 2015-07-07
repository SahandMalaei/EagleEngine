#ifndef _RESOURCE_H
#define _RESOURCE_H 1

#include "Eagle.h"

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

		string getAddress();

		virtual void releaseData();

		bool isLoaded();
	};

	class Texture : public Resource
	{
		friend class ResourceManagerClass;
		friend class GraphicsSystem;

	private:
		ID3D11Texture2D *m_d3dTexture11;

		ID3D11ShaderResourceView* m_shaderResourceView;
		ID3D11SamplerState* m_samplerState;

		short m_width, m_height;

	public:
		Texture();
		~Texture();

		int getWidth();
		int getHeight();
		Vector2 getDimensions();

		virtual void releaseData();

		void lock();
		void unlock();

		DWORD getPixelColor(int x, int y);
	};
};

#endif
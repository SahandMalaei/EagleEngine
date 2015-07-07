#ifndef _SHADER_H
#define _SHADER_H 1

#include "Eagle.h"

namespace ProjectEagle
{
	enum ShaderType
	{
		ShaderType_VertexShader = 0,
		ShaderType_PixelShader = 1
	};

	class Shader
	{
		friend class GraphicsSystem;

	private:
		union
		{
			ID3D11VertexShader *vertexShader;
			ID3D11PixelShader *pixelShader;
		};

		ID3D11InputLayout *inputLayout;

		ID3D10Blob *shaderData;
		void *dataBuffer;
		int dataSize;

		ID3D11Buffer *constantBuffer;

		ShaderType type;

		bool isInitialized;

		void createInputLayouts();

	public:
		Shader();
		~Shader();

		void compileVertexShaderFromFile(std::string fileAddress);
		void compilePixelShaderFromFile(std::string fileAddress);

		void loadVertexShaderFromFile(std::string fileAddress);
		void loadPixelShaderFromFile(std::string fileAddress);
	};
};

#endif
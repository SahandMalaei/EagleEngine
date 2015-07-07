#include "Eagle.h"

namespace ProjectEagle
{
	Shader::Shader()
	{
		isInitialized = 0;
	}

	Shader::~Shader()
	{
	}

	void Shader::compileVertexShaderFromFile(std::string fileAddress)
	{
		type = ShaderType_VertexShader;

		WCHAR addressString[1024];
		AnsiToUnicode((char *)fileAddress.c_str(), addressString, 2048);

		ID3D10Blob *errorData;

		HRESULT result;
		
#ifndef PLATFORM_WP8
		result = D3DCompileFromFile(addressString, 0, 0, "main", "vs_4_0_level_9_3", 0, 0, &shaderData, &errorData);
#endif

		if(FAILED(result))
		{
			if(!errorData)
			{
				eagle.error("Failed to load " + fileAddress + ". Error code : " + INT_TO_STRING(result));
			}
			else
			{
				eagle.error("Failed to load " + fileAddress + ". " + (char *)errorData->GetBufferPointer());
			}

			return;
		}

		/*if(errorData && errorData->GetBufferSize())
		{
			eagle.message((char *)errorData->GetBufferPointer());
		}*/
		
		result = graphics.getD3DDevice11()->CreateVertexShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(), 0, &vertexShader);

		if(FAILED(result))
		{
			eagle.error("Failed to load " + fileAddress + ". Error code : " + INT_TO_STRING(result));

			return;
		}

		dataBuffer = shaderData->GetBufferPointer();
		dataSize = shaderData->GetBufferSize();

		createInputLayouts();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CBChangesEveryFrame);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		result = graphics.getD3DDevice11()->CreateBuffer(&bd, 0, &constantBuffer);

		if(FAILED(result))
		{
			eagle.error("Failed to create the constant buffers. Error code : " + INT_TO_STRING(result));
		}

		isInitialized = 1;
	}

	void Shader::compilePixelShaderFromFile(std::string fileAddress)
	{
		type = ShaderType_PixelShader;

		WCHAR addressString[1024];
		AnsiToUnicode((char *)fileAddress.c_str(), addressString, 2048);

		ID3D10Blob *errorData;

		HRESULT result;

#ifndef PLATFORM_WP8
		result = D3DCompileFromFile(addressString, 0, 0, "main", "ps_4_0_level_9_3", 0, 0, &shaderData, &errorData);
#endif

		if(FAILED(result))
		{
			if(!errorData)
			{
				eagle.error("Failed to load " + fileAddress + ". Error code : " + INT_TO_STRING(result));
			}
			else
			{
				eagle.error("Failed to load " + fileAddress + ". " + (char *)errorData->GetBufferPointer());
			}

			return;
		}

		/*if(errorData && errorData->GetBufferSize())
		{
			eagle.message((char *)errorData->GetBufferPointer());
		}*/

		result = graphics.getD3DDevice11()->CreatePixelShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(), 0, &pixelShader);

		if(FAILED(result))
		{
			eagle.error("Failed to load " + fileAddress + ". Error code : " + INT_TO_STRING(result));

			return;
		}

		isInitialized = 1;

		dataBuffer = shaderData->GetBufferPointer();
		dataSize = shaderData->GetBufferSize();
	}

	void Shader::loadVertexShaderFromFile(std::string fileAddress)
	{
		type = ShaderType_VertexShader;

		FILE *file;

		if(!(file = fopen(fileAddress.c_str(), "rb")))
		{
			eagle.error("Failed to open " + (string)fileAddress);
		}

		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);

		fseek(file, 0, SEEK_SET);

		char *fileBuffer = new char[fileSize + 1];

		fread(fileBuffer, fileSize, 1, file);

		fclose(file);

		HRESULT result;
		result = graphics.getD3DDevice11()->CreateVertexShader(fileBuffer, fileSize, 0, &vertexShader);

		if(FAILED(result))
		{
			eagle.error("Failed to load " + fileAddress + ". Error code : " + INT_TO_STRING(result));

			return;
		}

		dataBuffer = fileBuffer;
		dataSize = fileSize;

		createInputLayouts();

		CD3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CBChangesEveryFrame);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		result = graphics.getD3DDevice11()->CreateBuffer(&bd, 0, &constantBuffer);

		if(FAILED(result))
		{
			eagle.error("Failed to create the constant buffers. Error code : " + INT_TO_STRING(result));
		}

		isInitialized = 1;
	}

	void Shader::loadPixelShaderFromFile(std::string fileAddress)
	{
		type = ShaderType_PixelShader;

		FILE *file;

		if(!(file = fopen(fileAddress.c_str(), "rb")))
		{
			eagle.error("Failed to open " + (string)fileAddress);
		}
		
		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);

		fseek(file, 0, SEEK_SET);

		char *fileBuffer = new char[fileSize + 1];

		fread(fileBuffer, fileSize, 1, file);

		fclose(file);

		HRESULT result;
		result = graphics.getD3DDevice11()->CreatePixelShader(fileBuffer, fileSize, 0, &pixelShader);

		if(FAILED(result))
		{
			eagle.error("Failed to load " + fileAddress + ". Error code : " + INT_TO_STRING(result));

			return;
		}

		isInitialized = 1;

		dataBuffer = fileBuffer;
		dataSize = fileSize;
	}

	void Shader::createInputLayouts()
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc0[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		HRESULT result = graphics.getD3DDevice11()->CreateInputLayout(inputElementDesc0, 3, dataBuffer, dataSize, &inputLayout);

		if(FAILED(result))
		{
			eagle.error("Failed to create the vertex input layout. Error code : " + INT_TO_STRING(result));

			return;
		}
	}
};
#include "Eagle.h"

namespace ProjectEagle
{
	D3D_DRIVER_TYPE d3d11DriverTypeList[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE
	};

	int d3d11DriverTypeCount = ARRAY_SIZE(d3d11DriverTypeList);

	D3D_FEATURE_LEVEL d3d11FeatureLevelList[] = 
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	int d3d11FeatureLevelCount = ARRAY_SIZE(d3d11FeatureLevelList);

	struct DrawCallData
	{
		int primitiveCount;
		int vertexCount;
		Texture *texture;
		int primitiveGroupSize;
		bool isTransformed;

		bool worldMatrixChange;
		DirectX::XMMATRIX worldMatrix;
	};

	GraphicsSystem::GraphicsSystem()
	{
		m_graphicsAPIType = GraphicsAPI_Direct3D11;

#ifndef PLATFORM_WP8
		m_windowHandle = 0;

		m_windowResizable = 0;
#endif

		m_lastPoint.x = 0;
		m_lastPoint.y = 0;

		m_screenWidth = -1;
		m_screenHeight = -1;

		m_fullscreen = 0;
		m_swapChainFullscreen = 0;

		m_cursorVisibility = 1;

		m_isDeviceLost = 0;
		m_isLostDeviceFixed = 0;

		m_multiSamplingSupport = 1;
		m_multiSamplingEnabled = 0;
		m_multiSamplingSampleCount = 1;
		m_multiSamplingQualityLevel = 0;

		m_textureEnabled = 0;

		m_drawCallCount = 0;
		m_frameVertexBuffer_CurrentVertexIndex = 0;

#ifndef PLATFORM_WP8
		m_drawCallBufferSize = 30000;
#else
		m_drawCallBufferSize = 15000;
#endif
		m_drawCallBuffer = new DrawCallData[m_drawCallBufferSize];
#ifndef PLATFORM_WP8
		m_frameVertexBufferSize = 100000;
#else
		m_frameVertexBufferSize = 50000;
#endif
		m_frameVertexBuffer = new Vertex[m_frameVertexBufferSize];

		m_renderingBufferedDrawCalls = 0;

		m_textureTopLeft.set(0, 0);
		m_textureBottomRight.set(1, 1);
	}

	GraphicsSystem::~GraphicsSystem()
	{
	}

	void GraphicsSystem::setGraphicsAPI(GraphicsAPIType type)
	{
		switch(type)
		{
		case GraphicsAPI_Direct3D11:
			{
				m_graphicsAPIType = type;

				break;
			}
		}
	}

#ifndef PLATFORM_WP8

	HWND GraphicsSystem::getWidnowHandle()
	{
		return m_windowHandle;
	}

#else

	Windows::UI::Core::CoreWindow ^GraphicsSystem::getWindowHandle()
	{
		return m_windowHandle.Get();
	}

#endif

	GraphicsAPIType GraphicsSystem::getGraphicsAPIType()
	{
		return m_graphicsAPIType;
	}

#ifndef PLATFORM_WP8
	ID3D11Device *GraphicsSystem::getD3DDevice11()
	{
		return m_d3dDevice11;
	}
#else
	ID3D11Device1 *GraphicsSystem::getD3DDevice11()
	{
		return m_d3dDevice11.Get();
	}
#endif

	ID3D11DeviceContext *GraphicsSystem::getD3DDevice11Context()
	{
#ifndef PLATFORM_WP8
		return m_d3dDevice11Context;
#else
		return m_d3dDevice11Context.Get();
#endif
	}

	void GraphicsSystem::preinitialize()
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
#ifndef PLATFORM_WP8
				if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1) ,(void**)&m_dxgiFactory)))
				{
					eagle.error("Failed to create the DXGIFactory");
				}

				m_dxgiFactory->EnumAdapters(0, &m_adapter);

				IDXGIOutput* output = NULL; 
				HRESULT hr;

				hr = m_adapter->EnumOutputs(0,&output);

				UINT numModes = 0;
				DXGI_MODE_DESC* displayModes = NULL;
				DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

				hr = output->GetDisplayModeList(format, 0, &numModes, NULL);

				displayModes = new DXGI_MODE_DESC[numModes]; 

				hr = output->GetDisplayModeList(format, 0, &numModes, displayModes);

				m_screenWidth = displayModes[numModes - 1].Width;
				m_screenHeight = displayModes[numModes - 1].Height;

				m_clientScreenWidth = m_screenWidth;
				m_clientScreenHeight = m_screenHeight;

				DXGI_ADAPTER_DESC adapterDesc;

				m_adapter->GetDesc(&adapterDesc);

				UnicodeToAnsi(adapterDesc.Description, m_adapterName, 128);

				m_displayFormat = displayModes[numModes - 1].Format;
#endif
				break;
			}
		}
	}

	bool GraphicsSystem::createWindowSizeDependentObjects()
	{
		HRESULT result;

		m_multiSamplingSampleCount = 1;
		m_multiSamplingQualityLevel = 0;

#ifndef PLATFORM_WP8
		if(m_multiSamplingEnabled)
		{
			for(int i = 1; i < 3; ++i)
			{
				int sampleCount = math.power(2, i);
				unsigned int quality;

				m_d3dDevice11->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &quality);

				if(quality == 0)
				{
					break;
				}
				else
				{
					m_multiSamplingSampleCount = sampleCount;
					m_multiSamplingQualityLevel = quality - 1;
				}
			}
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		std::memset(&swapChainDesc, 0, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = m_screenWidth;
		swapChainDesc.BufferDesc.Height = m_screenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = EAGLE_DEFAULT_FRAME_RATE;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = m_windowHandle;
		swapChainDesc.Windowed = !m_fullscreen;
		swapChainDesc.SampleDesc.Count = m_multiSamplingSampleCount;
		swapChainDesc.SampleDesc.Quality = m_multiSamplingQualityLevel;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		m_swapChainWidth = m_screenWidth;
		m_swapChainHeight = m_screenHeight;

		m_swapChainFullscreen = m_fullscreen;

		result = m_dxgiFactory->CreateSwapChain(m_d3dDevice11, &swapChainDesc, &m_swapChain);

		if(FAILED(result))
		{
			//eagle.error("Failed to create the Direct3D 11 swap chain. Error code : " + INT_TO_STRING(result));

			return 0;
		}

#endif
		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&m_backBufferTexture);

		if(FAILED(result))
		{
			//eagle.error("Failed to get the swap chain back buffer. Error code : " + INT_TO_STRING(result));

			return 0;
		}

#ifndef PLATFORM_WP8
		result = m_d3dDevice11->CreateRenderTargetView(m_backBufferTexture, 0, &m_renderTargetView);
#else
		result = m_d3dDevice11->CreateRenderTargetView(m_backBufferTexture.Get(), 0, &m_renderTargetView);
#endif

		//if(m_backBufferTexture) m_backBufferTexture->Release();

		if(FAILED(result))
		{
			//eagle.error("Failed to create the render target view. Error code : " + INT_TO_STRING(result));

			return 0;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		std::memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
		depthStencilDesc.Width = m_screenWidth;
		depthStencilDesc.Height = m_screenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = m_multiSamplingSampleCount;
		depthStencilDesc.SampleDesc.Quality = m_multiSamplingQualityLevel;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		result = m_d3dDevice11->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilTexture);

		if(FAILED(result))
		{
			//eagle.error("Failed to create the depth-stencil texture. Error code : " + INT_TO_STRING(result));

			return 0;
		}

		D3D11_DEPTH_STENCIL_DESC dsDesc;

		dsDesc.DepthEnable = 1;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		dsDesc.StencilEnable = 0;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		ID3D11DepthStencilState *pDSState;

		result = m_d3dDevice11->CreateDepthStencilState(&dsDesc, &pDSState);

		if(FAILED(result))
		{
			//eagle.error("Direct3D Depth-Stencil state creation failed with code " + INT_TO_STRING(result));

			return 0;
		}

		m_d3dDevice11Context->OMSetDepthStencilState(pDSState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		std::memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = (m_multiSamplingSampleCount > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

#ifndef PLATFORM_WP8
		result = m_d3dDevice11->CreateDepthStencilView(m_depthStencilTexture, &depthStencilViewDesc, &m_depthStencilView);
#else
		result = m_d3dDevice11->CreateDepthStencilView(m_depthStencilTexture.Get(), &depthStencilViewDesc, &m_depthStencilView);
#endif

		if(FAILED(result))
		{
			//eagle.error("Failed to create the depth-stencil view. Error code : " + INT_TO_STRING(result));

			return 0;
		}

#ifndef PLATFORM_WP8
		m_d3dDevice11Context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
#else
		m_d3dDevice11Context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
#endif

		D3D11_VIEWPORT viewport;
		viewport.Width = m_screenWidth;
		viewport.Height = m_screenHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		m_d3dDevice11Context->RSSetViewports(1, &viewport);

		return 1;
	}

	void GraphicsSystem::initialize()
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				HRESULT result = 0;

#ifndef PLATFORM_WP8

				int creationFlags = 0; // D3D11_CREATE_DEVICE_DEBUG;

				for(int i = 0; i < d3d11DriverTypeCount; ++i)
				{
					result = D3D11CreateDevice(0, d3d11DriverTypeList[i], 0,
						creationFlags, d3d11FeatureLevelList, d3d11FeatureLevelCount,
						D3D11_SDK_VERSION,
						&m_d3dDevice11, &m_d3d11FeatureLevel, &m_d3dDevice11Context);

					if(SUCCEEDED(result))
					{
						m_d3d11DriverType = d3d11DriverTypeList[i];

						break;
					}
				}

				if(FAILED(result))
				{
					eagle.error("Failed to create the Direct3D 11 device. Error code : " + INT_TO_STRING(result));

					return;
				}
#else
				UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
				//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;

				ComPtr<ID3D11Device> device;
				ComPtr<ID3D11DeviceContext> context;

				result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, d3d11FeatureLevelList, ARRAYSIZE(d3d11FeatureLevelList), D3D11_SDK_VERSION,
					&device, &m_d3d11FeatureLevel, &context);

				if(FAILED(result))
				{
					eagle.error("Failed to create the Direct3D11 device. Error code : " + INT_TO_STRING(result));

					return;
				}

				device.As(&m_d3dDevice11);
				context.As(&m_d3dDevice11Context);

				m_windowBounds = m_windowHandle->Bounds;

				// Calculate the necessary swap chain and render target size in pixels.
				m_renderTargetSize.Width = convertDipsToPixels(m_windowBounds.Width);
				m_renderTargetSize.Height = convertDipsToPixels(m_windowBounds.Height);

				m_screenWidth = m_renderTargetSize.Width;
				m_screenHeight = m_renderTargetSize.Height;

				m_clientScreenWidth = m_screenWidth;
				m_clientScreenHeight = m_screenHeight;

				m_swapChainWidth = m_screenWidth;
				m_swapChainHeight = m_screenHeight;

				m_swapChainFullscreen = m_fullscreen;

				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
				swapChainDesc.Width = static_cast<UINT>(m_renderTargetSize.Width); // Match the size of the window.
				swapChainDesc.Height = static_cast<UINT>(m_renderTargetSize.Height);
				swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
				swapChainDesc.Stereo = 0;
				swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = 1; // On phone, only single buffering is supported.
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // On phone, only stretch and aspect-ratio stretch scaling are allowed.
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // On phone, no swap effects are supported.
				swapChainDesc.Flags = 0;

				ComPtr<IDXGIDevice1> dxgiDevice;
				m_d3dDevice11.As(&dxgiDevice);

				ComPtr<IDXGIAdapter> dxgiAdapter;
				dxgiDevice->GetAdapter(&dxgiAdapter);

				ComPtr<IDXGIFactory2> dxgiFactory;
				dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

				Windows::UI::Core::CoreWindow ^window = m_windowHandle.Get();

				result = dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice11.Get(), reinterpret_cast<IUnknown*>(window), &swapChainDesc, 0, &m_swapChain);

				if(FAILED(result))
				{
					eagle.error("Failed to create the swap chain. Error code : " + INT_TO_STRING(result));

					return;
				}

				//dxgiDevice->SetMaximumFrameLatency(1);

				IDXGIAdapter *adapter; 

				dxgiFactory->EnumAdapters(0, &adapter);

				DXGI_ADAPTER_DESC adapterDesc;

				adapter->GetDesc(&adapterDesc);

				UnicodeToAnsi(adapterDesc.Description, m_adapterName, 128);
#endif

				if(!createWindowSizeDependentObjects())
				{
					eagle.error("Failed to create the Direct3D 11 swap chain. Error code : " + INT_TO_STRING(result));

					return;
				}

#ifndef PLATFORM_WP8
				m_simpleVertexShader.compileVertexShaderFromFile("Data/Shaders/SimpleVertexShader.hlsl");

				m_d3dDevice11Context->IASetInputLayout(m_simpleVertexShader.inputLayout);

				m_simplePixelShader.compilePixelShaderFromFile("Data/Shaders/SimplePixelShader.hlsl");

				m_texturedVertexShader.compileVertexShaderFromFile("Data/Shaders/TexturedVertexShader.hlsl");

				m_texturedPixelShader.compilePixelShaderFromFile("Data/Shaders/TexturedPixelShader.hlsl");
#else
				m_simpleVertexShader.loadVertexShaderFromFile("SimpleVertexShader.cso");

				m_d3dDevice11Context->IASetInputLayout(m_simpleVertexShader.inputLayout);

				m_simplePixelShader.loadPixelShaderFromFile("SimplePixelShader.cso");

				m_texturedVertexShader.loadVertexShaderFromFile("TexturedVertexShader.cso");

				m_texturedPixelShader.loadPixelShaderFromFile("TexturedPixelShader.cso");
#endif

				D3D11_BUFFER_DESC bufferDesc;
				std::memset(&bufferDesc, 0, sizeof(bufferDesc));

				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

				bufferDesc.ByteWidth = max<int>(sizeof(Vertex), sizeof(Vertex)) * m_frameVertexBufferSize;

				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				result = m_d3dDevice11->CreateBuffer(&bufferDesc, 0, &m_vertexBuffer);

				if(FAILED(result))
				{
					eagle.error("Failed to create the vertex buffer. Error code : " + INT_TO_STRING(result));

					return;
				}

				UINT offset = 0;
				UINT stride = sizeof(Vertex);

				m_d3dDevice11Context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

				if(FAILED(result))
				{
					eagle.error("Failed to create the sampler state. Error code : " + INT_TO_STRING(result));

					return;
				}

				ID3D11BlendState *blendState;

				D3D11_BLEND_DESC transparentDesc;
				transparentDesc.AlphaToCoverageEnable = 0;
				transparentDesc.IndependentBlendEnable = 0;

				transparentDesc.RenderTarget[0].BlendEnable = 1;
				transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				// Additive
				/*transparentDesc.RenderTarget[0].BlendEnable = 1;
				transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_ALPHA;
				transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/

				result = m_d3dDevice11->CreateBlendState(&transparentDesc, &blendState);

				if(FAILED(result))
				{
					eagle.error("Failed to create the blending state. Error code : " + INT_TO_STRING(result));

					return;
				}

				float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

				m_d3dDevice11Context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

				D3D11_RASTERIZER_DESC rasterizerDesc2D;
				std::memset(&rasterizerDesc2D, 0, sizeof(D3D11_RASTERIZER_DESC));
				rasterizerDesc2D.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc2D.CullMode = D3D11_CULL_NONE;
				rasterizerDesc2D.DepthBias = 0;
				rasterizerDesc2D.SlopeScaledDepthBias = 0;
				rasterizerDesc2D.DepthBiasClamp = 0;
				rasterizerDesc2D.DepthClipEnable = 1;
				rasterizerDesc2D.ScissorEnable = 0;
				rasterizerDesc2D.MultisampleEnable = (m_multiSamplingSampleCount > 1);
				rasterizerDesc2D.AntialiasedLineEnable = 0;

				HRESULT hr = m_d3dDevice11->CreateRasterizerState(&rasterizerDesc2D, &m_rasterizerState2D);

				if(hr != S_OK)
				{
					eagle.outputLogEvent("Direct3D 11 Rasterizer state creation failed with code " + INT_TO_STRING(hr));
				}

				m_d3dDevice11Context->RSSetState(m_rasterizerState2D);

				D3D11_SAMPLER_DESC samplerDesc;
				ZeroMemory(&samplerDesc, sizeof(samplerDesc));
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				ID3D11SamplerState *samplerState;
				graphics.m_d3dDevice11->CreateSamplerState(&samplerDesc, &samplerState);

				m_d3dDevice11Context->PSSetSamplers(0, 1, &samplerState);

				eagle.outputLogEvent("Graphics API configuration complete");

				break;
			}
		}

		m_camera.initialize();
	}

#ifdef PLATFORM_WP8
	void GraphicsSystem::createWindowSizeDependentResources()
	{
		m_windowBounds = m_windowHandle->Bounds;

		// Calculate the necessary swap chain and render target size in pixels.
		m_renderTargetSize.Width = convertDipsToPixels(m_windowBounds.Width);
		m_renderTargetSize.Height = convertDipsToPixels(m_windowBounds.Height);

		m_screenWidth = m_renderTargetSize.Width;
		m_screenHeight = m_renderTargetSize.Height;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
		swapChainDesc.Width = static_cast<UINT>(m_renderTargetSize.Width); // Match the size of the window.
		swapChainDesc.Height = static_cast<UINT>(m_renderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
		swapChainDesc.Stereo = 0;
		swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1; // On phone, only single buffering is supported.
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // On phone, only stretch and aspect-ratio stretch scaling are allowed.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGIDevice1> dxgiDevice;
		m_d3dDevice11.As(&dxgiDevice);

		ComPtr<IDXGIAdapter> dxgiAdapter;
		dxgiDevice->GetAdapter(&dxgiAdapter);

		ComPtr<IDXGIFactory2> dxgiFactory;
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

		Windows::UI::Core::CoreWindow ^window = m_windowHandle.Get();

		dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice11.Get(), reinterpret_cast<IUnknown*>(window), &swapChainDesc, 0, &m_swapChain);

		dxgiDevice->SetMaximumFrameLatency(1);

		HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBufferTexture);

		if(FAILED(result))
		{
			eagle.error("Failed to get the swap chain back buffer. Error code : " + INT_TO_STRING(result));

			return;
		}

		result = m_d3dDevice11->CreateRenderTargetView(m_backBufferTexture.Get(), 0, &m_renderTargetView);

		//if(m_backBufferTexture) m_backBufferTexture->Release();

		if(FAILED(result))
		{
			eagle.error("Failed to create the render target view. Error code : " + INT_TO_STRING(result));

			return;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.Width = m_screenWidth;
		depthStencilDesc.Height = m_screenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		result = m_d3dDevice11->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilTexture);

		if(FAILED(result))
		{
			eagle.error("Failed to create the depth-stencil texture. Error code : " + INT_TO_STRING(result));
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
		ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
		m_depthStencilViewDesc.Format = depthStencilDesc.Format;
		m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		m_depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_d3dDevice11->CreateDepthStencilView(m_depthStencilTexture.Get(), 0, &m_depthStencilView);

		if(FAILED(result))
		{
			eagle.error("Failed to create the depth-stencil view. Error code : " + INT_TO_STRING(result));
		}

		m_d3dDevice11Context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

		D3D11_DEPTH_STENCIL_DESC dsDesc;

		// Depth test parameters
		dsDesc.DepthEnable = 1;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		// Stencil test parameters
		dsDesc.StencilEnable = 1;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create depth stencil state
		ID3D11DepthStencilState * pDSState;
		m_d3dDevice11->CreateDepthStencilState(&dsDesc, &pDSState);

		m_d3dDevice11Context->OMSetDepthStencilState(pDSState, 1);

		D3D11_VIEWPORT viewport;
		viewport.Width = m_screenWidth;
		viewport.Height = m_screenHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		m_d3dDevice11Context->RSSetViewports(1, &viewport);
	}

	void GraphicsSystem::releaseResourcesForSuspending()
	{
		m_swapChain = nullptr;
		m_renderTargetView = nullptr;
		m_depthStencilView = nullptr;
	}
#endif

	int GraphicsSystem::handleDeviceStatus()
	{
		return 1;
	}

	void GraphicsSystem::handleWindowResize()
	{
		if(!eagle.isInitialized())
		{
			return;
		}

#ifndef PLATFORM_WP8
		m_renderTargetView->Release();
		m_backBufferTexture->Release();
		m_depthStencilView->Release();
		m_depthStencilTexture->Release();

		m_swapChain->ResizeBuffers(0, m_screenWidth, m_screenHeight, DXGI_FORMAT_UNKNOWN, 0);

		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&m_backBufferTexture);
		m_d3dDevice11->CreateRenderTargetView(m_backBufferTexture, 0, &m_renderTargetView);

		D3D11_VIEWPORT viewport;
		viewport.Width = m_screenWidth;
		viewport.Height = m_screenHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		m_d3dDevice11Context->RSSetViewports(1, &viewport);

		m_swapChainWidth = m_screenWidth;
		m_swapChainHeight = m_screenHeight;

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		std::memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
		depthStencilDesc.Width = m_screenWidth;
		depthStencilDesc.Height = m_screenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = m_multiSamplingSampleCount;
		depthStencilDesc.SampleDesc.Quality = m_multiSamplingQualityLevel;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		m_d3dDevice11->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilTexture);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		std::memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = (m_multiSamplingSampleCount > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		m_d3dDevice11->CreateDepthStencilView(m_depthStencilTexture, &depthStencilViewDesc, &m_depthStencilView);

		m_d3dDevice11Context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

//#ifndef PLATFORM_WP8
//		m_swapChain->Release();
//		m_renderTargetView->Release();
//		m_depthStencilView->Release();
//#endif
//
//		while(!createWindowSizeDependentObjects())
//		{
//			eagle.sleep(100);
//		}

		if(console.showing)
		{
			if(!console.mainPanelLocked)
			{
				console.position.y = m_screenHeight - console.height;
			}

			if(!console.informationPanelLocked)
			{
				console.informationPanelPosition.y = 0;
			}
		}
		else
		{
			if(!console.mainPanelLocked)
			{
				console.position.y = m_screenHeight;
			}

			if(!console.informationPanelLocked)
			{
				console.informationPanelPosition.y = -console.informationPanelHeight;
			}
		}
#endif
	}

	bool GraphicsSystem::handleFullscreenStateChange()
	{
		m_swapChainFullscreen = m_fullscreen;

		while(FAILED(m_swapChain->SetFullscreenState(m_fullscreen, 0)))
		{
			eagle.sleep(100);
		}

		return 1;
	}

	void GraphicsSystem::enableAntiAliasing()
	{
		if(!m_multiSamplingSupport)
		{
			return;
		}

#ifndef PLATFORM_WP8

		if(eagle.isInitialized() && !m_multiSamplingEnabled)
		{
			m_multiSamplingEnabled = 1;

			m_swapChain->Release();
			m_renderTargetView->Release();
			m_depthStencilView->Release();

			createWindowSizeDependentObjects();
		}

		m_multiSamplingEnabled = 1;

#endif
	}

	void GraphicsSystem::disableAntiAliasing()
	{
		if(!m_multiSamplingSupport)
		{
			return;
		}

#ifndef PLATFORM_WP8

		if(eagle.isInitialized() && m_multiSamplingEnabled)
		{
			m_multiSamplingEnabled = 0;

			m_swapChain->Release();
			m_renderTargetView->Release();
			m_depthStencilView->Release();

			createWindowSizeDependentObjects();
		}

		m_multiSamplingEnabled = 0;

#endif
	}

	void GraphicsSystem::toggleAntiAliasing()
	{
		if(!m_multiSamplingSupport)
		{
			return;
		}

#ifndef PLATFORM_WP8

		m_multiSamplingEnabled = !m_multiSamplingEnabled;

		if(eagle.isInitialized())
		{
			m_swapChain->Release();
			m_renderTargetView->Release();
			m_depthStencilView->Release();

			createWindowSizeDependentObjects();
		}

#endif
	}

	void GraphicsSystem::setAntiAliasingState(bool state)
	{
		if(!m_multiSamplingSupport)
		{
			return;
		}

		m_multiSamplingEnabled = state;

		switch(m_graphicsAPIType)
		{
		}
	}

	bool GraphicsSystem::isAntiAliasingEnabled()
	{
		if(!m_multiSamplingSupport)
		{
			return 0;
		}

		return m_multiSamplingEnabled;
	}

	int GraphicsSystem::getMultiSamplingSampleCount()
	{
		return m_multiSamplingSampleCount;
	}

	int GraphicsSystem::getMultiSamplingQualityLevel()
	{
		return m_multiSamplingQualityLevel;
	}



	bool GraphicsSystem::isWindowResizable()
	{
#ifndef PLATFORM_WP8
		return m_windowResizable;
#else
		return 0;
#endif
	}

	void GraphicsSystem::setWindowResizable(bool value)
	{
#ifndef PLATFORM_WP8
		m_windowResizable = value;

		DWORD dwStyle = GetWindowLong(m_windowHandle, GWL_STYLE);

		dwStyle = WINDOW_WINDOWED_STYLE;

		if(m_windowResizable)
		{
			dwStyle |= WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		}

		SetWindowLong(m_windowHandle, GWL_STYLE, dwStyle);
#endif
	}

	void GraphicsSystem::setIdentity()
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				if(DirectX::XMMatrixIsIdentity(m_worldMatrix) && DirectX::XMMatrixIsIdentity(m_viewMatrix) && DirectX::XMMatrixIsIdentity(m_projectionMatrix))
				{
					return;
				}

				m_worldMatrix = DirectX::XMMatrixIdentity();;
				m_viewMatrix = DirectX::XMMatrixIdentity();;
				m_projectionMatrix = DirectX::XMMatrixIdentity();;

				CBChangesEveryFrame cb;
				cb.world = XMMatrixTranspose(m_worldMatrix);
				cb.view = XMMatrixTranspose(m_viewMatrix);
				cb.projection = XMMatrixTranspose(m_projectionMatrix);

				m_d3dDevice11Context->UpdateSubresource(m_texturedVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);
				m_d3dDevice11Context->UpdateSubresource(m_simpleVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);

				m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &m_texturedVertexShader.constantBuffer);
				//m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &simpleVertexShader.constantBuffer);

				break;
			}
		}
	}

	void GraphicsSystem::setWorldIdentity()
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				DrawCallData drawCallData;
				drawCallData.worldMatrixChange = 1;
				drawCallData.worldMatrix = DirectX::XMMatrixTranslation(0, 0, 0);

				m_drawCallBuffer[m_drawCallCount] = drawCallData;

				m_drawCallCount++;

				break;

				break;
			}
		}
	}

	void GraphicsSystem::setWorldTransform(DirectX::XMMATRIX *matrix)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				DrawCallData drawCallData;
				drawCallData.worldMatrixChange = 1;
				drawCallData.worldMatrix = *matrix;

				m_drawCallBuffer[m_drawCallCount] = drawCallData;

				m_drawCallCount++;

				break;
			}
		}
	}

	void GraphicsSystem::setViewTransform(DirectX::XMMATRIX *matrix)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				m_viewMatrix = *matrix;

				CBChangesEveryFrame cb;
				cb.world = XMMatrixTranspose(m_worldMatrix);
				cb.view = XMMatrixTranspose(m_viewMatrix);
				cb.projection = XMMatrixTranspose(m_projectionMatrix);

				m_d3dDevice11Context->UpdateSubresource(m_texturedVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);
				m_d3dDevice11Context->UpdateSubresource(m_simpleVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);

				m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &m_texturedVertexShader.constantBuffer);
				//m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &simpleVertexShader.constantBuffer);

				break;
			}
		}
	}

	void GraphicsSystem::setProjectionTransform(DirectX::XMMATRIX *matrix)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				m_projectionMatrix = *matrix;

				CBChangesEveryFrame cb;
				cb.world = XMMatrixTranspose(m_worldMatrix);
				cb.view = XMMatrixTranspose(m_viewMatrix);
				cb.projection = XMMatrixTranspose(m_projectionMatrix);

				m_d3dDevice11Context->UpdateSubresource(m_texturedVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);
				m_d3dDevice11Context->UpdateSubresource(m_simpleVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);

				m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &m_texturedVertexShader.constantBuffer);
				//m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &simpleVertexShader.constantBuffer);

				break;
			}
		}
	}

	void GraphicsSystem::takeScreenshot()
	{
#ifndef PLATFORM_WP8
		renderBufferedDrawCalls();

		static int lastIndex = 0;

		FILE *f;
		char fileName[512];

		for(int i = lastIndex; ; ++i)
		{
			sprintf(fileName, "Screenshots/Screenshot%.3d.jpg", i);
			f = fopen(fileName, "r");
			if(f == 0)
			{
				lastIndex = i;

				break;
			}
			else
			{
				fclose(f);
			}
		}

		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				D3DX11SaveTextureToFileA(m_d3dDevice11Context, m_backBufferTexture, D3DX11_IFF_JPG, fileName);

				break;
			}
		}
#endif
	}

	Camera *GraphicsSystem::getCamera()
	{
		return &m_camera;
	}

	std::string GraphicsSystem::getDisplayAdapterName()
	{
		return m_adapterName;
	}

	int GraphicsSystem::getScreenWidth() 
	{
		return m_screenWidth;
	}

	void GraphicsSystem::setScreenWidth(int value) 
	{
		m_screenWidth = value;
	}

	int GraphicsSystem::getScreenHeight()
	{
		return m_screenHeight;
	}

	void GraphicsSystem::setScreenHeight(int value) 
	{
		m_screenHeight = value;
	}

	Vector2 GraphicsSystem::getScreenDimensions()
	{
		return Vector2(m_screenWidth, m_screenHeight);
	}

	void GraphicsSystem::setScreenDimensions(int width, int height)
	{
		m_screenWidth = width;
		m_screenHeight = height;
	}

	void GraphicsSystem::setScreenDimensions(Vector2 value)
	{
		m_screenWidth = value.x;
		m_screenHeight = value.y;
	}

	void GraphicsSystem::resetWindow()
	{
#ifndef PLATFORM_WP8
		DWORD dwStyle;
		RECT windowRectangle;

		windowRectangle.left = 0l;
		windowRectangle.right = (long)graphics.getScreenWidth();
		windowRectangle.top = 0l;
		windowRectangle.bottom = (long)graphics.getScreenHeight();

		dwStyle = GetWindowLong(m_windowHandle, GWL_STYLE);

		/*if(m_fullscreen)
		{
			dwStyle = WINDOW_FULLSCREEN_STYLE;
		}
		else
		{
			dwStyle = GetWindowLong(m_windowHandle, GWL_STYLE);

			if(dwStyle & WS_MAXIMIZE)
			{
				maximized = 1;
			}

			dwStyle = WINDOW_WINDOWED_STYLE;

			if(m_windowResizable)
			{
				dwStyle |= WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			}

			if(maximized)
			{
				dwStyle |= WS_MAXIMIZE;
			}
		}*/

		//SetWindowLong(m_windowHandle, GWL_STYLE, dwStyle);

		//if(!m_fullscreen)	
		{
			AdjustWindowRect(&windowRectangle, dwStyle, 0);
		}

		RECT windowPositionRect;
		GetWindowRect(m_windowHandle, &windowPositionRect);

		MoveWindow(m_windowHandle, windowPositionRect.left, windowPositionRect.top, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, 1);

		//console.print(INT_TO_STRING(windowRectangle.right - windowRectangle.left) + ", " + INT_TO_STRING(windowRectangle.bottom - windowRectangle.top));
#endif
	}

	int GraphicsSystem::getClientScreenWidth() 
	{
		return m_clientScreenWidth;
	}

	int GraphicsSystem::getClientScreenHeight() 
	{
		return m_clientScreenHeight;
	}

	bool GraphicsSystem::getFullscreen() 
	{
		return m_fullscreen;
	}

	void GraphicsSystem::setFullscreen(bool value) 
	{
		m_fullscreen = value;
	}

	Vector2 GraphicsSystem::getWindowPosition()
	{
#ifndef PLATFORM_WP8
		RECT windowPositionRect;
		GetWindowRect(m_windowHandle, &windowPositionRect);

		return Vector2(windowPositionRect.left, windowPositionRect.top);
#else
		return Vector2(0, 0);
#endif
	}

	void GraphicsSystem::setWindowPosition(Vector2 value)
	{
#ifndef PLATFORM_WP8
		DWORD dwStyle;
		RECT windowRectangle;

		windowRectangle.left = 0l;
		windowRectangle.right = (long)graphics.getScreenWidth();
		windowRectangle.top = 0l;
		windowRectangle.bottom = (long)graphics.getScreenHeight();

		dwStyle = GetWindowLong(m_windowHandle, GWL_STYLE);

		if(!m_fullscreen)	
		{
			AdjustWindowRect(&windowRectangle, dwStyle, 0);
		}

		/*RECT windowPositionRect;
		GetWindowRect(m_windowHandle, &windowPositionRect);*/

		//Vector2 movement = value - Vector2(windowPositionRect.left, windowPositionRect.top);

		MoveWindow(m_windowHandle, value.x, value.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, 1);
#endif
	}

	bool GraphicsSystem::getCursorVisibility()
	{
		return m_cursorVisibility;
	}

	void GraphicsSystem::setCursorVisibility(bool value)
	{
		m_cursorVisibility = value;
	}

	Vector2 GraphicsSystem::getCursorPosition()
	{
#ifndef PLATFORM_WP8
		POINT cursorPosition;
		GetCursorPos(&cursorPosition);

		return Vector2(cursorPosition.x, cursorPosition.y);
#else
		return Vector2(0, 0);
#endif
	}

	void GraphicsSystem::setCursorPosition(Vector2 value)
	{
#ifndef PLATFORM_WP8
		SetCursorPos(value.x, value.y);
#endif
	}

	void GraphicsSystem::setWindowAlpha(float value)
	{
#ifndef PLATFORM_WP8
		SetWindowLong(m_windowHandle, GWL_EXSTYLE, GetWindowLong(m_windowHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(m_windowHandle, 0, 255 * value, LWA_ALPHA);
#endif
	}

	bool GraphicsSystem::renderStart()
	{
#ifndef PLATFORM_WP8
		if(m_screenWidth != m_swapChainWidth || m_screenHeight != m_swapChainHeight)
		{
			bool wasWindowResizable = m_windowResizable;

			m_windowResizable = 0;

			if(m_fullscreen)
			{
				m_fullscreen = 0;

				handleFullscreenStateChange();

				m_fullscreen = 1;
			}

			resetWindow();

			handleWindowResize();

			if(m_fullscreen)
			{
				handleFullscreenStateChange();
			}

			m_windowResizable = wasWindowResizable;
		}

		if(m_fullscreen != m_swapChainFullscreen)
		{
			resetWindow();

			handleFullscreenStateChange();
		}
#endif

		m_textureTopLeft.set(0, 0);
		m_textureBottomRight.set(1, 1);

		setIdentity();

		m_camera.update();

		return 1;
	}

	bool GraphicsSystem::renderStop()
	{
		if(m_isDeviceLost)
		{
			return 1;
		}

		renderBufferedDrawCalls();

		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
#ifndef PLATFORM_WP8
				m_swapChain->Present(1, 0);
#else
				m_swapChain->Present(1, 0);
				
				m_d3dDevice11Context->DiscardView(m_renderTargetView.Get());
				m_d3dDevice11Context->DiscardView(m_depthStencilView.Get());
#endif
				break;
			}
		}

		return 1;
	}

	void GraphicsSystem::renderBufferedDrawCalls()
	{
		m_renderingBufferedDrawCalls = 1;

		int vertexStartLocation = 0;

		float halfScreenHeight = m_screenHeight / 2.0;
		float negativeHalfScreenHeight = - halfScreenHeight;
		float halfScreenWidth = m_screenWidth / 2;
		float aspectRatio = (float)m_screenWidth / m_screenHeight;

		for(int i = 0; i < m_drawCallCount; ++i)
		{
			if(m_drawCallBuffer[i].worldMatrixChange)
			{
				continue;
			}

			int vertexCount = m_drawCallBuffer[i].vertexCount * m_drawCallBuffer[i].primitiveGroupSize;

			for(int j = 0; j < vertexCount; ++j)
			{
				if(m_drawCallBuffer[i].isTransformed)
				{
					m_frameVertexBuffer[vertexStartLocation + j].x /= halfScreenWidth;
					m_frameVertexBuffer[vertexStartLocation + j].x -= 1;
					m_frameVertexBuffer[vertexStartLocation + j].y /= negativeHalfScreenHeight;
					m_frameVertexBuffer[vertexStartLocation + j].y += 1;
				}
				else
				{
					m_frameVertexBuffer[vertexStartLocation + j].x /= halfScreenHeight;
					m_frameVertexBuffer[vertexStartLocation + j].x -= aspectRatio;
					m_frameVertexBuffer[vertexStartLocation + j].y /= negativeHalfScreenHeight;
					m_frameVertexBuffer[vertexStartLocation + j].y += 1;
					m_frameVertexBuffer[vertexStartLocation + j].z /= halfScreenHeight;
				}
			}

			vertexStartLocation += m_drawCallBuffer[i].vertexCount * m_drawCallBuffer[i].primitiveGroupSize;
		}

		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		D3D11_MAPPED_SUBRESOURCE ms;
		m_d3dDevice11Context->Map(m_vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, m_frameVertexBuffer, m_frameVertexBuffer_CurrentVertexIndex * sizeof(Vertex));
		m_d3dDevice11Context->Unmap(m_vertexBuffer, NULL);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		m_d3dDevice11Context->IASetInputLayout(m_simpleVertexShader.inputLayout);

		m_d3dDevice11Context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		m_d3dDevice11Context->IASetPrimitiveTopology(primitiveTopology);

		vertexStartLocation = 0;

		for(int i = 0; i < m_drawCallCount; ++i)
		{
			if(m_drawCallBuffer[i].worldMatrixChange)
			{
				m_worldMatrix = m_drawCallBuffer[i].worldMatrix;

				CBChangesEveryFrame cb;
				cb.world = XMMatrixTranspose(m_worldMatrix);
				cb.view = XMMatrixTranspose(m_viewMatrix);
				cb.projection = XMMatrixTranspose(m_projectionMatrix);

				m_d3dDevice11Context->UpdateSubresource(m_texturedVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);
				m_d3dDevice11Context->UpdateSubresource(m_simpleVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);

				m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &m_texturedVertexShader.constantBuffer);

				continue;
			}

			setTexture(m_drawCallBuffer[i].texture);

			if(m_textureEnabled)
			{
				m_d3dDevice11Context->VSSetShader(m_texturedVertexShader.vertexShader, 0, 0);
				m_d3dDevice11Context->PSSetShader(m_texturedPixelShader.pixelShader, 0, 0);
				m_d3dDevice11Context->PSSetShaderResources(0, 1, &m_currentTexture->m_shaderResourceView);
			}
			else
			{
				m_d3dDevice11Context->VSSetShader(m_simpleVertexShader.vertexShader, 0, 0);
				m_d3dDevice11Context->PSSetShader(m_simplePixelShader.pixelShader, 0, 0);
			}

			if(m_drawCallBuffer[i].isTransformed)
			{
				setIdentity();

				for(int j = 0; j < m_drawCallBuffer[i].primitiveGroupSize; ++j)
				{
					m_d3dDevice11Context->Draw(m_drawCallBuffer[i].vertexCount, vertexStartLocation + j * m_drawCallBuffer[i].vertexCount);
				}

				setViewTransform(m_camera.getViewMatrix());
				setProjectionTransform(m_camera.getProjectionMatrix());
			}
			else
			{
				/*if(!DirectX::XMMatrixIsIdentity(m_drawCallBuffer[i].worldMatrix))
				{
					DirectX::XMMATRIX matrix = m_drawCallBuffer[i].worldMatrix;
					setWorldTransform(&matrix);
				}
				else if(!DirectX::XMMatrixIsIdentity(m_worldMatrix))
				{
					m_worldMatrix = DirectX::XMMatrixTranslation(0, 0, 0);

					CBChangesEveryFrame cb;
					cb.world = XMMatrixTranspose(m_worldMatrix);
					cb.view = XMMatrixTranspose(m_viewMatrix);
					cb.projection = XMMatrixTranspose(m_projectionMatrix);

					m_d3dDevice11Context->UpdateSubresource(m_texturedVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);
					m_d3dDevice11Context->UpdateSubresource(m_simpleVertexShader.constantBuffer, 0, nullptr, &cb, 0, 0);

					m_d3dDevice11Context->VSSetConstantBuffers(0, 1, &m_texturedVertexShader.constantBuffer);
				}*/

				for(int j = 0; j < m_drawCallBuffer[i].primitiveGroupSize; ++j)
				{
					m_d3dDevice11Context->Draw(m_drawCallBuffer[i].vertexCount, vertexStartLocation + j * m_drawCallBuffer[i].vertexCount);
				}
			}

			vertexStartLocation += m_drawCallBuffer[i].vertexCount * m_drawCallBuffer[i].primitiveGroupSize;
		}

		m_drawCallCount = 0;
		m_frameVertexBuffer_CurrentVertexIndex = 0;

		setTexture(0);

		m_renderingBufferedDrawCalls = 0;
	}

	void GraphicsSystem::drawPrimitive(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int startVertexIndex)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				int vertexCount = 2 + primitiveCount;
				Texture *texture = m_currentTexture;

				if(m_drawCallCount == m_drawCallBufferSize || vertexCount + m_frameVertexBuffer_CurrentVertexIndex > m_frameVertexBufferSize)
				{
					if(vertexCount > m_frameVertexBufferSize)
					{
						return;
					}
					else
					{
						renderBufferedDrawCalls();

						setTexture(texture);
					}
				}

				DrawCallData drawCallData;
				drawCallData.primitiveCount = primitiveCount;
				drawCallData.vertexCount = vertexCount;
				drawCallData.texture = m_currentTexture;
				drawCallData.isTransformed = 0;
				drawCallData.primitiveGroupSize = 1;
				drawCallData.worldMatrixChange = 0;

				m_drawCallBuffer[m_drawCallCount] = drawCallData;

				memcpy(&m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex], &vertexList[startVertexIndex], vertexCount * sizeof(Vertex));

				m_frameVertexBuffer_CurrentVertexIndex += vertexCount;

				m_drawCallCount++;

				break;
			}
		}
	}

	void GraphicsSystem::drawTransformedPrimitive(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int startVertexIndex)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				int vertexCount = 2 + primitiveCount;
				Texture *texture = m_currentTexture;

				if(m_drawCallCount == m_drawCallBufferSize || vertexCount + m_frameVertexBuffer_CurrentVertexIndex > m_frameVertexBufferSize)
				{
					if(vertexCount > m_frameVertexBufferSize)
					{
						return;
					}
					else
					{
						renderBufferedDrawCalls();

						setTexture(texture);
					}
				}

				DrawCallData drawCallData;
				drawCallData.primitiveCount = primitiveCount;
				drawCallData.vertexCount = vertexCount;
				drawCallData.texture = m_currentTexture;
				drawCallData.isTransformed = 1;
				drawCallData.primitiveGroupSize = 1;
				drawCallData.worldMatrixChange = 0;

				m_drawCallBuffer[m_drawCallCount] = drawCallData;

				memcpy(&m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex], &vertexList[startVertexIndex], vertexCount * sizeof(Vertex));

				/*for(int i = 0; i < vertexCount; ++i)
				{
					m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex + i] = vertexList[i + startVertexIndex];
				}*/

				m_frameVertexBuffer_CurrentVertexIndex += vertexCount;

				m_drawCallCount++;

				break;
			}
		}
	}

	void GraphicsSystem::drawPrimitiveList(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int objectCount, int startVertexIndex)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				int vertexCount = 2 + primitiveCount;
				Texture *texture = m_currentTexture;

				if(m_drawCallCount == m_drawCallBufferSize || vertexCount * objectCount + m_frameVertexBuffer_CurrentVertexIndex > m_frameVertexBufferSize)
				{
					if(vertexCount * objectCount > m_frameVertexBufferSize)
					{
						if(vertexCount > m_frameVertexBufferSize)
						{
							return;
						}
						else
						{
							int batchSize = (int)(m_frameVertexBufferSize / vertexCount);

							for(int i = 0; i < objectCount; i += batchSize)
							{
								if(m_currentTexture != texture)
								{
									m_currentTexture = texture;
								}

								int currentBatchSize = min<int>(batchSize, objectCount - i);
								//drawPrimitive(vertexList, primitiveCount, primitiveType, i * vertexCount);
								drawPrimitiveList(vertexList, primitiveCount, primitiveType, currentBatchSize, i * vertexCount);
							}

							return;
						}
					}
					else
					{
						renderBufferedDrawCalls();

						setTexture(texture);
					}
				}

				DrawCallData drawCallData;
				drawCallData.primitiveCount = primitiveCount;
				drawCallData.vertexCount = vertexCount;
				drawCallData.texture = m_currentTexture;
				drawCallData.isTransformed = 0;
				drawCallData.primitiveGroupSize = objectCount;
				drawCallData.worldMatrixChange = 0;

				m_drawCallBuffer[m_drawCallCount] = drawCallData;

				memcpy(&m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex], &vertexList[startVertexIndex], vertexCount * objectCount * sizeof(Vertex));

				/*for(int i = 0; i < vertexCount * objectCount; ++i)
				{
					m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex + i] = vertexList[i + startVertexIndex];
				}*/

				m_frameVertexBuffer_CurrentVertexIndex += vertexCount * objectCount;

				m_drawCallCount++;

				break;
			}
		}
	}

	void GraphicsSystem::drawTransformedPrimitiveList(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int objectCount, int startVertexIndex)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				int vertexCount = 2 + primitiveCount;
				Texture *texture = m_currentTexture;

				if(m_drawCallCount == m_drawCallBufferSize || vertexCount * objectCount + m_frameVertexBuffer_CurrentVertexIndex > m_frameVertexBufferSize)
				{
					if(vertexCount * objectCount > m_frameVertexBufferSize)
					{
						if(vertexCount > m_frameVertexBufferSize)
						{
							return;
						}
						else
						{
							int batchSize = (int)(m_frameVertexBufferSize / vertexCount);

							for(int i = 0; i < objectCount; i += batchSize)
							{
								if(m_currentTexture != texture)
								{
									m_currentTexture = texture;
								}

								int currentBatchSize = min<int>(batchSize, objectCount - i);
								drawTransformedPrimitiveList(vertexList, primitiveCount, primitiveType, currentBatchSize, i * vertexCount);
							}

							return;
						}
					}
					else
					{
						renderBufferedDrawCalls();

						setTexture(texture);
					}
				}

				DrawCallData drawCallData;
				drawCallData.primitiveCount = primitiveCount;
				drawCallData.vertexCount = vertexCount;
				drawCallData.texture = m_currentTexture;
				drawCallData.isTransformed = 1;
				drawCallData.primitiveGroupSize = objectCount;
				drawCallData.worldMatrixChange = 0;

				m_drawCallBuffer[m_drawCallCount] = drawCallData;

				memcpy(&m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex], &vertexList[startVertexIndex], vertexCount * objectCount * sizeof(Vertex));

				/*for(int i = 0; i < vertexCount * objectCount; ++i)
				{
					m_frameVertexBuffer[m_frameVertexBuffer_CurrentVertexIndex + i] = vertexList[i + startVertexIndex];
				}*/

				m_frameVertexBuffer_CurrentVertexIndex += vertexCount * objectCount;

				m_drawCallCount++;

				break;
			}
		}
	}

	void GraphicsSystem::clearScene(ColorValue color)
	{
		switch(m_graphicsAPIType)
		{
		case GraphicsAPI_Direct3D11:
			{
				float colorList[4];
				colorList[0] = color.r;
				colorList[1] = color.g;
				colorList[2] = color.b;
				colorList[3] = color.a;

#ifndef PLATFORM_WP8
				m_d3dDevice11Context->ClearRenderTargetView(m_renderTargetView, colorList);
				m_d3dDevice11Context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#else
				m_d3dDevice11Context->ClearRenderTargetView(m_renderTargetView.Get(), colorList);
				m_d3dDevice11Context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#endif

				break;
			}
		}
	}

	void GraphicsSystem::setTexture(Texture *texture)
	{
		if(!m_renderingBufferedDrawCalls)
		{
			m_currentTexture = texture;

			return;
		}

		if(texture)
		{
			if(texture->m_loaded)
			{
				m_textureEnabled = 1;

				/*m_d3dDevice11Context->VSSetShader(m_texturedVertexShader.vertexShader, 0, 0);
				m_d3dDevice11Context->PSSetShader(m_texturedPixelShader.pixelShader, 0, 0);
				m_d3dDevice11Context->PSSetShaderResources(0, 1, &texture->m_shaderResourceView);*/

				m_currentTexture = texture;
			}
			else
			{
				m_textureEnabled = 0;

				/*m_d3dDevice11Context->VSSetShader(m_simpleVertexShader.vertexShader, 0, 0);
				m_d3dDevice11Context->PSSetShader(m_simplePixelShader.pixelShader, 0, 0);*/

				m_currentTexture = 0;
			}
		}
		else
		{
			m_textureEnabled = 0;

			/*m_d3dDevice11Context->VSSetShader(m_simpleVertexShader.vertexShader, 0, 0);
			m_d3dDevice11Context->PSSetShader(m_simplePixelShader.pixelShader, 0, 0);*/

			m_currentTexture = 0;
		}

	}

	void GraphicsSystem::setTextureSourceRectangle(Vector2 topLeft, Vector2 bottomRight)
	{
		m_textureTopLeft = topLeft;
		m_textureBottomRight = bottomRight;
	}

	Vector2 GraphicsSystem::getTextureTopLeft()
	{
		return m_textureTopLeft;
	}

	Vector2 GraphicsSystem::getTextureBottomRight()
	{
		return m_textureBottomRight;
	}

	void GraphicsSystem::drawLine(float x1, float y1, float x2, float y2, int thickness, ColorValue color, float depth)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		m_lastPoint.x = x2;
		m_lastPoint.y = y2;

		Vector2 normalVector = Vector2(x2 - x1, y2 - y1).normal();

		Vertex v[4];

		v[0].x = x1 - normalVector.y * ((float)thickness / 2);
		v[0].y = y1 + normalVector.x * ((float)thickness / 2);
		v[0].z = depth;
		v[0].color = color;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x1 + normalVector.y * ((float)thickness / 2);
		v[1].y = y1 - normalVector.x * ((float)thickness / 2);
		v[1].z = depth;
		v[1].color = color;
		v[1].tu = tu0;
		v[1].tv = tv1;
		v[2].x = x2 - normalVector.y * ((float)thickness / 2);
		v[2].y = y2 + normalVector.x * ((float)thickness / 2);
		v[2].z = depth;
		v[2].color = color;
		v[2].tu = tu1;
		v[2].tv = tv0;
		v[3].x = x2 + normalVector.y * ((float)thickness / 2);
		v[3].y = y2 - normalVector.x * ((float)thickness / 2);
		v[3].z = depth;
		v[3].color = color;
		v[3].tu = tu1;
		v[3].tv = tv1;

		drawPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::drawLineTransformed(float x1, float y1, float x2, float y2, int thickness, ColorValue color, float depth)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		m_lastPoint.x = x2;
		m_lastPoint.y = y2;

		Vector2 normalVector = Vector2(x2 - x1, y2 - y1).normal();

		Vertex v[4];

		v[0].x = x1 - normalVector.y * ((float)thickness / 2);
		v[0].y = y1 + normalVector.x * ((float)thickness / 2);
		v[0].z = depth;
		v[0].color = color;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x1 + normalVector.y * ((float)thickness / 2);
		v[1].y = y1 - normalVector.x * ((float)thickness / 2);
		v[1].z = depth;
		v[1].color = color;
		v[1].tu = tu0;
		v[1].tv = tv1;
		v[2].x = x2 - normalVector.y * ((float)thickness / 2);
		v[2].y = y2 + normalVector.x * ((float)thickness / 2);
		v[2].z = depth;
		v[2].color = color;
		v[2].tu = tu1;
		v[2].tv = tv0;
		v[3].x = x2 + normalVector.y * ((float)thickness / 2);
		v[3].y = y2 - normalVector.x * ((float)thickness / 2);
		v[3].z = depth;
		v[3].color = color;
		v[3].tu = tu1;
		v[3].tv = tv1;

		drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::drawLineGradient(float x1, float y1, float x2, float y2, int thickness0, int thickness1, ColorValue color0, ColorValue color1, float z)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		z *= -1;

		m_lastPoint.x = x2;
		m_lastPoint.y = y2;

		Vector2 linearVelocity = math.linearVelocity(math.angleToTarget(x1, y1, x2, y2) - pi / 2);

		Vertex v[4];
		v[0].x = x1 - linearVelocity.x * ((float)thickness0 / 2);
		v[0].y = y1 - linearVelocity.y * ((float)thickness0 / 2);
		v[0].z = z;
		v[0].color = color0;
		//v[0].weight = 1;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x1 + linearVelocity.x * ((float)thickness0 / 2);
		v[1].y = y1 + linearVelocity.y * ((float)thickness0 / 2);
		v[1].z = z;
		v[1].color = color0;
		//v[1].weight = 1;
		v[1].tu = tu0;
		v[1].tv = tv1;
		v[2].x = x2 - linearVelocity.x * ((float)thickness1 / 2);
		v[2].y = y2 - linearVelocity.y * ((float)thickness1 / 2);
		v[2].z = z;
		v[2].color = color1;
		//v[2].weight = 1;
		v[2].tu = tu1;
		v[2].tv = tv0;
		v[3].x = x2 + linearVelocity.x * ((float)thickness1 / 2);
		v[3].y = y2 + linearVelocity.y * ((float)thickness1 / 2);
		v[3].z = z;
		v[3].color = color1;
		//v[3].weight = 1;
		v[3].tu = tu1;
		v[3].tv = tv1;

		//d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)v, sizeof(Vertex));

		drawPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::drawLineGradientTransformed(float x1, float y1, float x2, float y2, int thickness0, int thickness1, ColorValue color0, ColorValue color1, float z)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		m_lastPoint.x = x2;
		m_lastPoint.y = y2;

		Vector2 linearVelocity = math.linearVelocity(math.angleToTarget(x1, y1, x2, y2) - pi / 2);

		Vertex v[4];

		v[0].x = x1 - linearVelocity.x * ((float)thickness0 / 2);
		v[0].y = y1 - linearVelocity.y * ((float)thickness0 / 2);
		v[0].z = z;
		v[0].color = color0;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x1 + linearVelocity.x * ((float)thickness0 / 2);
		v[1].y = y1 + linearVelocity.y * ((float)thickness0 / 2);
		v[1].z = z;
		v[1].color = color0;
		v[1].tu = tu0;
		v[1].tv = tv1;
		v[2].x = x2 - linearVelocity.x * ((float)thickness1 / 2);
		v[2].y = y2 - linearVelocity.y * ((float)thickness1 / 2);
		v[2].z = z;
		v[2].color = color1;
		v[2].tu = tu1;
		v[2].tv = tv0;
		v[3].x = x2 + linearVelocity.x * ((float)thickness1 / 2);
		v[3].y = y2 + linearVelocity.y * ((float)thickness1 / 2);
		v[3].z = z;
		v[3].color = color1;
		v[3].tu = tu1;
		v[3].tv = tv1;

		//d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(TRANSFORMED_VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)v, sizeof(Vertex));
		drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::drawLineToLastPoint(float x, float y, int thickness, ColorValue color, float depth)
	{
		//if(!dev) return;

		drawLine(m_lastPoint.x, m_lastPoint.y, x, y, thickness, color, depth);

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::drawLineToLastPointTransformed(float x, float y, int thickness, ColorValue color, float depth)
	{
		//if(!dev) return;

		drawLineTransformed(m_lastPoint.x, m_lastPoint.y, x, y, thickness, color, depth);

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::drawPoint(float x, float y, ColorValue color, float depth)
	{
		/*Vertex v;
		v.x = x;
		v.y = y;
		v.z = depth * -1;
		v.color = color;
		v.tu = tu0;
		v.tv = tv0;

		drawPrimitive(&v, 1, PrimitiveType_PointList);*/

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::drawPointTransformed(float x, float y, ColorValue color, float depth)
	{
		/*Vertex v;
		v.x = x;
		v.y = y;
		v.z = depth;
		v.weight = 1;
		v.color = color;
		v.tu = tu0;
		v.tv = tv0;

		drawTransformedPrimitive(&v, 2, PrimitiveType_PointList);*/

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::drawRectangle(float left, float top, float width, float height, int thickness, ColorValue color, float depth)
	{
		if(thickness == 0)
		{
			return;
		}

		int v = (height) / math.abs(height);
		int h = (width) / math.abs(width);
		int thicknessOnTwo = thickness / 2;
		thickness = (thicknessOnTwo + 0.5) * 2;

		drawLine(left, top + v * thicknessOnTwo, left + width, top + v * thicknessOnTwo, thickness, color, depth);
		drawLine(left + h * thicknessOnTwo, top, left + h * thicknessOnTwo, top + height, thickness, color, depth);
		drawLine(left + width, top + height - v * thicknessOnTwo, left, top + height - v * thicknessOnTwo, thickness, color, depth);
		drawLine(left + width - h * thicknessOnTwo, top + height, left + width - h * thicknessOnTwo, top, thickness, color, depth);
	}

	void GraphicsSystem::drawRectangleTransformed(float left, float top, float width, float height, int thickness, ColorValue color, float depth)
	{
		if(thickness == 0)
		{
			return;
		}

		int v = (height) / math.abs(height);
		int h = (width) / math.abs(width);
		int thicknessOnTwo = thickness / 2;
		thickness = (thicknessOnTwo + 0.5) * 2;

		drawLineTransformed(left, top + v * thicknessOnTwo, left + width, top + v * thicknessOnTwo, thickness, color, depth);
		drawLineTransformed(left + h * thicknessOnTwo, top, left + h * thicknessOnTwo, top + height, thickness, color, depth);
		drawLineTransformed(left + width, top + height - v * thicknessOnTwo, left, top + height - v * thicknessOnTwo, thickness, color, depth);
		drawLineTransformed(left + width - h * thicknessOnTwo, top + height, left + width - h * thicknessOnTwo, top, thickness, color, depth);
	}

	void GraphicsSystem::drawRotatedRectangle(float left, float top, float width, float height, float centerX, float centerY, float rotation, int thickness, ColorValue color, float depth)
	{
		if(thickness == 0)
		{
			return;
		}

		Vector2 center = Vector2(left + centerX, top + centerY); //(Vector2(left, top) + Vector2(right, bottom)) / 2;

		Vector2 rotatedTopLeft = math.rotatePoint(Vector2(left, top), center, rotation);
		Vector2 rotatedTopRight = math.rotatePoint(Vector2(left + width, top), center, rotation);
		Vector2 rotatedBottomLeft = math.rotatePoint(Vector2(left, top + height), center, rotation);
		Vector2 rotatedBottomRight = math.rotatePoint(Vector2(left + width, top + height), center, rotation);

		Vector2 rightValue, downValue;

		rightValue = ((rotatedTopRight - rotatedTopLeft).normal());
		downValue = ((rotatedBottomLeft - rotatedTopLeft).normal());

		int thicknessOnTwo = thickness / 2;
		thickness = (thicknessOnTwo + 0.5) * 2;

		drawLine(rotatedTopLeft.x + thicknessOnTwo * downValue.x, rotatedTopLeft.y + thicknessOnTwo * downValue.y, rotatedTopRight.x + thicknessOnTwo * downValue.x, rotatedTopRight.y + thicknessOnTwo * downValue.y, thickness, color, depth);
		drawLine(rotatedTopRight.x - thicknessOnTwo * rightValue.x, rotatedTopRight.y - thicknessOnTwo * rightValue.y, rotatedBottomRight.x - thicknessOnTwo * rightValue.x, rotatedBottomRight.y - thicknessOnTwo * rightValue.y, thickness, color, depth);
		drawLine(rotatedBottomLeft.x - thicknessOnTwo * downValue.x, rotatedBottomLeft.y - thicknessOnTwo * downValue.y, rotatedBottomRight.x - thicknessOnTwo * downValue.x, rotatedBottomRight.y - thicknessOnTwo * downValue.y, thickness, color, depth);
		drawLine(rotatedTopLeft.x + thicknessOnTwo * rightValue.x, rotatedTopLeft.y + thicknessOnTwo * rightValue.y, rotatedBottomLeft.x + thicknessOnTwo * rightValue.x, rotatedBottomLeft.y + thicknessOnTwo * rightValue.y, thickness, color, depth);
	}

	void GraphicsSystem::drawRotatedRectangle(const RotatedRectangle &rectangle, int thickness, ColorValue color, float depth)
	{
		drawRotatedRectangle(rectangle.left, rectangle.top, rectangle.width, rectangle.height, rectangle.centerX, rectangle.centerY, rectangle.rotation, thickness, color, depth);
	}

	void GraphicsSystem::drawRotatedRectangleTransformed(float left, float top, float width, float height, float centerX, float centerY, float rotation, int thickness, ColorValue color, float depth)
	{
		if(thickness == 0)
		{
			return;
		}

		Vector2 center = Vector2(left + centerX, top + centerY); //(Vector2(left, top) + Vector2(right, bottom)) / 2;

		Vector2 rotatedTopLeft = math.rotatePoint(Vector2(left, top), center, rotation);
		Vector2 rotatedTopRight = math.rotatePoint(Vector2(left + width, top), center, rotation);
		Vector2 rotatedBottomLeft = math.rotatePoint(Vector2(left, top + height), center, rotation);
		Vector2 rotatedBottomRight = math.rotatePoint(Vector2(left + width, top + height), center, rotation);

		Vector2 rightValue, downValue;

		rightValue = ((rotatedTopRight - rotatedTopLeft).normal());
		downValue = ((rotatedBottomLeft - rotatedTopLeft).normal());

		int thicknessOnTwo = thickness / 2;
		thickness = (thicknessOnTwo + 0.5) * 2;

		drawLineTransformed(rotatedTopLeft.x + thicknessOnTwo * downValue.x, rotatedTopLeft.y + thicknessOnTwo * downValue.y, rotatedTopRight.x + thicknessOnTwo * downValue.x, rotatedTopRight.y + thicknessOnTwo * downValue.y, thickness, color, depth);
		drawLineTransformed(rotatedTopRight.x - thicknessOnTwo * rightValue.x, rotatedTopRight.y - thicknessOnTwo * rightValue.y, rotatedBottomRight.x - thicknessOnTwo * rightValue.x, rotatedBottomRight.y - thicknessOnTwo * rightValue.y, thickness, color, depth);
		drawLineTransformed(rotatedBottomLeft.x - thicknessOnTwo * downValue.x, rotatedBottomLeft.y - thicknessOnTwo * downValue.y, rotatedBottomRight.x - thicknessOnTwo * downValue.x, rotatedBottomRight.y - thicknessOnTwo * downValue.y, thickness, color, depth);
		drawLineTransformed(rotatedTopLeft.x + thicknessOnTwo * rightValue.x, rotatedTopLeft.y + thicknessOnTwo * rightValue.y, rotatedBottomLeft.x + thicknessOnTwo * rightValue.x, rotatedBottomLeft.y + thicknessOnTwo * rightValue.y, thickness, color, depth);
	}

	void GraphicsSystem::drawRotatedRectangleTransformed(const RotatedRectangle &rectangle, int thickness, ColorValue color, float depth)
	{
		drawRotatedRectangleTransformed(rectangle.left, rectangle.top, rectangle.width, rectangle.height, rectangle.centerX, rectangle.centerY, rectangle.rotation, thickness, color, depth);
	}

	void GraphicsSystem::fillRectangle(float left, float top, float width, float height, ColorValue color, float depth)
	{
		fillQuadrilateral(left, top, left + width, top, left, top + height, left + width, top + height, color, color, color, color, depth);
	}

	void GraphicsSystem::fillRectangleTransformed(float left, float top, float width, float height, ColorValue color, float depth)
	{
		fillQuadrilateralTransformed(left, top, left + width, top, left, top + height, left + width, top + height, color, color, color, color, depth);
	}

	void GraphicsSystem::fillRotatedRectangle(float left, float top, float width, float height, float centerX, float centerY, float rotation, ColorValue color, float depth)
	{
		Vector2 center = Vector2(left + centerX, top + centerY); //(Vector2(left, top) + Vector2(right, bottom)) / 2;

		Vector2 rotatedTopLeft = math.rotatePoint(Vector2(left, top), center, rotation);
		Vector2 rotatedTopRight = math.rotatePoint(Vector2(left + width, top), center, rotation);
		Vector2 rotatedBottomLeft = math.rotatePoint(Vector2(left, top + height), center, rotation);
		Vector2 rotatedBottomRight = math.rotatePoint(Vector2(left + width, top + height), center, rotation);

		fillQuadrilateral(rotatedTopLeft, rotatedTopRight, rotatedBottomLeft, rotatedBottomRight, color, color, color, color, depth, 0);
	}

	void GraphicsSystem::fillRotatedRectangleTransformed(float left, float top, float width, float height, float centerX, float centerY, float rotation, ColorValue color, float depth)
	{
		Vector2 center = Vector2(left + centerX, top + centerY); //(Vector2(left, top) + Vector2(right, bottom)) / 2;

		Vector2 rotatedTopLeft = math.rotatePoint(Vector2(left, top), center, rotation);
		Vector2 rotatedTopRight = math.rotatePoint(Vector2(left + width, top), center, rotation);
		Vector2 rotatedBottomLeft = math.rotatePoint(Vector2(left, top + height), center, rotation);
		Vector2 rotatedBottomRight = math.rotatePoint(Vector2(left + width, top + height), center, rotation);

		fillQuadrilateralTransformed(rotatedTopLeft, rotatedTopRight, rotatedBottomLeft, rotatedBottomRight, color, color, color, color, depth, 0);
	}

	void GraphicsSystem::drawQuadrilateral(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int thickness, ColorValue color, float depth)
	{
		drawLine(x1, y1, x2, y2, thickness, color, depth);
		drawLine(x1, y1, x3, y3, thickness, color, depth);
		drawLine(x3, y3, x4, y4, thickness, color, depth);
		drawLine(x2, y2, x4, y4, thickness, color, depth);
	}

	void GraphicsSystem::drawQuadrilateralTransformed(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int thickness, ColorValue color, float depth)
	{
		drawLineTransformed(x1, y1, x2, y2, thickness, color, depth);
		drawLineTransformed(x1, y1, x3, y3, thickness, color, depth);
		drawLineTransformed(x4, y4, x2, y2, thickness, color, depth);
		drawLineTransformed(x4, y4, x3, y3, thickness, color, depth);
	}

	void GraphicsSystem::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int thickness, ColorValue color, float depth)
	{
		drawLine(x1, y1, x2, y2, thickness, color, depth);
		drawLineToLastPoint(x3, y3, thickness, color, depth);
		drawLineToLastPoint(x1, y1, thickness, color, depth);
	}

	void GraphicsSystem::drawTriangleTransformed(float x1, float y1, float x2, float y2, float x3, float y3, int thickness, ColorValue color, float depth)
	{
		drawLineTransformed(x1, y1, x2, y2, thickness, color, depth);
		drawLineToLastPointTransformed(x3, y3, thickness, color, depth);
		drawLineToLastPointTransformed(x1, y1, thickness, color, depth);
	}

	void GraphicsSystem::fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, ColorValue color1, ColorValue color2, ColorValue color3, float depth)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		Vertex v[3];
		v[0].x = x1;
		v[0].y = y1;
		v[0].z = depth;
		v[0].color = color1;
		//v[0].weight = 1;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x2;
		v[1].y = y2;
		v[1].z = depth;
		v[1].color = color2;
		//v[1].weight = 1;
		v[1].tu = tu1;
		v[1].tv = tv0;
		v[2].x = x3;
		v[2].y = y3;
		v[2].z = depth;
		v[2].color = color3;
		//v[2].weight = 1;
		v[2].tu = tu0;
		v[2].tv = tv1;

		//if(texture != 0) d3dDevice9->SetTexture(0, texture->getD3DTexture());
		//else //d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, (void*)v, sizeof(Vertex));
		drawPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::fillTriangleTransformed(float x1, float y1, float x2, float y2, float x3, float y3, ColorValue color1, ColorValue color2, ColorValue color3, float depth)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		Vertex v[3];
		v[0].x = x1;
		v[0].y = y1;
		v[0].z = depth;
		v[0].color = color1;
		//v[0].weight = 1;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x2;
		v[1].y = y2;
		v[1].z = depth;
		v[1].color = color2;
		//v[1].weight = 1;
		v[1].tu = tu1;
		v[1].tv = tv0;
		v[2].x = x3;
		v[2].y = y3;
		v[2].z = depth;
		v[2].color = color3;
		//v[2].weight = 1;
		v[2].tu = tu0;
		v[2].tv = tv1;

		//if(texture != 0) d3dDevice9->SetTexture(0, texture->getD3DTexture());
		//else //d3dDevice9->SetTexture(0, 0);
			//d3dDevice9->SetFVF(TRANSFORMED_VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, (void*)v, sizeof(Vertex));

		drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::fillQuadrilateral(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ColorValue color1, ColorValue color2, ColorValue color3, ColorValue color4, float depth)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		Vertex v[4];
		v[0].x = x1;
		v[0].y = y1;
		v[0].z = depth;
		v[0].color = color1;
		//v[0].weight = 1;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x2;
		v[1].y = y2;
		v[1].z = depth;
		v[1].color = color2;
		//v[1].weight = 1;
		v[1].tu = tu1;
		v[1].tv = tv0;
		v[2].x = x3;
		v[2].y = y3;
		v[2].z = depth;
		v[2].color = color3;
		//v[2].weight = 1;
		v[2].tu = tu0;
		v[2].tv = tv1;
		v[3].x = x4;
		v[3].y = y4;
		v[3].z = depth;
		v[3].color = color4;
		//v[3].weight = 1;
		v[3].tu = tu1;
		v[3].tv = tv1;

		//if(texture != 0) d3dDevice9->SetTexture(0, texture->getD3DTexture());
		//else //d3dDevice9->SetTexture(0, 0);
			//d3dDevice9->SetFVF(VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)v, sizeof(Vertex));
		drawPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::fillQuadrilateralTransformed(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ColorValue color1, ColorValue color2, ColorValue color3, ColorValue color4, float depth)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		Vertex v[4];
		v[0].x = x1;
		v[0].y = y1;
		v[0].z = depth;
		v[0].color = color1;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x2;
		v[1].y = y2;
		v[1].z = depth;
		v[1].color = color2;
		v[1].tu = tu1;
		v[1].tv = tv0;
		v[2].x = x3;
		v[2].y = y3;
		v[2].z = depth;
		v[2].color = color3;
		v[2].tu = tu0;
		v[2].tv = tv1;
		v[3].x = x4;
		v[3].y = y4;
		v[3].z = depth;
		v[3].color = color4;
		v[3].tu = tu1;
		v[3].tv = tv1;

		//if(texture != 0) d3dDevice9->SetTexture(0, texture->getD3DTexture());
		//else //d3dDevice9->SetTexture(0, 0);
			//d3dDevice9->SetFVF(TRANSFORMED_VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)v, sizeof(Vertex));

		drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}


	void GraphicsSystem::drawPolygon(float x, float y, int sides, int r, int thickness, ColorValue color, float depth)
	{
		if(sides < 3) sides = 3;
		else if(sides > 40) sides = 40;

		depth *= -1;

		Vertex v[82];
		float angleStep = pi * 2 / sides;
		float thicknessOnTwo = thickness / 2;

		for(int i = 0; i < sides; ++i)
		{
			float a = i * angleStep;
			float cosA = cos(a);
			float sinA = sin(a);

			v[i * 2].x = cosA * (r - thicknessOnTwo) + x;
			v[i * 2].y = sinA * (r - thicknessOnTwo) + y;
			v[i * 2].z = depth;
			v[i * 2].color = color;

			v[i * 2 + 1].x = cosA * (r + thicknessOnTwo) + x;
			v[i * 2 + 1].y = sinA * (r + thicknessOnTwo) + y;
			v[i * 2 + 1].z = depth;
			v[i * 2 + 1].color = color;
		}

		v[sides * 2] = v[0];
		v[sides * 2 + 1] = v[1];

		//d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, sides * 2, (void*)&v, sizeof(Vertex));
		drawPrimitive(v, sides * 2, PrimitiveType_TriangleStrip);

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::drawPolygonTransformed(float x, float y, int sides, int r, int thickness, ColorValue color, float depth)
	{
		if(sides < 3) sides = 3;
		else if(sides > 40) sides = 40;

		Vertex v[82];
		float angleStep = pi * 2 / sides;
		float thicknessOnTwo = thickness / 2;

		for(int i = 0; i < sides; ++i)
		{
			float a = i * angleStep;
			float cosA = cos(a);
			float sinA = sin(a);

			v[i * 2].x = cosA * (r - thicknessOnTwo) + x;
			v[i * 2].y = sinA * (r - thicknessOnTwo) + y;
			v[i * 2].z = depth;
			v[i * 2].color = color;

			v[i * 2 + 1].x = cosA * (r + thicknessOnTwo) + x;
			v[i * 2 + 1].y = sinA * (r + thicknessOnTwo) + y;
			v[i * 2 + 1].z = depth;
			v[i * 2 + 1].color = color;
		}

		v[sides * 2] = v[0];
		v[sides * 2 + 1] = v[1];

		//d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(TRANSFORMED_VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, sides * 2, (void*)&v, sizeof(Vertex));
		drawTransformedPrimitive(v, sides * 2, PrimitiveType_TriangleStrip);

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::fillPolygon(float x, float y, int sides, int r, ColorValue color, float depth)
	{
		Vertex *v = new Vertex[sides];

		float theta = pi * 2 / sides;

		for(int i = 0; i < sides; ++i)
		{
			float a = i * theta;
			v[i].x = cos(a) * r + x;
			v[i].y = sin(a) * r + y;
			v[i].z = depth;
			v[i].color = color;
			//v[i].weight = 1;
		}

		//d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 38, (void*)v, sizeof(Vertex));
		drawPrimitive(v, 38, PrimitiveType_TriangleStrip);

		delete &v;
	}

	void GraphicsSystem::fillPolygonTransformed(float x, float y, int sides, int r, ColorValue color, float depth)
	{
		Vertex *v = new Vertex[sides];

		float theta = pi * 2 / sides;

		for(int i = 0; i < sides; ++i)
		{
			float a = i * theta;
			v[i].x = cos(a) * r + x;
			v[i].y = sin(a) * r + y;
			v[i].z = depth;
			v[i].color = color;
		}

		//d3dDevice9->SetTexture(0, 0);
		//d3dDevice9->SetFVF(TRANSFORMED_VERTEX_FORMAT);
		//d3dDevice9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 38, (void*)v, sizeof(Vertex));
		drawTransformedPrimitive(v, 38, PrimitiveType_TriangleFan);

		delete &v;
	}

	void GraphicsSystem::drawCircle(float x, float y, int r, float startAngle, float endAngle, int thickness, ColorValue color, float depth)
	{
		//if(!dev) return;

		//color = lineColor;

		drawEllipse(x, y, r, r, startAngle, endAngle, thickness, color, depth);
	}

	void GraphicsSystem::drawCircleTransformed(float x, float y, int r, float startAngle, float endAngle, int thickness, ColorValue color, float depth)
	{
		//if(!dev) return;

		//color = lineColor;

		drawEllipseTransformed(x, y, r, r, startAngle, endAngle, thickness, color, depth);
	}

	void GraphicsSystem::fillCircle(float x, float y, int r, float startAngle, float endAngle, ColorValue color, float depth)
	{
		//if(!dev) return;

		//color = lineColor;

		fillEllipse(x, y, r, r, startAngle, endAngle, color, depth);
	}

	void GraphicsSystem::fillCircleTransformed(float x, float y, int r, float startAngle, float endAngle, ColorValue color, float depth)
	{
		//if(!dev) return;

		//color = lineColor;

		fillEllipseTransformed(x, y, r, r, startAngle, endAngle, color, depth);
	}

	void GraphicsSystem::drawEllipse(float x, float y, int rx, int ry, float startAngle, float endAngle, int thickness, ColorValue color, float depth)
	{
		drawPoint(x + rx, y, color, depth);

		float wrappedDifference = math.wrapAngleRadians(endAngle - startAngle);
		//endAngle = math.wrapAngleRadians(endAngle);

		if(abs(wrappedDifference) == 0.0f) wrappedDifference = 2 * pi;

		//if(endAngle == startAngle) endAngle -= 0.001;

		for(int i = 1; i < 40; ++i)
		{
			float a = startAngle + (i - 1) * ((wrappedDifference) / 38.0);
			drawLineToLastPoint(cos(a) * rx + x, sin(a) * ry + y, thickness, color, depth);
		}

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::drawEllipseTransformed(float x, float y, int rx, int ry, float startAngle, float endAngle, int thickness, ColorValue color, float depth)
	{
		//if(!d3dDevice9) return;

		//color = lineColor;

		drawPointTransformed(x + rx, y, color, depth);

		float wrappedDifference = math.wrapAngleRadians(endAngle - startAngle);
		//endAngle = math.wrapAngleRadians(endAngle);

		if(abs(wrappedDifference) == 0.0f) wrappedDifference = 2 * pi;

		for(int i = 1; i < 40; ++i)
		{
			float a = startAngle + (i - 1) * ((wrappedDifference) / 38.0);
			drawLineToLastPointTransformed(cos(a) * rx + x, sin(a) * ry + y, thickness, color, depth);
		}

		m_lastPoint.x = x;
		m_lastPoint.y = y;
	}

	void GraphicsSystem::fillEllipse(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue color, float depth)
	{
		const int stepCount = 42;

		float wrappedDifference = math.wrapAngleRadians(endAngle - startAngle);

		if(abs(wrappedDifference) == 0.0f) wrappedDifference = 2 * pi;

		Vertex v[3];

		for(int i = 0; i < stepCount; ++i)
		{
			float a0 = startAngle + (i - 1) * ((wrappedDifference) / stepCount);
			v[0].x = cos(a0) * rx + x;
			v[0].y = sin(a0) * ry + y;
			v[0].z = depth;
			v[0].color = color;

			float a1 = startAngle + (i) * ((wrappedDifference) / stepCount);
			v[1].x = cos(a1) * rx + x;
			v[1].y = sin(a1) * ry + y;
			v[1].z = depth;
			v[1].color = color;

			v[2].x = x;
			v[2].y = y;
			v[2].z = depth;
			v[2].color = color;

			drawPrimitive(v, 1, PrimitiveType_TriangleStrip);
		}
	}

	void GraphicsSystem::fillEllipseTransformed(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue color, float depth)
	{
		const int stepCount = 42;

		float wrappedDifference = math.wrapAngleRadians(endAngle - startAngle);

		if(abs(wrappedDifference) == 0.0f) wrappedDifference = 2 * pi;

		Vertex v[3];

		for(int i = 0; i < stepCount; ++i)
		{
			float a0 = startAngle + (i - 1) * ((wrappedDifference) / stepCount);
			v[0].x = cos(a0) * rx + x;
			v[0].y = sin(a0) * ry + y;
			v[0].z = depth;
			v[0].color = color;

			float a1 = startAngle + (i) * ((wrappedDifference) / stepCount);
			v[1].x = cos(a1) * rx + x;
			v[1].y = sin(a1) * ry + y;
			v[1].z = depth;
			v[1].color = color;

			v[2].x = x;
			v[2].y = y;
			v[2].z = depth;
			v[2].color = color;

			drawTransformedPrimitive(v, 1, PrimitiveType_TriangleStrip);
		}
	}

	void GraphicsSystem::fillGradientEllipse(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue innerColor, ColorValue outerColor, float depth)
	{
		const int stepCount = 42;

		float wrappedDifference = math.wrapAngleRadians(endAngle - startAngle);

		if(abs(wrappedDifference) == 0.0f) wrappedDifference = 2 * pi;

		Vertex v[3];

		for(int i = 0; i < stepCount; ++i)
		{
			float a0 = startAngle + (i - 1) * ((wrappedDifference) / stepCount);
			v[0].x = cos(a0) * rx + x;
			v[0].y = sin(a0) * ry + y;
			v[0].z = depth;
			v[0].color = outerColor;

			float a1 = startAngle + (i) * ((wrappedDifference) / stepCount);
			v[1].x = cos(a1) * rx + x;
			v[1].y = sin(a1) * ry + y;
			v[1].z = depth;
			v[1].color = outerColor;

			v[2].x = x;
			v[2].y = y;
			v[2].z = depth;
			v[2].color = innerColor;

			drawPrimitive(v, 1, PrimitiveType_TriangleStrip);
		}
	}

	void GraphicsSystem::fillGradientEllipseTransformed(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue innerColor, ColorValue outerColor, float depth)
	{
		const int stepCount = 42;

		float wrappedDifference = math.wrapAngleRadians(endAngle - startAngle);

		if(abs(wrappedDifference) == 0.0f) wrappedDifference = 2 * pi;

		Vertex v[3];

		for(int i = 0; i < stepCount; ++i)
		{
			float a0 = startAngle + (i - 1) * ((wrappedDifference) / stepCount);
			v[0].x = cos(a0) * rx + x;
			v[0].y = sin(a0) * ry + y;
			v[0].z = depth;
			v[0].color = outerColor;

			float a1 = startAngle + (i) * ((wrappedDifference) / stepCount);
			v[1].x = cos(a1) * rx + x;
			v[1].y = sin(a1) * ry + y;
			v[1].z = depth;
			v[1].color = outerColor;

			v[2].x = x;
			v[2].y = y;
			v[2].z = depth;
			v[2].color = innerColor;

			drawTransformedPrimitive(v, 1, PrimitiveType_TriangleStrip);
		}
	}

	void GraphicsSystem::fillPlane(float x1, float y1, float x2, float y2, int depth1, int depth2, ColorValue color)
	{
		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		Vertex v[4];
		v[0].x = x1;
		v[0].y = y1;
		v[0].z = depth1;
		v[0].color = color;
		v[0].tu = tu0;
		v[0].tv = tv0;
		v[1].x = x2;
		v[1].y = y2;
		v[1].z = depth1;
		v[1].color = color;
		v[1].tu = tu1;
		v[1].tv = tv0;
		v[2].x = x1;
		v[2].y = y1;
		v[2].z = depth2;
		v[2].color = color;
		v[2].tu = tu0;
		v[2].tv = tv1;
		v[3].x = x2;
		v[3].y = y2;
		v[3].z = depth2;
		v[3].color = color;
		v[3].tu = tu1;
		v[3].tv = tv1;

		drawPrimitive(v, 2, PrimitiveType_TriangleStrip);
	}

	void GraphicsSystem::renderCube(Vector3 topLeftBack, Vector3 bottomRightFront, ColorValue color)
	{
		int sideIndex = 0;

		float top = topLeftBack.y;
		float bottom = bottomRightFront.y;
		float left = topLeftBack.x;
		float right = bottomRightFront.x;
		float back = topLeftBack.z;
		float front = bottomRightFront.z;

		float tu0 = m_textureTopLeft.x;
		float tv0 = m_textureTopLeft.y;
		float tu1 = m_textureBottomRight.x;
		float tv1 = m_textureBottomRight.y;

		float x1 = left;
		float x2 = left;
		float y1 = top;
		float y2 = bottom;

		Vertex v[6 * 4];
		v[sideIndex * 4 + 0].x = x1;
		v[sideIndex * 4 + 0].y = y1;
		v[sideIndex * 4 + 0].z = back;
		v[sideIndex * 4 + 0].color = color;
		v[sideIndex * 4 + 0].tu = tu0;
		v[sideIndex * 4 + 0].tv = tv0;
		v[sideIndex * 4 + 1].x = x2;
		v[sideIndex * 4 + 1].y = y1;
		v[sideIndex * 4 + 1].z = front;
		v[sideIndex * 4 + 1].color = color;
		v[sideIndex * 4 + 1].tu = tu1;
		v[sideIndex * 4 + 1].tv = tv0;
		v[sideIndex * 4 + 2].x = x1;
		v[sideIndex * 4 + 2].y = y2;
		v[sideIndex * 4 + 2].z = back;
		v[sideIndex * 4 + 2].color = color;
		v[sideIndex * 4 + 2].tu = tu0;
		v[sideIndex * 4 + 2].tv = tv1;
		v[sideIndex * 4 + 3].x = x2;
		v[sideIndex * 4 + 3].y = y2;
		v[sideIndex * 4 + 3].z = front;
		v[sideIndex * 4 + 3].color = color;
		v[sideIndex * 4 + 3].tu = tu1;
		v[sideIndex * 4 + 3].tv = tv1;

		sideIndex++;

		x1 = right;
		x2 = right;
		y1 = top;
		y2 = bottom;

		v[sideIndex * 4 + 0].x = x1;
		v[sideIndex * 4 + 0].y = y1;
		v[sideIndex * 4 + 0].z = front;
		v[sideIndex * 4 + 0].color = color;
		v[sideIndex * 4 + 0].tu = tu0;
		v[sideIndex * 4 + 0].tv = tv0;
		v[sideIndex * 4 + 1].x = x2;
		v[sideIndex * 4 + 1].y = y1;
		v[sideIndex * 4 + 1].z = back;
		v[sideIndex * 4 + 1].color = color;
		v[sideIndex * 4 + 1].tu = tu1;
		v[sideIndex * 4 + 1].tv = tv0;
		v[sideIndex * 4 + 2].x = x1;
		v[sideIndex * 4 + 2].y = y2;
		v[sideIndex * 4 + 2].z = front;
		v[sideIndex * 4 + 2].color = color;
		v[sideIndex * 4 + 2].tu = tu0;
		v[sideIndex * 4 + 2].tv = tv1;
		v[sideIndex * 4 + 3].x = x2;
		v[sideIndex * 4 + 3].y = y2;
		v[sideIndex * 4 + 3].z = back;
		v[sideIndex * 4 + 3].color = color;
		v[sideIndex * 4 + 3].tu = tu1;
		v[sideIndex * 4 + 3].tv = tv1;

		sideIndex++;

		x1 = left;
		x2 = right;
		y1 = top;
		y2 = bottom;

		v[sideIndex * 4 + 0].x = x1;
		v[sideIndex * 4 + 0].y = y1;
		v[sideIndex * 4 + 0].z = front;
		v[sideIndex * 4 + 0].color = color;
		v[sideIndex * 4 + 0].tu = tu0;
		v[sideIndex * 4 + 0].tv = tv0;
		v[sideIndex * 4 + 1].x = x2;
		v[sideIndex * 4 + 1].y = y1;
		v[sideIndex * 4 + 1].z = front;
		v[sideIndex * 4 + 1].color = color;
		v[sideIndex * 4 + 1].tu = tu1;
		v[sideIndex * 4 + 1].tv = tv0;
		v[sideIndex * 4 + 2].x = x1;
		v[sideIndex * 4 + 2].y = y2;
		v[sideIndex * 4 + 2].z = front;
		v[sideIndex * 4 + 2].color = color;
		v[sideIndex * 4 + 2].tu = tu0;
		v[sideIndex * 4 + 2].tv = tv1;
		v[sideIndex * 4 + 3].x = x2;
		v[sideIndex * 4 + 3].y = y2;
		v[sideIndex * 4 + 3].z = front;
		v[sideIndex * 4 + 3].color = color;
		v[sideIndex * 4 + 3].tu = tu1;
		v[sideIndex * 4 + 3].tv = tv1;

		sideIndex++;

		x1 = left;
		x2 = right;
		y1 = top;
		y2 = bottom;

		v[sideIndex * 4 + 0].x = x1;
		v[sideIndex * 4 + 0].y = y1;
		v[sideIndex * 4 + 0].z = back;
		v[sideIndex * 4 + 0].color = color;
		v[sideIndex * 4 + 0].tu = tu0;
		v[sideIndex * 4 + 0].tv = tv0;
		v[sideIndex * 4 + 1].x = x2;
		v[sideIndex * 4 + 1].y = y1;
		v[sideIndex * 4 + 1].z = back;
		v[sideIndex * 4 + 1].color = color;
		v[sideIndex * 4 + 1].tu = tu1;
		v[sideIndex * 4 + 1].tv = tv0;
		v[sideIndex * 4 + 2].x = x1;
		v[sideIndex * 4 + 2].y = y2;
		v[sideIndex * 4 + 2].z = back;
		v[sideIndex * 4 + 2].color = color;
		v[sideIndex * 4 + 2].tu = tu0;
		v[sideIndex * 4 + 2].tv = tv1;
		v[sideIndex * 4 + 3].x = x2;
		v[sideIndex * 4 + 3].y = y2;
		v[sideIndex * 4 + 3].z = back;
		v[sideIndex * 4 + 3].color = color;
		v[sideIndex * 4 + 3].tu = tu1;
		v[sideIndex * 4 + 3].tv = tv1;

		sideIndex++;

		x1 = left;
		x2 = right;
		y1 = top;
		y2 = top;

		v[sideIndex * 4 + 0].x = x1;
		v[sideIndex * 4 + 0].y = y1;
		v[sideIndex * 4 + 0].z = back;
		v[sideIndex * 4 + 0].color = color;
		v[sideIndex * 4 + 0].tu = tu0;
		v[sideIndex * 4 + 0].tv = tv0;
		v[sideIndex * 4 + 1].x = x2;
		v[sideIndex * 4 + 1].y = y1;
		v[sideIndex * 4 + 1].z = back;
		v[sideIndex * 4 + 1].color = color;
		v[sideIndex * 4 + 1].tu = tu1;
		v[sideIndex * 4 + 1].tv = tv0;
		v[sideIndex * 4 + 2].x = x1;
		v[sideIndex * 4 + 2].y = y2;
		v[sideIndex * 4 + 2].z = front;
		v[sideIndex * 4 + 2].color = color;
		v[sideIndex * 4 + 2].tu = tu0;
		v[sideIndex * 4 + 2].tv = tv1;
		v[sideIndex * 4 + 3].x = x2;
		v[sideIndex * 4 + 3].y = y2;
		v[sideIndex * 4 + 3].z = front;
		v[sideIndex * 4 + 3].color = color;
		v[sideIndex * 4 + 3].tu = tu1;
		v[sideIndex * 4 + 3].tv = tv1;

		sideIndex++;

		x1 = left;
		x2 = right;
		y1 = bottom;
		y2 = bottom;

		v[sideIndex * 4 + 0].x = x1;
		v[sideIndex * 4 + 0].y = y1;
		v[sideIndex * 4 + 0].z = front;
		v[sideIndex * 4 + 0].color = color;
		v[sideIndex * 4 + 0].tu = tu0;
		v[sideIndex * 4 + 0].tv = tv0;
		v[sideIndex * 4 + 1].x = x2;
		v[sideIndex * 4 + 1].y = y1;
		v[sideIndex * 4 + 1].z = front;
		v[sideIndex * 4 + 1].color = color;
		v[sideIndex * 4 + 1].tu = tu1;
		v[sideIndex * 4 + 1].tv = tv0;
		v[sideIndex * 4 + 2].x = x1;
		v[sideIndex * 4 + 2].y = y2;
		v[sideIndex * 4 + 2].z = back;
		v[sideIndex * 4 + 2].color = color;
		v[sideIndex * 4 + 2].tu = tu0;
		v[sideIndex * 4 + 2].tv = tv1;
		v[sideIndex * 4 + 3].x = x2;
		v[sideIndex * 4 + 3].y = y2;
		v[sideIndex * 4 + 3].z = back;
		v[sideIndex * 4 + 3].color = color;
		v[sideIndex * 4 + 3].tu = tu1;
		v[sideIndex * 4 + 3].tv = tv1;

		drawPrimitiveList(v, 2, PrimitiveType_TriangleStrip, 6);
	}

#ifdef PLATFORM_WP8

	float GraphicsSystem::convertDipsToPixels(float dips)
	{
		static const float dipsPerInch = 96.0f;
		return floor(dips * Windows::Graphics::Display::DisplayProperties::LogicalDpi / dipsPerInch + 0.5f);
	}

	ColorValue GraphicsSystem::getAccentColor()
	{
		ColorValue output;
		output.r = (float)m_accentColor.R / 255.0;
		output.g = (float)m_accentColor.G / 255.0;
		output.b = (float)m_accentColor.B / 255.0;
		output.a = (float)m_accentColor.A / 255.0;

		return output;
	}

#endif
};
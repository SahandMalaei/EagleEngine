#ifndef _GRAPHICS_H
#define _GRAPHICS_H 1

#include "Eagle.h"

#define COLOR_RGBA(r, g, b, a) ColorValue(r, g, b, a)
#define COLOR_RGB(r, g, b) ColorValue(r, g, b)

#define COLOR_GET_B(color) (int)(((DWORD)color) & 0x0000ff)
#define COLOR_GET_G(color) (int)((((DWORD)color) & 0x00ff00) / 0x000100)
#define COLOR_GET_R(color) (int)((((DWORD)color) & 0xff0000) / 0x010000)
#define COLOR_GET_A(color) (int)((((DWORD)color) & 0xff000000) / 0x01000000)

namespace ProjectEagle
{
	enum GraphicsAPIType
	{
		//GraphicsAPI_Direct3D9 = 0,
		GraphicsAPI_Direct3D11 = 1
	};

	enum PrimitiveType
	{
		PrimitiveType_PointList = 0,
		PrimitiveType_TriangleStrip = 1,
		PrimitiveType_TriangleFan = 2
	};

	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct DrawCallData;

	enum BlendMode
	{
		BlendMode_Normal = 0,
		BlendMode_Additive = 1
	};

	class GraphicsSystem
	{
#ifdef PLATFORM_WP8
		friend ref class ApplicationWP8;
#endif
		friend class EagleEngine;
		friend class ResourceManagerClass;

	private:
		GraphicsAPIType m_graphicsAPIType;

#ifndef PLATFORM_WP8
		IDXGIAdapter *m_adapter; 
		IDXGIFactory1 *m_dxgiFactory; 
		ID3D11Device *m_d3dDevice11;
		ID3D11DeviceContext *m_d3dDevice11Context;
		IDXGISwapChain *m_swapChain;
		ID3D11DepthStencilView *m_depthStencilView;
		ID3D11RenderTargetView *m_renderTargetView;

		ID3D11Texture2D *m_backBufferTexture;
		ID3D11Texture2D *m_depthStencilTexture;
#else
		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice11;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dDevice11Context;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		ComPtr<ID3D11Texture2D> m_backBufferTexture;
		ComPtr<ID3D11Texture2D> m_depthStencilTexture;
#endif

		ID3D11Buffer *m_vertexBuffer;

		D3D_FEATURE_LEVEL m_d3d11FeatureLevel;
		D3D_DRIVER_TYPE m_d3d11DriverType;

		DXGI_FORMAT m_displayFormat;

		ID3D11RasterizerState *m_rasterizerState2D;

#ifdef PLATFORM_WP8
		Windows::Foundation::Size m_renderTargetSize;
		Windows::Foundation::Rect m_windowBounds;

		Platform::Agile<Windows::UI::Core::CoreWindow> m_windowHandle;

		void createWindowSizeDependentResources();
		void releaseResourcesForSuspending();
#else
		HWND m_windowHandle;

		bool m_windowResizable;
#endif

		bool m_fullscreen;
		bool m_swapChainFullscreen;

		int m_screenWidth, m_screenHeight;
		int m_clientScreenWidth, m_clientScreenHeight;
		int m_swapChainWidth, m_swapChainHeight;

		bool m_cursorVisibility;

		bool m_multiSamplingSupport, m_multiSamplingEnabled;
		int m_multiSamplingQualityLevel;
		int m_multiSamplingSampleCount;

		DWORD m_ambientColor;

		char m_adapterName[512];

		Vector2 m_lastPoint;

		void preinitialize();
		void initialize();

		bool m_isDeviceLost, m_isLostDeviceFixed;
		int handleDeviceStatus();

		bool createWindowSizeDependentObjects();
		bool handleFullscreenStateChange();

		bool renderStart();
		bool renderStop();

		Camera m_camera;

		Shader m_simpleVertexShader;
		Shader m_simplePixelShader;
		Shader m_texturedVertexShader;
		Shader m_texturedPixelShader;

		bool m_textureEnabled;

		DirectX::XMMATRIX m_worldMatrix;
		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projectionMatrix;

#ifdef PLATFORM_WP8
		float GraphicsSystem::convertDipsToPixels(float dips);

		Windows::UI::Color m_accentColor;
#endif

		int m_drawCallBufferSize;
		DrawCallData *m_drawCallBuffer;
		int m_drawCallCount;
		int m_frameVertexBufferSize;
		Vertex *m_frameVertexBuffer;
		int m_frameVertexBuffer_CurrentVertexIndex;

		void renderBufferedDrawCalls();
		bool m_renderingBufferedDrawCalls;

		Texture *m_currentTexture;
		Vector2 m_textureTopLeft, m_textureBottomRight;

		BlendMode m_blendMode;
		ID3D11BlendState *m_blendState[2];

		void resetWindow();

	public:
		GraphicsSystem();
		~GraphicsSystem();

		void setGraphicsAPI(GraphicsAPIType type);
		GraphicsAPIType getGraphicsAPIType();

#ifndef PLATFORM_WP8
		HWND getWidnowHandle();
#else
		Windows::UI::Core::CoreWindow ^getWindowHandle();
#endif

#ifndef PLATFORM_WP8
		ID3D11Device *getD3DDevice11();
#else
		ID3D11Device1 *getD3DDevice11();
#endif
		ID3D11DeviceContext *getD3DDevice11Context();

		int getScreenWidth();
		void setScreenWidth(int value);
		int getScreenHeight();
		void setScreenHeight(int value);

		int getClientScreenWidth();
		int getClientScreenHeight();

		Vector2 getScreenDimensions();
		void setScreenDimensions(int width, int height);
		void setScreenDimensions(Vector2 value);

		bool getFullscreen();
		void setFullscreen(bool value);

		Vector2 getWindowPosition();
		void setWindowPosition(Vector2 value);

		void setWindowAlpha(float value);

		void handleWindowResize();

		bool getCursorVisibility();
		void setCursorVisibility(bool value);

		Vector2 getCursorPosition();
		void setCursorPosition(Vector2 value);

		void enableAntiAliasing();
		void disableAntiAliasing();
		void toggleAntiAliasing();
		void setAntiAliasingState(bool state);
		bool isAntiAliasingEnabled();

		int getMultiSamplingSampleCount();
		int getMultiSamplingQualityLevel();

		bool isWindowResizable();
		void setWindowResizable(bool value);

		void setIdentity();
		void setWorldIdentity();
		void setWorldTransform(DirectX::XMMATRIX *matrix);
		void setViewTransform(DirectX::XMMATRIX *matrix);
		void setProjectionTransform(DirectX::XMMATRIX *matrix);

		void takeScreenshot();

		Camera *getCamera();

		std::string getDisplayAdapterName();

		void drawPrimitive(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int startVertexIndex = 0);
		void drawTransformedPrimitive(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int startVertexIndex = 0);

		void drawPrimitiveList(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int objectCount, int startVertexIndex = 0);
		void drawTransformedPrimitiveList(Vertex *vertexList, int primitiveCount, PrimitiveType primitiveType, int objectCount, int startVertexIndex = 0);

		void setTexture(Texture *texture);

		void setTextureSourceRectangle(Vector2 topLeft, Vector2 bottomRight);
		Vector2 getTextureTopLeft();
		Vector2 getTextureBottomRight();

		BlendMode getBlendMode();
		void setBlendMode(BlendMode mode);

		void clearScene(ColorValue color);

		void drawPoint(float x, float y, ColorValue color, float depth = .1);
		void drawPoint(Vector2 pos, ColorValue color, float depth = .1){drawPoint(pos.x, pos.y, color, depth);}
		void drawPointTransformed(float x, float y, ColorValue color, float depth = .1);
		void drawPointTransformed(Vector2 pos, ColorValue color, float depth = .1){drawPointTransformed(pos.x, pos.y, color, depth);}

		void drawLine(float x1, float y1, float x2, float y2, int thickness, ColorValue color, float depth = .1);
		void drawLine(Vector2 pos0, Vector2 pos1, int thickness, ColorValue color, float depth = .1){drawLine(pos0.x, pos0.y, pos1.x, pos1.y, thickness, color, depth);}
		void drawLineTransformed(float x1, float y1, float x2, float y2, int thickness, ColorValue color, float depth = .1);
		void drawLineTransformed(Vector2 pos0, Vector2 pos1, int thickness, ColorValue color, float depth = .1){drawLineTransformed(pos0.x, pos0.y, pos1.x, pos1.y, thickness, color, depth);}

		void drawLineGradient(float x1, float y1, float x2, float y2, int thickness0, int thickness1, ColorValue color0, ColorValue color1, float depth = .1);
		void drawLineGradient(Vector2 pos0, Vector2 pos1, int thickness0, int thickness1, ColorValue color0, ColorValue color1, float depth = .1){drawLineGradient(pos0.x, pos0.y, pos1.x, pos1.y, thickness0, thickness1, color0, color1, depth);}
		void drawLineGradientTransformed(float x1, float y1, float x2, float y2, int thickness0, int thickness1, ColorValue color0, ColorValue color1, float depth = .1);
		void drawLineGradientTransformed(Vector2 pos0, Vector2 pos1, int thickness0, int thickness1, ColorValue color0, ColorValue color1, float depth = .1){drawLineGradientTransformed(pos0.x, pos0.y, pos1.x, pos1.y, thickness0, thickness1, color0, color1, depth);}

		void drawLineToLastPoint(float x, float y, int thickness, ColorValue color, float depth = .1);
		void drawLineToLastPoint(Vector2 pos, int thickness, ColorValue color, float depth = .1){drawLineToLastPoint(pos.x, pos.y, thickness, color, depth);}
		void drawLineToLastPointTransformed(float x, float y, int thickness, ColorValue color, float depth = .1);
		void drawLineToLastPointTransformed(Vector2 pos, int thickness, ColorValue color, float depth = .1){drawLineToLastPointTransformed(pos.x, pos.y, thickness, color, depth);}

		void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int thickness, ColorValue color, float depth = .1);
		void drawTriangle(Vector2 pos0, Vector2 pos1, Vector2 pos2, int thickness, ColorValue color, float depth = .1){drawTriangle(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, thickness, color, depth);}
		void drawTriangleTransformed(float x1, float y1, float x2, float y2, float x3, float y3, int thickness, ColorValue color, float depth = .1);
		void drawTriangleTransformed(Vector2 pos0, Vector2 pos1, Vector2 pos2, int thickness, ColorValue color, float depth = .1){drawTriangleTransformed(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, thickness, color, depth);}

		void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, ColorValue color0, ColorValue color1, ColorValue color2, float depth = .1);
		void fillTriangle(Vector2 pos0, Vector2 pos1, Vector2 pos2, ColorValue color0, ColorValue color1, ColorValue color2, float depth = .1){fillTriangle(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, color0, color1, color2, depth);}
		void fillTriangleTransformed(float x1, float y1, float x2, float y2, float x3, float y3, ColorValue color0, ColorValue color1, ColorValue color2, float depth = .1);
		void fillTriangleTransformed(Vector2 pos0, Vector2 pos1, Vector2 pos2, ColorValue color0, ColorValue color1, ColorValue color2, float depth = .1){fillTriangleTransformed(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, color0, color1, color2, depth);}

		void drawRectangle(float left, float top, float width, float height, int thickness, ColorValue color, float depth = .1);
		void drawRectangle(Vector2 topLeft, Vector2 dimensions, int thickness, ColorValue color, float depth = .1){drawRectangle(topLeft.x, topLeft.y, dimensions.x, dimensions.y, thickness, color, depth);}
		void drawRectangle(Rectangle rectangle, int thickness, ColorValue color, float depth = .1){drawRectangle(rectangle.left, rectangle.top, rectangle.width, rectangle.height, thickness, color, depth);}
		void drawRectangleTransformed(float left, float top, float width, float height, int thickness, ColorValue color, float depth = .1);
		void drawRectangleTransformed(Vector2 topLeft, Vector2 dimensions, int thickness, ColorValue color, float depth = .1){drawRectangleTransformed(topLeft.x, topLeft.y, dimensions.x, dimensions.y, thickness, color, depth);}
		void drawRectangleTransformed(Rectangle rectangle, int thickness, ColorValue color, float depth = .1){drawRectangleTransformed(rectangle.left, rectangle.top, rectangle.width, rectangle.height, thickness, color, depth);}

		void drawRotatedRectangle(float left, float top, float right, float bottom, float centerX, float centerY, float rotation, int thickness, ColorValue color, float depth = .1);
		void drawRotatedRectangle(Vector2 topLeft, Vector2 bottomRight, Vector2 center, float rotation, int thickness, ColorValue color, float depth = .1){drawRotatedRectangle(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, center.x, center.y, rotation, thickness, color, depth);}
		void drawRotatedRectangle(const RotatedRectangle &rectangle, int thickness, ColorValue color, float depth = .1);
		void drawRotatedRectangleTransformed(float left, float top, float right, float bottom, float centerX, float centerY, float rotation, int thickness, ColorValue color, float depth = .1);
		void drawRotatedRectangleTransformed(Vector2 topLeft, Vector2 bottomRight, Vector2 center, float rotation, int thickness, ColorValue color, float depth = .1){drawRotatedRectangleTransformed(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, center.x, center.y, rotation, thickness, color, depth);}
		void drawRotatedRectangleTransformed(const RotatedRectangle &rectangle, int thickness, ColorValue color, float depth = .1);

		void fillRectangle(float left, float top, float width, float height, ColorValue color, float depth = .1);
		void fillRectangle(Vector2 topLeft, Vector2 dimensions, ColorValue color, float depth = .1){fillRectangle(topLeft.x, topLeft.y, dimensions.x, dimensions.y, color, depth);}
		void fillRectangle(Rectangle rectangle, ColorValue color, float depth = .1){fillRectangle(rectangle.left, rectangle.top, rectangle.width, rectangle.height, color, depth);}
		void fillRectangleTransformed(float left, float top, float right, float bottom, ColorValue color, float depth = .1);
		void fillRectangleTransformed(Vector2 topLeft, Vector2 dimensions, ColorValue color, float depth = .1){fillRectangleTransformed(topLeft.x, topLeft.y, dimensions.x, dimensions.y, color, depth);}
		void fillRectangleTransformed(Rectangle rectangle, ColorValue color, float depth = .1){fillRectangleTransformed(rectangle.left, rectangle.top, rectangle.width, rectangle.height, color, depth);}

		void fillRotatedRectangle(float left, float top, float width, float height, float centerX, float centerY, float rotation, ColorValue color, float depth = .1);
		void fillRotatedRectangle(Vector2 topLeft, Vector2 dimensions, Vector2 center, float rotation, ColorValue color, float depth = .1){fillRotatedRectangle(topLeft.x, topLeft.y, dimensions.x, dimensions.y, center.x, center.y, rotation, color, depth);}
		void fillRotatedRectangle(const RotatedRectangle &rectangle, ColorValue color, float depth = .1){fillRotatedRectangle(rectangle.left, rectangle.top, rectangle.width, rectangle.height, rectangle.centerX, rectangle.centerY, rectangle.rotation, color, depth);}
		void fillRotatedRectangleTransformed(float left, float top, float right, float bottom, float centerX, float centerY, float rotation, ColorValue color, float depth = .1);
		void fillRotatedRectangleTransformed(Vector2 topLeft, Vector2 dimensions, Vector2 center, float rotation, ColorValue color, float depth = .1){fillRotatedRectangleTransformed(topLeft.x, topLeft.y, dimensions.x, dimensions.y, center.x, center.y, rotation, color, depth);}
		void fillRotatedRectangleTransformed(const RotatedRectangle &rectangle, ColorValue color, float depth = .1){fillRotatedRectangleTransformed(rectangle.left, rectangle.top, rectangle.width, rectangle.height, rectangle.centerX, rectangle.centerY, rectangle.rotation, color, depth);}

		void drawQuadrilateral(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int thickness, ColorValue color, float depth = .1);
		void drawQuadrilateral(Vector2 pos0, Vector2 pos1, Vector2 pos2, Vector2 pos3, int thickness, ColorValue color, float depth = .1){drawQuadrilateral(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, thickness, color, depth);}
		void drawQuadrilateralTransformed(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int thickness, ColorValue color, float depth = .1);
		void drawQuadrilateralTransformed(Vector2 pos0, Vector2 pos1, Vector2 pos2, Vector2 pos3, int thickness, ColorValue color, float depth = .1){drawQuadrilateralTransformed(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, thickness, color, depth);}

		void fillQuadrilateral(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ColorValue color0, ColorValue color1, ColorValue color2, ColorValue color3, float depth = .1);
		void fillQuadrilateral(Vector2 pos0, Vector2 pos1, Vector2 pos2, Vector2 pos3, ColorValue color0, ColorValue color1, ColorValue color2, ColorValue color3, float depth = .1, Texture *texture = 0){fillQuadrilateral(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,color0, color1, color2, color3, depth);}
		void fillQuadrilateralTransformed(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ColorValue color1, ColorValue color2, ColorValue color3, ColorValue color4, float depth = .1);
		void fillQuadrilateralTransformed(Vector2 pos0, Vector2 pos1, Vector2 pos2, Vector2 pos3, ColorValue color0, ColorValue color1, ColorValue color2, ColorValue color3, float depth = .1, Texture *texture = 0){fillQuadrilateralTransformed(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,color0, color1, color2, color3, depth);}

		void drawPolygon(float x, float y, int sides, int r, int thickness, ColorValue color, float depth = .1);
		void drawPolygon(Vector2 pos, int sides, int r, int thickness, ColorValue color, float depth = .1){drawPolygon(pos.x, pos.y, sides, r, thickness, color,depth);}
		void drawPolygonTransformed(float x, float y, int sides, int r, int thickness, ColorValue color, float depth = .1);
		void drawPolygonTransformed(Vector2 pos, int sides, int r, int thickness, ColorValue color, float depth = .1){drawPolygonTransformed(pos.x, pos.y, sides, r, thickness, color, depth);}

		void fillPolygon(float x, float y, int sides, int r, ColorValue color, float depth = .1);
		void fillPolygon(Vector2 pos, int sides, int r, ColorValue color, float depth = .1){fillPolygon(pos.x, pos.y, sides, r, color, depth);}
		void fillPolygonTransformed(float x, float y, int sides, int r, ColorValue color, float depth = .1);
		void fillPolygonTransformed(Vector2 pos, int sides, int r, ColorValue color, float depth = .1){fillPolygonTransformed(pos.x, pos.y, sides, r, color, depth);}

		void drawCircle(float x, float y, int r, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1);
		void drawCircle(Vector2 pos, int r, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1){drawCircle(pos.x, pos.y, r, startAngle, endAngle, thickness, color, depth);}
		void drawCircleTransformed(float x, float y, int r, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1);
		void drawCircleTransformed(Vector2 pos, int r, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1){drawCircleTransformed(pos.x, pos.y, r, startAngle, endAngle, thickness, color, depth);}

		void fillCircle(float x, float y, int r, float startAngle, float endAngle, ColorValue color, float depth = .1);
		void fillCircle(Vector2 pos, int r, float startAngle, float endAngle, ColorValue color, float depth = .1){fillCircle(pos.x, pos.y, r, startAngle, endAngle, color, depth);}
		void fillCircleTransformed(float x, float y, int r, float startAngle, float endAngle, ColorValue color, float depth = .1);
		void fillCircleTransformed(Vector2 pos, int r, float startAngle, float endAngle, ColorValue color, float depth = .1){fillCircleTransformed(pos.x, pos.y, r, startAngle, endAngle, color, depth);}

		void drawEllipse(float x, float y, int rx, int ry, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1);
		void drawEllipse(Vector2 pos, int rx, int ry, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1){drawEllipse(pos.x, pos.y, rx, ry, startAngle, endAngle, thickness, color, depth);}
		void drawEllipseTransformed(float x, float y, int rx, int ry, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1);
		void drawEllipseTransformed(Vector2 pos, int rx, int ry, float startAngle, float endAngle, int thickness, ColorValue color, float depth = .1){drawEllipseTransformed(pos.x, pos.y, rx, ry, startAngle, endAngle, thickness, color, depth);}

		void fillEllipse(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue color, float depth = .1);
		void fillEllipse(Vector2 pos, int rx, int ry, float startAngle, float endAngle, ColorValue color, float depth = .1){fillEllipse(pos.x, pos.y, rx, ry, startAngle, endAngle, color, depth);}
		void fillEllipseTransformed(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue color, float depth = .1);
		void fillEllipseTransformed(Vector2 pos, int rx, int ry, float startAngle, float endAngle, ColorValue color, float depth = .1){fillEllipseTransformed(pos.x, pos.y, rx, ry, startAngle, endAngle, color, depth);}

		void fillGradientEllipse(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue innerColor, ColorValue outerColor, float depth = .1);
		void fillGradientEllipse(Vector2 pos, int rx, int ry, float startAngle, float endAngle, ColorValue innerColor, ColorValue outerColor, float depth = .1){fillGradientEllipse(pos.x, pos.y, rx, ry, startAngle, endAngle, innerColor, outerColor, depth);}
		void fillGradientEllipseTransformed(float x, float y, int rx, int ry, float startAngle, float endAngle, ColorValue innerColor, ColorValue outerColor, float depth = .1);
		void fillGradientEllipseTransformed(Vector2 pos, int rx, int ry, float startAngle, float endAngle, ColorValue innerColor, ColorValue outerColor, float depth = .1){fillGradientEllipseTransformed(pos.x, pos.y, rx, ry, startAngle, endAngle, innerColor, outerColor, depth);}

		void fillPlane(float x1, float y1, float x2, float y2, int depth0, int depth1, ColorValue color);
		void fillPlane(Vector2 pos0, Vector2 pos1, int depth0, int depth1, ColorValue color){fillPlane(pos0.x, pos0.y, pos1.x, pos1.y, depth0, depth1, color);}

		void renderCube(Vector3 topLeftBack, Vector3 bottomRightFront, ColorValue color);

#ifdef PLATFORM_WP8
		ColorValue getAccentColor();
#endif
	};
};

#endif
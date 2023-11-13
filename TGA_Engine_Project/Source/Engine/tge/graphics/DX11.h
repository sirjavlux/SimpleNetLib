#pragma once
#include <array>
#include <tge/Graphics/DepthBuffer.h>
#include <tge/Graphics/RenderTarget.h>
#include <tge/Graphics/RenderStateManager.h>
#include <tge/Math/Color.h>
#include <tge/Math/Vector.h>
#include <thread>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Debug;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;

namespace Tga
{
class WindowsWindow;

// DirectX 11 Framework. Shorthand to make it easier to deal with.
class DX11
{
	static WindowsWindow* myWindowHandler;
	
public:
	DX11();
	~DX11();

	bool Init(WindowsWindow* aWindowHandler);
	bool Resize(Vector2ui resolution);
	void BeginFrame(Color aClearColor);
	void EndFrame(bool aEnableVsync = true);

	static bool IsOnSameThreadAsEngine();

	static Vector2ui GetResolution();

	static ID3D11Device* Device;
	static ID3D11DeviceContext* Context;
	static IDXGISwapChain* SwapChain;
	static RenderTarget* BackBuffer;
	static DepthBuffer* DepthBuffer;

	static RenderStateManager* RenderStateManager;

	static bool LoadPixelShader(const char* aPath, ID3D11PixelShader** aPixelShader);
	static bool LoadVertexShader(const char* aPath, ID3D11VertexShader** aVertexShader, std::string& data);
	static bool LoadGeometryShader(const char* aPath, ID3D11GeometryShader** aGeometryShader);

	static void ResetDrawCallCounter() { PreviousDrawCallCount = DrawCallCount; DrawCallCount = 0; };
	static void LogDrawCall() { DrawCallCount++; }
	static int GetPreviousDrawCallCount() { return PreviousDrawCallCount; }

private:
	static int DrawCallCount;
	static int PreviousDrawCallCount;
	static bool IsCreated;
	static std::thread::id RenderThreadId;

	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	RenderTarget myBackBuffer;
	Tga::DepthBuffer myDepthBuffer;

	ComPtr<ID3D11Debug> myD3dDebug;

	Tga::RenderStateManager myRenderStateManager;
};

} // namespace Tga
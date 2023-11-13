#include "stdafx.h"
#include <tge/graphics/DX11.h>

#include <fstream>
#include <tge/windows/WindowsWindow.h>

using namespace Tga;

#define REPORT_DX_WARNINGS

ID3D11Device* DX11::Device;
ID3D11DeviceContext* DX11::Context;
IDXGISwapChain* DX11::SwapChain;
RenderTarget* DX11::BackBuffer;
DepthBuffer* DX11::DepthBuffer;

WindowsWindow* DX11::myWindowHandler;

RenderStateManager* DX11::RenderStateManager;

bool DX11::IsCreated;
std::thread::id DX11::RenderThreadId;
int DX11::DrawCallCount = 0;
int DX11::PreviousDrawCallCount = 0;

DX11::DX11()
{
	assert(IsCreated == false);
	IsCreated = true;

	Device = nullptr;
	Context = nullptr;
	SwapChain = nullptr;
	BackBuffer = nullptr;
	DepthBuffer = nullptr;

	RenderStateManager = nullptr;
}

DX11::~DX11()
{
	assert(IsCreated == true);
	IsCreated = false;

#ifdef REPORT_DX_WARNINGS
	if (myD3dDebug)
	{
		myD3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
	}
#endif

	Device = nullptr;
	Context = nullptr;
	SwapChain = nullptr;
	BackBuffer = nullptr;
	DepthBuffer = nullptr;

	RenderStateManager = nullptr;
}

IDXGIAdapter* FindBestAdapter()
{
	HRESULT result = S_OK;
	IDXGIFactory* factory;

	DXGI_MODE_DESC* displayModeList = nullptr;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}
	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter* usingAdapter = nullptr;
	int adapterIndex = 0;
	std::vector<DXGI_ADAPTER_DESC> myAdapterDescs;
	std::vector<IDXGIAdapter*> myAdapters;
	while (factory->EnumAdapters(adapterIndex, &usingAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		usingAdapter->GetDesc(&adapterDesc);
		myAdapterDescs.push_back(adapterDesc);
		myAdapters.push_back(usingAdapter);
		++adapterIndex;
	}

	if (adapterIndex == 0)
	{
		return nullptr;
	}

	INFO_PRINT("%s", "Video card(s) detected: ");
	for (DXGI_ADAPTER_DESC desc : myAdapterDescs)
	{
		int memory = (int)(desc.DedicatedVideoMemory / 1024 / 1024);
		INFO_PRINT("	%ls%s%i%s", desc.Description, " Mem: ", memory, "Mb");
		memory;
	}

	DXGI_ADAPTER_DESC usingAdapterDesc = myAdapterDescs[0];
	usingAdapter = myAdapters[0];

	INFO_PRINT("%s", "Detecting best card...");


	const std::wstring nvidia = L"NVIDIA";
	const std::wstring ati = L"ATI";

	int memory = (int)(usingAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
	int mostMem = 0;

	for (unsigned int i = 0; i < myAdapterDescs.size(); i++)
	{
		DXGI_ADAPTER_DESC desc = myAdapterDescs[i];
		memory = (int)(desc.DedicatedVideoMemory / 1024 / 1024);
		std::wstring name = desc.Description;
		if (name.find(nvidia) != std::wstring::npos || name.find(ati) != std::wstring::npos)
		{
			if (memory > mostMem)
			{
				mostMem = memory;
				usingAdapterDesc = desc;
				usingAdapter = myAdapters[i];
			}
		}
	}

	INFO_PRINT("%s%ls%s%i", "Using graphic card: ", usingAdapterDesc.Description, " Dedicated Mem: ", mostMem);

	// Get the adapter (video card) description.
	result = usingAdapter->GetDesc(&usingAdapterDesc);
	if (FAILED(result))
	{
		return nullptr;
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	return usingAdapter;
}


bool DX11::Init(WindowsWindow* aWindowHandler)
{
	myWindowHandler = aWindowHandler;
	
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = myWindowHandler->GetWindowHandle();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;
#if defined(_DEBUG)
#if defined(REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

	IDXGIAdapter* adapter = FindBestAdapter();
	if (!adapter)
	{
		return false;
	}

	result = D3D11CreateDeviceAndSwapChain(
		adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext
	);

#if defined(_DEBUG)
#if defined(REPORT_DX_WARNINGS)
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Device could not create itself in debug mode, trying without debug layer... If you have Win10, try this:  Settings panel -> System -> Apps & features -> Manage optional Features -> Add a feature -> Select ""Graphics Tools""");
		creationFlags = 0;

		INFO_PRINT("%s", "Creating device without debug layer");
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&mySwapChain,
			&myDevice,
			nullptr,
			&myContext
		);
	}
#endif
#endif

	if (FAILED(result))
	{
		return false;
	}

	myD3dDebug = nullptr;
#ifdef _DEBUG
	if (SUCCEEDED(myDevice.As(&myD3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue = nullptr;
		if (SUCCEEDED(myD3dDebug.As(&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, false);

			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// Add more message IDs here as needed
			};

			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	Device = myDevice.Get();
	Context = myContext.Get();
	SwapChain = mySwapChain.Get();

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	myBackBuffer = RenderTarget::Create(backBufferTexture);
	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();
	myDepthBuffer = DepthBuffer::Create({ static_cast<unsigned int>(textureDesc.Width), static_cast<unsigned int>(textureDesc.Height) });

	myBackBuffer.SetAsActiveTarget(&myDepthBuffer);

	BackBuffer = &myBackBuffer;
	DepthBuffer = &myDepthBuffer;

	if (!myRenderStateManager.Init())
		return false;

	myRenderStateManager.ResetStates();

	RenderStateManager = &myRenderStateManager;
	RenderThreadId = std::this_thread::get_id();



	return true;
}

bool DX11::Resize(Vector2ui)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	myContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	myContext->OMSetDepthStencilState(0, 0);
	myContext->ClearState();
	
	if (!mySwapChain)
	{
		return false;
	}

	myBackBuffer = RenderTarget();
	myDepthBuffer = Tga::DepthBuffer();

	myContext->Flush();
	if (mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0) != S_OK)
	{
		ERROR_PRINT("%s", "Could not resize buffers!");
		return false;
	}

	BackBuffer = nullptr;
	DepthBuffer = nullptr;

	ID3D11Texture2D* backBufferTexture = nullptr;
	if (mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture) != S_OK)
	{
		ERROR_PRINT("%s", "Could not resize buffers!");
		return false;
	}

	if (!backBufferTexture)
	{
		return false;
	}

	myBackBuffer = RenderTarget::Create(backBufferTexture);
	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();
	myDepthBuffer = DepthBuffer::Create({ static_cast<unsigned int>(textureDesc.Width), static_cast<unsigned int>(textureDesc.Height) });
	myBackBuffer.SetAsActiveTarget(&myDepthBuffer);

	BackBuffer = &myBackBuffer;
	DepthBuffer = &myDepthBuffer;

	return true;
}

void DX11::BeginFrame(Color aClearColor)
{
	myBackBuffer.Clear(aClearColor.AsLinearVec4());
	myDepthBuffer.Clear(1.0f, 0);

	DX11::BackBuffer->SetAsActiveTarget();
}

void DX11::EndFrame(bool aEnableVSync)
{
	if (aEnableVSync)
	{
		DX11::SwapChain->Present(1, 0);
	}
	else
	{
		DX11::SwapChain->Present(0, 0);
	}
}

bool DX11::IsOnSameThreadAsEngine()
{
	return RenderThreadId == std::this_thread::get_id();
}

Vector2ui DX11::GetResolution()
{
	return
	{
		static_cast<unsigned int>(myWindowHandler->GetWidth()),
		static_cast<unsigned int>(myWindowHandler->GetHeight())
	};
}

bool DX11::LoadPixelShader(const char* aPath, ID3D11PixelShader** aPixelShader)
{
	HRESULT result;

	std::ifstream vsFile;
	vsFile.open(aPath, std::ios::binary);
	std::string data = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(data.data(), data.size(), nullptr, aPixelShader);
	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();

	return true;
}

bool DX11::LoadVertexShader(const char* aPath, ID3D11VertexShader** aVertexShader, std::string& data)
{
	HRESULT result;

	std::ifstream vsFile;
	vsFile.open(aPath, std::ios::binary);
	data = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(data.data(), data.size(), nullptr, aVertexShader);
	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();

	return true;
}

bool DX11::LoadGeometryShader(const char* aPath, ID3D11GeometryShader** aGeometryShader)
{
	HRESULT result;

	std::ifstream vsFile;
	vsFile.open(aPath, std::ios::binary);
	std::string data = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(data.data(), data.size(), nullptr, aGeometryShader);
	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();

	return true;
}

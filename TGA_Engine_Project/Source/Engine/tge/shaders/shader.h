#pragma once
#include <tge/math/matrix4x4.h>
#include <tge/render/RenderObject.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <tge/EngineDefines.h>
#include <wrl/client.h>
#include <functional>

using Microsoft::WRL::ComPtr;

namespace Tga
{
    struct CommonConstantBufferData
    {
        Vector4f myResolution;
        Vector4f myTimings;
        Matrix4x4f myToCamera;
        Matrix4x4f myToProjection;
        Vector4f myCameraPosition;
        float myNearPlane;
        float myFarPlane;
        float myUnused0;
        float myUnused1;
    };

    struct LightConstantBufferData
    {
        struct PointLightData
        {
            Vector4f myPosition;
            Vector4f myColorAndIntensity;
            float myRange;
            Vector3f garbage; // Padding
        } myPointLights[NUMBER_OF_LIGHTS_ALLOWED];

        unsigned int myNumberOfLights;
        int myNumEnvMapMipLevels;
        float garbage0;
        float garbage1;

        Vector4f myAmbientLightColorAndIntensity;
        Vector4f myDirectionalLightDirection;
        Vector4f myDirectionalLightColorAndIntensity;
    };

    class GraphicsEngine;
    class Engine;
    class Shader
    {
    public:
        Shader(Engine& aEngine);
        virtual ~Shader();
        virtual bool Init(){ return false; }

		typedef std::function<void(const std::string& aBlob)> callback_layout;
		bool CreateShaders(const wchar_t* aVertex, const wchar_t* aPixel, callback_layout aLayout = nullptr);
        bool PrepareRender(const RenderObjectSharedData& sharedData) const;

    protected:
		virtual bool CreateInputLayout(const std::string& aVS) { aVS; return false; }
        void DoOneFrameUpdates() const;
        ComPtr<ID3D11VertexShader> myVertexShader;    // the vertex shader
        ComPtr<ID3D11PixelShader> myPixelShader;     // the pixel shader
        ComPtr<ID3D11InputLayout> myLayout;            // the pointer to the input layout
       
        GraphicsEngine* myDirect3dEngine;
        Engine* myEngine;

		bool myIsReadyToRender;
		int myRandomSeed;

        std::wstring myVertexShaderFile;
        std::wstring myPixelShaderFile;
    };
}
#pragma once
#include <tge/render/RenderCommon.h>
#include <tge/shaders/shader.h>

namespace Tga
{
    struct CustomShapeConstantBufferData
    {
		Matrix4x4f ModelToWorld;
    };

    class GraphicsEngine;
    class CustomShape;
    class CustomShape2D;
    class CustomShape3D;
    class RenderObjectCustom;
    class CustomShapeDrawer : public Shader
    {
    public:
        CustomShapeDrawer(Engine& anEngine);
        ~CustomShapeDrawer();
        bool Init();
        void Draw(CustomShape2D& aObject);
        void Draw(CustomShape3D& aObject);

    private:
        CustomShapeDrawer &operator =( const CustomShapeDrawer &anOther ) = delete;

        bool CreateInputLayout(const std::string& aVS) override;
        bool InitShaders();
        void CreateBuffer();

        int SetShaderParameters(CustomShape& aObject);
        int SetShaderParameters(CustomShape2D& aObject);
        int SetShaderParameters(CustomShape3D& aObject);

        int UpdateVertexes(CustomShape& aObject);
        ComPtr<ID3D11Buffer> myVertexBuffer;
        ComPtr<ID3D11Buffer> myObjectBuffer;
        int myMaxPoints = 0;
    };
}
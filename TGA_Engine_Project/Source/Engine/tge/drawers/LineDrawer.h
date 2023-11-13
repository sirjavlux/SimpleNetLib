#pragma once
#include <tge/render/RenderCommon.h>
#include <tge/shaders/Shader.h>

namespace Tga
{
    struct LinePrimitive;
    struct LineMultiPrimitive;
    class GraphicsEngine;
    class LineDrawer : public Shader
    {
    public:
        LineDrawer(Engine& anEngine);
        ~LineDrawer();
        bool Init() override;
        void Draw(LinePrimitive& aObject);
		void Draw(LineMultiPrimitive& aObject);
    private:
        LineDrawer &operator =( const LineDrawer &anOther ) = delete;
        bool CreateInputLayout(const std::string& aVS) override;
        bool InitShaders();
        void CreateBuffer();

        void SetShaderParameters(LinePrimitive& aObject);
        void UpdateVertexes(LinePrimitive& aObject);
        ComPtr<ID3D11Buffer> myVertexBuffer;
        SimpleVertex myVertices[2] = {{},{}};
    };
}
// TGP.cpp : Defines the entry point for the application.
//

#include <fstream>
#include <iomanip>
#include <tge/light/LightManager.h>
#include <tge/graphics/AmbientLight.h>
#include <tge/graphics/DirectionalLight.h>
#include <tge/graphics/PointLight.h>
#include <tge/input/InputManager.h>
#include <tge/error/ErrorManager.h>
#include <tge/graphics/Camera.h>
#include <tge/graphics/DX11.h>
#include <tge/drawers/ModelDrawer.h>
#include <tge/graphics/FullscreenEffect.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/graphics/RenderTarget.h>
#include <tge/Model/ModelFactory.h>
#include <tge/Model/ModelInstance.h>
#include <tge/graphics/RenderStateManager.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/Timer.h>
#include <tge/settings/settings.h>

#ifdef _DEBUG
#pragma comment(lib,"Engine_Debug.lib")
#pragma comment(lib,"External_Debug.lib")
#else
#pragma comment(lib,"Engine_Release.lib")
#pragma comment(lib,"External_Release.lib")
#endif // _DEBUG

#pragma comment (lib, "assimp-vc142-mt.lib")
#pragma comment (lib, "d3d11.lib")

using namespace Tga;

float camSpeed = 1000.f;
float camRotSpeed = 1.f;

struct RenderData
{
    DepthBuffer myIntermediateDepth;
    RenderTarget myIntermediateTexture;

    std::vector<std::shared_ptr<ModelInstance>> myDeferredModels;
    std::vector<std::shared_ptr<ModelInstance>> myModels;
    std::vector<std::shared_ptr<PointLight>> myPointLights;
    std::shared_ptr<DirectionalLight> myDirectionalLight;
    std::shared_ptr<AmbientLight> myAmbientLight;
    std::shared_ptr<Camera> myMainCamera;

    Tga::SpriteSharedData mySpriteSharedData;
    Tga::Sprite3DInstanceData mySpriteInstanceData;
};

void Render(RenderData& renderData, GraphicsEngine& graphicsEngine)
{
    ////////////////////////////////////////////////////////////////////////////////
    //// Cleanup

    renderData.myIntermediateTexture.Clear();
    renderData.myIntermediateDepth.Clear();

    ////////////////////////////////////////////////////////////////////////////////
    //// Update Camera

    graphicsEngine.SetCamera(*renderData.myMainCamera);
    graphicsEngine.UpdateAndBindCommonBuffer();

    ////////////////////////////////////////////////////////////////////////////////
    //// Prepare lights

    Engine::GetInstance()->GetLightManager().SetDirectionalLight(*renderData.myDirectionalLight);
    Engine::GetInstance()->GetLightManager().SetAmbientLight(*renderData.myAmbientLight);
    graphicsEngine.UpdateAndBindLightBuffer();

    Engine::GetInstance()->GetLightManager().ClearPointLights();
    for (std::shared_ptr<PointLight>& pointLight : renderData.myPointLights)
    {
        Engine::GetInstance()->GetLightManager().AddPointLight(*pointLight);
    }
    

    ////////////////////////////////////////////////////////////////////////////////
    //// Draw all forward rendered objects

    DX11::RenderStateManager->SetBlendState(BlendState::Disabled);
    renderData.myIntermediateTexture.SetAsActiveTarget(&renderData.myIntermediateDepth);

    for (auto& modelInstance : renderData.myModels)
    {
       graphicsEngine.GetModelDrawer().DrawPbr(*modelInstance);
    }

    ////////////////////////////////////////////////////////////////////////////////

    {
        Tga::SpriteBatchScope batch = graphicsEngine.GetSpriteDrawer().BeginBatch(renderData.mySpriteSharedData);
        batch.Draw(renderData.mySpriteInstanceData);
    }

    ////////////////////////////////////////////////////////////////////////////////

    DX11::RenderStateManager->SetBlendState(BlendState::Disabled);
    DX11::BackBuffer->SetAsActiveTarget();
    renderData.myIntermediateTexture.SetAsResourceOnSlot(0);
    graphicsEngine.GetFullscreenEffectTonemap().Render();
}

void Go(void);
int main(const int /*argc*/, const char* /*argc*/[])
{
    Go();
    return 0;
}

Tga::InputManager* SInputManager;

// This is where the application starts of for real. By keeping this in it's own file
// we will have the same behaviour for both console and windows startup of the
// application.
//
void Go(void)
{
    Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
    Tga::EngineConfiguration createParameters;

    createParameters.myWinProcCallback = [](HWND, UINT message, WPARAM wParam, LPARAM lParam)
    {
        SInputManager->UpdateEvents(message, wParam, lParam);
        return 0;
    };

    createParameters.myApplicationName = L"TGA 2D Tutorial 16";
    createParameters.myActivateDebugSystems = Tga::DebugFeature::Fps |
        Tga::DebugFeature::Mem |
        Tga::DebugFeature::Drawcalls |
        Tga::DebugFeature::Cpu |
        Tga::DebugFeature::Filewatcher |
        Tga::DebugFeature::OptimizeWarnings;
    createParameters.myEnableVSync = true;

    if (!Tga::Engine::Start(createParameters))
    {
        ERROR_PRINT("Fatal error! Engine could not start!");
        system("pause");
        return;
    }

    {
        Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();
        bool bShouldRun = true;

        RenderData renderData;
        {
            renderData.myIntermediateDepth = DepthBuffer::Create(DX11::GetResolution());
            renderData.myIntermediateTexture = RenderTarget::Create(DX11::GetResolution(), DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);
        }

        HWND windowHandle = *Tga::Engine::GetInstance()->GetHWND();

        Tga::InputManager myInputManager(windowHandle);
        SInputManager = &myInputManager;
        bool isMouseTrapped = false;


        renderData.mySpriteSharedData = {};
        renderData.mySpriteSharedData.myTexture = Tga::Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/tge_logo_w.dds");

        renderData.mySpriteInstanceData = {};
        renderData.mySpriteInstanceData.myTransform = Matrix4x4f::CreateScaleMatrix({ 100.f, 100.f, 100.f });
        renderData.mySpriteInstanceData.myTransform.SetPosition({ 0, 300, 0 });

        ModelFactory& modelFactory = ModelFactory::GetInstance();

        // TODO: ModelFactory needs to spit out shared ptrs.
        std::shared_ptr<ModelInstance> mdlPlane = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Plane"));
        mdlPlane->SetScale({ 10 });

        std::shared_ptr<ModelInstance> mdlCube = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Cube"));
        mdlCube->SetLocation({ 0, 50, 400 });

        std::shared_ptr<ModelInstance> mdlCube2 = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Cube"));
        mdlCube2->SetLocation({ -200, 50, 400 });

        // When meshes are loaded, textures are also automatically loaded.
        // If textures are named the same as the materials in the model (plus _C, _M or _N for different types of textures), they are automatically loaded.
        //std::shared_ptr<ModelInstance> matball = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"../source/tutorials/Tutorial17PBR/data/TMA_Matball.fbx"));
        std::shared_ptr<ModelInstance> matball = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"TMA_Matball.fbx"));
        matball->SetLocation({ 300, 0, 0 });
        matball->SetRotation(Rotator(0, 80, 0));

        // If textures named the same as materials are not found, textures named the same as the fbx-file itself are loaded:
        std::shared_ptr<ModelInstance> mdlChest = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Particle_Chest.fbx"));
        mdlChest->SetLocation({ 100, 0, 0 });

        auto dLight = DirectionalLight(
            Transform{
                Vector3f(0, 0, 0),
                Rotator(225, 0, 0)
            },
            Color{ 0.2f, 0.2f, 0.2f },
            0.f
        );

        std::wstring cubeMap = Settings::ResolveGameAssetPathW("cube_1024_preblurred_angle3_Skansen3.dds");        

        auto aLight = AmbientLight(
            cubeMap,
            Color { 0.0f, 0.0f, 0.0f },
            0.f   
        );

        std::shared_ptr<Camera> camera = std::make_shared<Camera>(Camera());
        
        camera->SetPerspectiveProjection(
            90,
            {
                (float)resolution.x,
                (float)resolution.y
            },
            0.1f,
            50000.0f);

        camera->SetPosition(Vector3f(0.0f, 500.0f, -550.0f));
        camera->SetRotation(Rotator(45, 0, 0));

        renderData.myModels.push_back(mdlPlane);
        renderData.myModels.push_back(mdlCube);
        renderData.myModels.push_back(mdlChest);
        renderData.myModels.push_back(mdlCube2);
        renderData.myModels.push_back(matball);

        renderData.myMainCamera = camera;
        renderData.myAmbientLight = std::make_shared<AmbientLight>(aLight);
        renderData.myDirectionalLight = std::make_shared<DirectionalLight>(dLight);

        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(0.f, 400.f, 200.f),
                Rotator::Zero
            },
            Color { 0.2f, 1.0f, 0.2f },
            100000.0f,
            800.f));
        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(-200.f, 400.f, 100.f),
                Rotator::Zero
            },
            Color { 0.2f, 0.2f, 1.0f }, 
            100000.0f,
            800.f));
        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(200.f, 400.f, -100.f),
                Rotator::Zero
            },
            Color { 1.f, 1.0f, 1.0f }, 
            50000.0f,
            800.f));

        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(-500.f, 400.f, 0.f),
                Rotator(90.f, 0.f, 0.f)
            },
            Color { 1.f, 0.2f, 0.2f }, 
            100000.0f,
            700.f));

        MSG msg = { 0 };

        Timer timer;

        while (bShouldRun)
        {

            timer.Update();
            myInputManager.Update();

            Transform modelTransform = mdlCube->GetTransform();
            modelTransform.AddRotation(Vector3f(0, 100, 0) * timer.GetDeltaTime());
            mdlCube->SetTransform(modelTransform);

            Transform camTransform = camera->GetTransform();
            Vector3f camMovement = Vector3f::Zero;
            Rotator camRotation = Rotator::Zero;

            // Only read 3D navigation input if
            // the mouse is currently trapped.
            if (isMouseTrapped)
            {
                if (myInputManager.IsKeyHeld(0x57)) // W
                {
                    camMovement += camTransform.GetMatrix().GetForward() * 1.0f;
                }
                if (myInputManager.IsKeyHeld(0x53)) // S
                {
                    camMovement += camTransform.GetMatrix().GetForward() * -1.0f;
                }
                if (myInputManager.IsKeyHeld(0x41)) // A
                {
                    camMovement += camTransform.GetMatrix().GetRight() * -1.0f;
                }
                if (myInputManager.IsKeyHeld(0x44)) // D
                {
                    camMovement += camTransform.GetMatrix().GetRight() * 1.0f;
                }

                camera->SetPosition(camera->GetTransform().GetPosition() + camMovement * camSpeed * timer.GetDeltaTime());

                const Vector2f mouseDelta = myInputManager.GetMouseDelta();

                camRotation.X = mouseDelta.Y;
                camRotation.Y = mouseDelta.X;

                camTransform.AddRotation(camRotation * camRotSpeed);

                camera->SetRotation(camTransform.GetRotation());
            }

            if (myInputManager.IsKeyPressed(VK_RBUTTON))
            {
                // Capture mouse.
                if (!isMouseTrapped)
                {
                    myInputManager.HideMouse();
                    myInputManager.CaptureMouse();
                    isMouseTrapped = true;
                }
            }

            if (myInputManager.IsKeyReleased(VK_RBUTTON))
            {
                // When we let go, recapture.
                if (isMouseTrapped)
                {
                    myInputManager.ShowMouse();
                    myInputManager.ReleaseMouse();
                    isMouseTrapped = false;
                }
            }

            if (myInputManager.IsKeyPressed(VK_SHIFT))
            {
                // When we hold shift, "sprint".
                camSpeed /= 4;
            }

            if (myInputManager.IsKeyReleased(VK_SHIFT))
            {
                // When we let go, "walk".
                camSpeed *= 4;
            }

            if (myInputManager.IsKeyPressed(VK_ESCAPE))
            {
                PostQuitMessage(0);
            }

            if (!Tga::Engine::GetInstance()->BeginFrame())
            {
                break;
            }

            Render(renderData, Tga::Engine::GetInstance()->GetGraphicsEngine());

            Tga::Engine::GetInstance()->EndFrame();
        }
    }

    Tga::Engine::GetInstance()->Shutdown();

    return;
}

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

#include "unitysceneloader/UnityLoader.h"

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

void Render(RenderData& renderData, GraphicsEngine& graphicsEngine)
{

    //// Update Camera

    const std::shared_ptr<Camera> renderCamera = renderData.myMainCamera;
    graphicsEngine.SetCamera(*renderCamera);
    graphicsEngine.UpdateAndBindCommonBuffer();

    ////////////////////////////////////////////////////////////////////////////////
    //// Draw all forward rendered objects

    DX11::RenderStateManager->SetBlendState(BlendState::Disabled);
    renderData.myBackBuffer->SetAsActiveTarget(DX11::DepthBuffer);

    for (auto& modelInstance : renderData.myModels)
    {
        graphicsEngine.GetModelDrawer().Draw(*modelInstance);
    }

    ////////////////////////////////////////////////////////////////////////////////

    {
        Tga::SpriteBatchScope batch = graphicsEngine.GetSpriteDrawer().BeginBatch(renderData.mySpriteSharedData);
        batch.Draw(renderData.mySpriteInstanceData);
    }

    renderData.myBackBuffer->SetAsActiveTarget();
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

    createParameters.myApplicationName = L"TGA 2D Tutorial 17 - Unity Scene Loader";
    createParameters.myActivateDebugSystems = Tga::DebugFeature::Fps |
        Tga::DebugFeature::Mem |
        Tga::DebugFeature::Drawcalls |
        Tga::DebugFeature::Cpu |
        Tga::DebugFeature::Filewatcher |
        Tga::DebugFeature::OptimizeWarnings;

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
            renderData.myBackBuffer = DX11::BackBuffer;
        }

        HWND windowHandle = *Tga::Engine::GetInstance()->GetHWND();

        Tga::InputManager myInputManager(windowHandle);
        SInputManager = &myInputManager;
        bool isMouseTrapped = false;


        renderData.mySpriteSharedData = {};
        renderData.mySpriteInstanceData = {};

        auto dLight = DirectionalLight(
            Transform{
                Vector3f(0, 0, 0),
                Rotator(225, 0, 0)
            },
            Color{ 1, 1, 1 },
            0.f
        );

        renderData.myUnityLoader = std::make_unique<UnityLoader>(UnityLoader());
        renderData.myUnityLoader->LoadGameObjectsASCII("exportedLevelASCII.txt", renderData);
        
        Camera& camera = *renderData.myMainCamera;
        
        MSG msg = { 0 };

        Timer timer;

        
        while (bShouldRun)
        {
            timer.Update();
            myInputManager.Update();

            Transform camTransform = camera.GetTransform();
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

                camera.SetPosition(camera.GetTransform().GetPosition() + camMovement * camSpeed * timer.GetDeltaTime());

                const Vector2f mouseDelta = myInputManager.GetMouseDelta();

                camRotation.X = mouseDelta.Y;
                camRotation.Y = mouseDelta.X;

                camTransform.AddRotation(camRotation * camRotSpeed);

                camera.SetRotation(camTransform.GetRotation());
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

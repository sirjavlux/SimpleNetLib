// TGP.cpp : Defines the entry point for the application.
//

#include <fstream>
#include <iomanip>
#include <tge/animation/AnimationPlayer.h>
#include <tge/drawers/CustomShapeDrawer.h>
#include <tge/drawers/LineDrawer.h>
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
#include <tge/primitives/CustomShape.h>
#include <tge/primitives/LinePrimitive.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/Timer.h>
#include <tge/settings/settings.h>

#pragma comment (lib, "assimp-vc142-mt.lib")
#pragma comment (lib, "d3d11.lib")

using namespace Tga;

float camSpeed = 1000.f;
float camRotSpeed = 1.f;

struct RenderData
{
    RenderTarget* myBackBuffer;

    std::vector<Tga::LinePrimitive> myLines;
    std::vector<std::shared_ptr<ModelInstance>> myModels;
    std::vector<AnimatedModelInstance*> myAnimatedModels;
    std::shared_ptr<Camera> myMainCamera;

    Tga::SpriteSharedData mySpriteSharedData;
    Tga::Sprite3DInstanceData mySpriteInstanceData;
    Tga::CustomShape3D myCustomShape;
};

void Render(RenderData& renderData, GraphicsEngine& graphicsEngine)
{

    ////////////////////////////////////////////////////////////////////////////////
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

    for (auto& modelInstance : renderData.myAnimatedModels)
    {
        graphicsEngine.GetModelDrawer().Draw(*modelInstance);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Sprite with 3d coordinates
    {
        Tga::SpriteBatchScope batch = graphicsEngine.GetSpriteDrawer().BeginBatch(renderData.mySpriteSharedData);
        batch.Draw(renderData.mySpriteInstanceData);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // 3d line rendering
    {
        for (auto& line : renderData.myLines)
        {
            Tga::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer().Draw(line);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Custom shape rendering
    {
        renderData.myCustomShape.Reset();
        renderData.myCustomShape.AddPoint({  1.0f, 0.00f,  1.f }, Tga::Color(1, 0, 0, 1));
        renderData.myCustomShape.AddPoint({  0.0f, 1.41f,  0.f }, Tga::Color(0, 1, 0, 1));
        renderData.myCustomShape.AddPoint({  1.0f, 0.00f, -1.f }, Tga::Color(0, 0, 1, 1));

        renderData.myCustomShape.AddPoint({ -1.0f, 0.00f, -1.f }, Tga::Color(1, 0, 0, 1));
        renderData.myCustomShape.AddPoint({  0.0f, 1.41f,  0.f }, Tga::Color(0, 1, 0, 1));
        renderData.myCustomShape.AddPoint({ -1.0f, 0.00f,  1.f }, Tga::Color(0, 0, 1, 1));

        renderData.myCustomShape.AddPoint({  1.0f, 0.00f,  1.f }, Tga::Color(1, 0, 0, 1));
        renderData.myCustomShape.AddPoint({  0.0f, 1.41f,  0.f }, Tga::Color(0, 1, 0, 1));
        renderData.myCustomShape.AddPoint({ -1.0f, 0.00f,  1.f }, Tga::Color(0, 0, 1, 1));

        renderData.myCustomShape.AddPoint({ -1.0f, 0.00f, -1.f }, Tga::Color(1, 0, 0, 1));
        renderData.myCustomShape.AddPoint({  0.0f, 1.41f,  0.f }, Tga::Color(0, 1, 0, 1));
        renderData.myCustomShape.AddPoint({  1.0f, 0.00f, -1.f }, Tga::Color(0, 0, 1, 1));

        renderData.myCustomShape.BuildShape();

        Tga::Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer().Draw(renderData.myCustomShape);
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

    createParameters.myApplicationName = L"TGA 2D Tutorial 16";
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

        std::shared_ptr<ModelInstance> mdlChest = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Particle_Chest.fbx"));
        mdlChest->SetLocation({ 100, 0, 0 });

        auto dLight = DirectionalLight(
            Transform{
                Vector3f(0, 0, 0),
                Rotator(225, 0, 0)
            },
            Color{ 1, 1, 1 },
            0.f
        );

        Tga::AnimatedModelInstance animatedModel = modelFactory.GetAnimatedModelInstance(L"CH_NPC_Undead_17G3_SK.fbx");
        animatedModel.SetLocation({ 0.0f, 100.0f, 0.0f });

        Tga::AnimationPlayer animation = modelFactory.GetAnimationPlayer(L"CH_NPC_Undead@Idle_01_17G3_AN.fbx", animatedModel.GetModel());
        animation.SetIsLooping(true);

        renderData.myAnimatedModels.push_back(&animatedModel);

        std::shared_ptr<Camera> camera = std::make_shared<Camera>(Camera());
        camera->SetPerspectiveProjection(
            90,
            {
                (float)resolution.x,
                (float)resolution.y
            },
            0.1f,
            50000.0f)
            ;

        camera->SetPosition(Vector3f(0.0f, 500.0f, -550.0f));
        camera->SetRotation(Rotator(45, 0, 0));

        renderData.myModels.push_back(mdlPlane);
        renderData.myModels.push_back(mdlCube);
        renderData.myModels.push_back(mdlChest);
        renderData.myModels.push_back(mdlCube2);

        renderData.myCustomShape.GetTransform().SetPosition({ -300.0f, 100.0f, 0.0f });
        renderData.myCustomShape.GetTransform().SetScale({ 75.0f, 75.0f, 75.0f });

        renderData.myMainCamera = camera;
        
        MSG msg = { 0 };

        Timer timer;

        animation.Play();
        while (bShouldRun)
        {
            renderData.myLines.clear();

            timer.Update();
            myInputManager.Update();

            Transform modelTransform = mdlCube->GetTransform();
            modelTransform.AddRotation(Vector3f(0, 100, 0) * timer.GetDeltaTime());
            mdlCube->SetTransform(modelTransform);

            Transform camTransform = camera->GetTransform();
            Vector3f camMovement = Vector3f::Zero;
            Rotator camRotation = Rotator::Zero;
            
            animation.Update(timer.GetDeltaTime());
            animatedModel.SetPose(animation);

            {
                // Example of getting information from the pose of a character:

                // Find the bone index:
                std::string boneName = "RightHand";
                size_t boneIndex = animatedModel.GetModel()->GetSkeleton()->JointNameToIndex.find(boneName)->second;

                // Convert pose to model space
                ModelSpacePose pose;
                animatedModel.GetModel()->GetSkeleton()->ConvertPoseToModelSpace(animation.GetLocalSpacePose(), pose);

                // Find the hands transform in world space
                Matrix4x4 animatedModelTransform = animatedModel.GetTransform().GetMatrix();
                Matrix4x4 handToWorldTransform = pose.JointTransforms[boneIndex] * animatedModelTransform;

                // Specify the line's position relative to the hand and convert to world space:
                Vector4f v0 = Vector4f(-10.f, 8.f, 3.f, 1.f) * handToWorldTransform;
                Vector4f v1 = Vector4f(-10.f, -100.f, 3.f, 1.f) * handToWorldTransform;

                // Draw the line:
                Tga::LinePrimitive line;
                line.color = { 0.0f, 0.0f, 0.0f, 1.f };
                line.fromPosition = v0;
                line.toPosition = v1;
                renderData.myLines.push_back(line);
            }

            {
                // Example lines, forming an yellow square:
                Tga::LinePrimitive line;
                line.color = { 0.95f, 0.55f, 0.3f, 1.f };

                line.fromPosition = { -100.f, 100.f, 0.f };
                line.toPosition = { -200.f, 100.f, 0.f };
                renderData.myLines.push_back(line);

                line.fromPosition = line.toPosition;
                line.toPosition = { -200.f, 200.f, 0.f };
                renderData.myLines.push_back(line);

                line.fromPosition = line.toPosition;
                line.toPosition = { -100.f, 200.f, 0.f };
                renderData.myLines.push_back(line);

                line.fromPosition = line.toPosition;
                line.toPosition = { -100.f, 100.f, 0.f };
                renderData.myLines.push_back(line);
            }

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

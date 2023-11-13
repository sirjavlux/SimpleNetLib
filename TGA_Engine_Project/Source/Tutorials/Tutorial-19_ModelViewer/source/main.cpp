// TGP.cpp : Defines the entry point for the application.
//

#include <fstream>
#include <iomanip>
#include <tge/animation/AnimationPlayer.h>
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
#include <tge/imguiinterface/ImGuiInterface.h>
#include <filesystem>
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
namespace fs = std::filesystem;
struct RenderData
{
    RenderTarget* myBackBuffer;

    std::shared_ptr<ModelInstance> myModel;
    std::shared_ptr<AnimatedModelInstance> myAnimatedModel;
    std::shared_ptr<AnimationPlayer> myAnimation;
    std::shared_ptr<DirectionalLight> myDirectionalLight;
    std::shared_ptr<AmbientLight> myAmbientLight;

    std::shared_ptr<Camera> myMainCamera;

    SpriteSharedData mySpriteSharedData;
    Sprite3DInstanceData mySpriteInstanceData;

    std::shared_ptr<ModelInstance> myPlane;
    Vector3f targetPosition = (0.0f, 0.0f, 0.0f);

    bool showFloor, playAnimation;
};

struct LightData
{
    float lightDirections[3];
    float cubemapIntensity = 1.0f;
    float rgb[3];
    std::string path = Settings::ResolveGameAssetPath("cubemap/");
    std::vector<std::wstring> w_cubemaps;
    std::vector<std::string> s_cubemaps;
    std::wstring cubeMap = Settings::ResolveGameAssetPathW(L"cubemap/Gdansk_shipyard_buildings_cubemap.dds");
};

RenderData GlobalRenderData;
LightData GlobalLightData;

void Render(RenderData& renderData, Tga::GraphicsEngine& graphicsEngine)
{

    ////////////////////////////////////////////////////////////////////////////////
    //// Update Camera

    const std::shared_ptr<Camera> renderCamera = renderData.myMainCamera;
    graphicsEngine.SetCamera(*renderCamera);
    graphicsEngine.UpdateAndBindCommonBuffer();
    Engine::GetInstance()->GetLightManager().SetDirectionalLight(*renderData.myDirectionalLight);
    Engine::GetInstance()->GetLightManager().SetAmbientLight(*renderData.myAmbientLight);

    graphicsEngine.UpdateAndBindLightBuffer();

    ////////////////////////////////////////////////////////////////////////////////
    //// Draw all forward rendered objects

    DX11::RenderStateManager->SetBlendState(BlendState::Disabled);
    renderData.myBackBuffer->SetAsActiveTarget(DX11::DepthBuffer);

    if(renderData.myModel)
    {
        graphicsEngine.GetModelDrawer().DrawPbr(*renderData.myModel);
    }
    if(renderData.myAnimatedModel)
    {
        graphicsEngine.GetModelDrawer().DrawPbr(*renderData.myAnimatedModel);
    }
    if (GlobalRenderData.showFloor) 
    {
        graphicsEngine.GetModelDrawer().DrawPbr(*renderData.myPlane);
    }

    ////////////////////////////////////////////////////////////////////////////////


    renderData.myBackBuffer->SetAsActiveTarget();
}

void Go(void);
int main(const int /*argc*/, const char* /*argc*/[])
{
    Go();
    return 0;
}

Tga::InputManager* SInputManager;
void CreatePreviewForAsset(std::wstring filePath)
{
    ModelFactory& modelFactory = ModelFactory::GetInstance();
    bool hasMesh = modelFactory.ModelHasMesh(filePath);

    if (hasMesh) 
    {
        std::shared_ptr <Tga::AnimatedModelInstance> animatedModel = std::make_shared<Tga::AnimatedModelInstance>(modelFactory.GetAnimatedModelInstance(filePath));

        if (animatedModel->IsValid()) //If the model has an animation
        {
            GlobalRenderData.myAnimatedModel = animatedModel;
            GlobalRenderData.myModel = nullptr;
            GlobalRenderData.myAnimation = std::make_shared<Tga::AnimationPlayer>(modelFactory.GetAnimationPlayer(filePath, animatedModel->GetModel()));
            GlobalRenderData.targetPosition = GlobalRenderData.myAnimatedModel->GetModel()->GetMeshData(0).Bounds.Center;

        }
        else
        {
            std::shared_ptr<ModelInstance> currentModelToPreview = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(filePath));
            if (currentModelToPreview->IsValid()) 
            {
                GlobalRenderData.myModel = currentModelToPreview;
                GlobalRenderData.myAnimatedModel = nullptr;
                GlobalRenderData.targetPosition = GlobalRenderData.myModel->GetModel()->GetMeshData(0).Bounds.Center;

            }
        }
    }
    else if (!hasMesh && GlobalRenderData.myAnimatedModel )
    {
        GlobalRenderData.myAnimation = std::make_shared<Tga::AnimationPlayer>(modelFactory.GetAnimationPlayer(filePath, GlobalRenderData.myAnimatedModel->GetModel()));
    }
    //assert(!hasMesh && !GlobalRenderData.myAnimatedModel, "Error - No valid model was found");
    // error!
}

bool IsFBX(std::wstring aPath) //Return true if the filepath ends with .fbx (this is for the drag-and-drop model viewer)
{
    if (aPath.substr(aPath.find_last_of(L".") + 1) == L"fbx")
    {
        return true;
    }
    else
    {
        return false;
    }
}
void HandleInput(InputManager myInputManager,
    std::shared_ptr<Camera> camera,
    float camRotSpeed,
    bool& isScrollMouseTrapped,
    bool& isAltDown,
    bool& isLeftMouseTrapped,
    bool& isRightMouseTrapped)
{
    Transform camTransform = camera->GetTransform();
    Vector3f camMovement = Vector3f::Zero;
    Rotator camRotation = Rotator::Zero;
    const Vector2f mouseDelta = myInputManager.GetMouseDelta();

    if (isScrollMouseTrapped && isAltDown)
    {
        camMovement += camTransform.GetMatrix().GetUp() * mouseDelta.Y;
        camMovement -= camTransform.GetMatrix().GetRight() * mouseDelta.X;
        camera->SetPosition(camera->GetTransform().GetPosition() + camMovement);
    }
    else if (isLeftMouseTrapped && isAltDown)
    {

        Vector3f camPos = camera->GetTransform().GetPosition();
        Vector3f targetPoint = GlobalRenderData.targetPosition;
        Vector3f currDistance = targetPoint - camPos;
        float relationCamToTarget = std::sqrt((currDistance.x * currDistance.x) + (currDistance.y * currDistance.y) + (currDistance.z * currDistance.z));

        camera->SetPosition(Vector3f(0.f, 0.f, 0.f));
        camRotation.X = mouseDelta.Y;
        camRotation.Y = mouseDelta.X;
        camTransform.AddRotation(camRotation * camRotSpeed);
        camera->SetRotation(camTransform.GetRotation());
        camera->SetPosition((targetPoint)+(camera->GetTransform().GetMatrix().GetForward() * -relationCamToTarget));

    }
    else if (isRightMouseTrapped && isAltDown)
    {
        camMovement += camTransform.GetMatrix().GetForward() * mouseDelta.Y;
        camera->SetPosition(camera->GetTransform().GetPosition() + camMovement);
    }

    if (myInputManager.IsKeyPressed(VK_RBUTTON))
    {
        // Capture mouse.
        if (!isRightMouseTrapped)
        {
            myInputManager.HideMouse();
            myInputManager.CaptureMouse();
            isRightMouseTrapped = true;
        }
    }


    if (myInputManager.IsKeyReleased(VK_RBUTTON))
    {
        // When we let go, recapture.
        if (isRightMouseTrapped)
        {
            myInputManager.ShowMouse();
            myInputManager.ReleaseMouse();
            isRightMouseTrapped = false;
        }
    }
    if (myInputManager.IsKeyPressed(VK_MBUTTON))
    {
        // Capture mouse.
        if (!isScrollMouseTrapped)
        {
            myInputManager.HideMouse();
            myInputManager.CaptureMouse();
            isScrollMouseTrapped = true;
        }
    }

    if (myInputManager.IsKeyReleased(VK_MBUTTON))
    {
        // When we let go, recapture.
        if (isScrollMouseTrapped)
        {
            myInputManager.ShowMouse();
            myInputManager.ReleaseMouse();
            isScrollMouseTrapped = false;
        }
    }
    if (myInputManager.IsKeyPressed(VK_LBUTTON))
    {
        // Capture mouse.
        if (!isLeftMouseTrapped)
        {
            myInputManager.HideMouse();
            myInputManager.CaptureMouse();
            isLeftMouseTrapped = true;
        }
    }

    if (myInputManager.IsKeyReleased(VK_LBUTTON))
    {
        // When we let go, recapture.
        if (isLeftMouseTrapped)
        {
            myInputManager.ShowMouse();
            myInputManager.ReleaseMouse();
            isLeftMouseTrapped = false;
        }
    }
    if (myInputManager.IsKeyPressed(0x46)) //'F'
    {
        camera->SetPosition(GlobalRenderData.targetPosition - (camera->GetTransform().GetMatrix().GetForward() * 200.0f));

        std::cout << "'F' Position: X - " << GlobalRenderData.targetPosition.X << " Y - " << GlobalRenderData.targetPosition.Y << " Z - " << GlobalRenderData.targetPosition.Z << std::endl;

    }
    if (myInputManager.IsKeyPressed(0x12)) //Left Alt
    {
        if (!isAltDown)
        {
            isAltDown = true;
        }
    }

    if (myInputManager.IsKeyReleased(0x12))//Left Alt
    {
        if (isAltDown)
        {
            isAltDown = false;
        }
    }

    if (myInputManager.IsKeyPressed(VK_ESCAPE))
    {
        PostQuitMessage(0);
    }
}
// This is where the application starts of for real. By keeping this in it's own file
// we will have the same behaviour for both console and windows startup of the
// application.
//
void ShowAboutWindow(bool* p_open);
void Go(void)
{
    Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
    GlobalLightData.path = Settings::ResolveGameAssetPath("cubemap/");
    GlobalLightData.cubeMap = Settings::ResolveGameAssetPathW(L"cubemap/Gdansk_shipyard_buildings_cubemap.dds");

	Tga::EngineConfiguration createParameters;
	createParameters.myWinProcCallback = [](HWND, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DROPFILES:
		{
			HDROP hDropInfo = (HDROP)wParam;

			UINT num_paths = DragQueryFileW(hDropInfo, 0xFFFFFFFF, 0, 512);

			wchar_t* filename = nullptr;
			UINT max_filename_len = 0;

			for (UINT i = 0; i < num_paths; ++i) {
				UINT filename_len = DragQueryFileW(hDropInfo, i, nullptr, 512) + 1;
				if (filename_len > max_filename_len) {
					max_filename_len = filename_len;
					wchar_t* tmp = (wchar_t*)realloc(filename, max_filename_len * sizeof(*filename));
					if (tmp != nullptr) {
						filename = tmp;
					}
				}
				DragQueryFileW(hDropInfo, i, filename, filename_len);
			}
            //GlobalRenderData.modelName = filename;
			if (filename > 0 && IsFBX(filename))
                CreatePreviewForAsset(filename);
			free(filename);
			DragFinish(hDropInfo);
		}
		}
		SInputManager->UpdateEvents(message, wParam, lParam);
		return 0;
	};

    createParameters.myApplicationName = L"TGA 2D Tutorial 19";
    createParameters.myActivateDebugSystems = Tga::DebugFeature::None;//Tga::DebugFeature::Fps |
    //    Tga::DebugFeature::Mem |
    //    Tga::DebugFeature::Drawcalls |
    //    Tga::DebugFeature::Cpu |
    //    Tga::DebugFeature::Filewatcher |
    //    Tga::DebugFeature::OptimizeWarnings;

    if (!Tga::Engine::Start(createParameters))
    {
        ERROR_PRINT("Fatal error! Engine could not start!");
        system("pause");
        return;
    }

    {
        Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();
        bool bShouldRun = true;
        float lightDirections[3];
        float rgb[3];
        GlobalLightData.rgb[0] = 100.0f / 255.0f; //set startup color to dark gray
        GlobalLightData.rgb[1] = 100.0f / 255.0f;
        GlobalLightData.rgb[2] = 100.0f / 255.0f;
        GlobalRenderData.showFloor = true;
        GlobalRenderData.playAnimation = true;


        GlobalLightData.w_cubemaps.reserve(32);
        GlobalLightData.s_cubemaps.reserve(32);
        for (const auto& entry : fs::directory_iterator(GlobalLightData.path))
        {
            std::string test(entry.path().string());
            std::wstring tempPath(test.begin(), test.end());
            if (tempPath.substr(tempPath.find_last_of(L".") + 1) == L"dds") //safe check. Check if file is a .dds 
            {
                GlobalLightData.s_cubemaps.push_back(test);
                GlobalLightData.w_cubemaps.push_back(tempPath);
                std::cout << "Sampling cubemap: " << entry.path() << std::endl;
            }
        }


        //RenderData renderData;
        {
            GlobalRenderData.myBackBuffer = DX11::BackBuffer;
        }

        HWND windowHandle = *Tga::Engine::GetInstance()->GetHWND();
        DragAcceptFiles(windowHandle, TRUE);
        Tga::InputManager myInputManager(windowHandle); 
        SInputManager = &myInputManager;
        bool isRightMouseTrapped = false;
        bool isLeftMouseTrapped = false;
        bool isScrollMouseTrapped = false;
        bool isAltDown = false;
        static bool show_app_about = false;

        // TODO: ModelFactory needs to spit out shared ptrs.
        GlobalRenderData.myPlane = std::make_shared<ModelInstance>(ModelFactory::GetInstance().GetModelInstance(L"Plane"));
        GlobalRenderData.myPlane->SetScale({ 10 });
        static const char* currentCubeMap = GlobalLightData.s_cubemaps[0].c_str();
        auto aLight = AmbientLight(
            GlobalLightData.cubeMap,
            Color{ 0.0f, 0.0f, 0.0f },
            GlobalLightData.cubemapIntensity
        );

        auto dLight = DirectionalLight(
            Transform{
                Vector3f(0, 0, 0),
                Rotator(270, 0, 0)
            },
            Color{ 1, 1, 1 },
            0.f
        );
        GlobalLightData.lightDirections[0] = dLight.GetTransform().GetRotation().X;
        GlobalLightData.lightDirections[1] = dLight.GetTransform().GetRotation().Y;
        GlobalLightData.lightDirections[2] = dLight.GetTransform().GetRotation().Z;
        GlobalRenderData.myDirectionalLight = std::make_shared<DirectionalLight>(dLight);
        GlobalRenderData.myAmbientLight = std::make_shared<AmbientLight>(aLight);
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(Camera());

        float camRotSpeed = 1.f;
        camera->SetPerspectiveProjection(
            90,
            {
                (float)resolution.x,
                (float)resolution.y
            },
            0.1f,
            50000.0f)
            ;



        camera->SetPosition(Vector3f(0.0f, 500.0f, 550.0f));
        camera->SetRotation(Rotator(45, 180, 0));

        GlobalRenderData.myMainCamera = camera;
        
        MSG msg = { 0 };

        Timer timer;
        static bool isOpen = false;

        while (bShouldRun)
        {
            timer.Update();
            myInputManager.Update();

            if (!Tga::Engine::GetInstance()->BeginFrame())
            {
                break;
            }
#pragma region ImGUI stuff
#ifndef _RETAIL
            if (ImGui::Begin("Light and Rendering", &isOpen, 0))
            {
                GlobalRenderData.showFloor == ImGui::Button("Toggle Platform", { 350,50 }) ? GlobalRenderData.showFloor = false : GlobalRenderData.showFloor = true;
                ImGui::MenuItem("About Modelviewer >", NULL, &show_app_about);
                if (GlobalRenderData.myAnimatedModel)
                    GlobalRenderData.playAnimation == ImGui::Button("Play Animation", { 150,30 }) ? GlobalRenderData.playAnimation = false : GlobalRenderData.playAnimation = true;
                ImGui::SliderFloat3("Directional Light", GlobalLightData.lightDirections, 0, 360);
                if (ImGui::Button("Reset Light Rotation", { 150,30 })) {
                    GlobalLightData.lightDirections[0] = 270;
                    GlobalLightData.lightDirections[1] = 0;
                    GlobalLightData.lightDirections[2] = 0;
                }
                if (ImGui::BeginCombo("CubeMaps", currentCubeMap))
                {
                    for (int n = 0; n < GlobalLightData.s_cubemaps.size(); n++)
                    {
                        bool is_selected = (currentCubeMap == GlobalLightData.s_cubemaps[n].c_str()); // You can store your selection however you want, outside or inside your objects
                        if (ImGui::Selectable(GlobalLightData.s_cubemaps[n].c_str(), is_selected))
                        {
                            auto newALight = AmbientLight(
                                GlobalLightData.w_cubemaps[n],
                                Color{ 0.0f, 0.0f, 0.0f },
                                1.f
                            );
                            GlobalRenderData.myAmbientLight = std::make_shared<AmbientLight>(newALight);

                            currentCubeMap = GlobalLightData.s_cubemaps[n].c_str();
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                    ImGui::EndCombo();
                }
                ImGui::SliderFloat("Cubemap Intensity", &GlobalLightData.cubemapIntensity, 0, 1);
                GlobalRenderData.myAmbientLight->SetIntensity(GlobalLightData.cubemapIntensity);
                ImGui::ColorPicker3("Background Color", GlobalLightData.rgb);
                if (show_app_about)
                {
                    ShowAboutWindow(&show_app_about);
                }
            }
#endif
#pragma endregion
            GlobalRenderData.myDirectionalLight->SetRotation(Rotator(lightDirections[0], lightDirections[1], lightDirections[2]));
            GlobalRenderData.myDirectionalLight->SetColor(Tga::Color(rgb[0], rgb[1], rgb[2]));
            Engine::GetInstance()->SetClearColor(Color(GlobalLightData.rgb[0], GlobalLightData.rgb[1], GlobalLightData.rgb[2]));

            if (GlobalRenderData.myAnimatedModel) //If the current model has an animation
            {
                if (GlobalRenderData.myAnimation && GlobalRenderData.myAnimation->IsValid())
                {
                    GlobalRenderData.myAnimation->SetIsLooping(true);
                    GlobalRenderData.myAnimation->Play();
                    //GlobalRenderData.myAnimatedModel->SetCurrentAnimationIndex(GlobalRenderData.myAnimatedModel->GetAnimationCount() - 1);
                    GlobalRenderData.myAnimation->Update(timer.GetDeltaTime());
                    GlobalRenderData.myAnimatedModel->SetPose(*GlobalRenderData.myAnimation);
                }
            }
#pragma region Camera

            // Only read 3D navigation input if
            // the mouse is currently trapped.
            //Transform camTransform = camera->GetTransform();
            //Vector3f camMovement = Vector3f::Zero;
            //Rotator camRotation = Rotator::Zero;
            //    const Vector2f mouseDelta = myInputManager.GetMouseDelta();
                HandleInput(myInputManager, camera, camRotSpeed, isScrollMouseTrapped, isAltDown, isLeftMouseTrapped, isRightMouseTrapped);

            //if (isScrollMouseTrapped && isAltDown)
            //{
            //    camMovement += camTransform.GetMatrix().GetUp() * mouseDelta.Y;
            //    camMovement -= camTransform.GetMatrix().GetRight() * mouseDelta.X;
            //    camera->SetPosition(camera->GetTransform().GetPosition() + camMovement);
            //}
            //else if (isLeftMouseTrapped && isAltDown)
            //{
            //    
            //    Tga::Vector3f camPos = camera->GetTransform().GetPosition();
            //    Tga::Vector3f targetPoint = camPos + (camera->GetTransform().GetMatrix().GetForward() * 200.0f);
            //    Tga::Vector3f currDistance = targetPoint - camPos;
            //    float relationCamToTarget = std::sqrt((currDistance.x * currDistance.x) + (currDistance.y * currDistance.y) + (currDistance.z * currDistance.z));
            //    camera->SetPosition(Tga::Vector3f(0.f,0.f,0.f));
            //    camRotation.X = mouseDelta.Y;
            //    camRotation.Y = mouseDelta.X;
            //    camTransform.AddRotation(camRotation * camRotSpeed);
            //    camera->SetRotation(camTransform.GetRotation());
            //    camera->SetPosition(targetPoint + (camera->GetTransform().GetMatrix().GetForward() * -relationCamToTarget));

            // }
            //else if (isRightMouseTrapped && isAltDown)
            //{
            //    camMovement += camTransform.GetMatrix().GetForward() *  (mouseDelta.Y + mouseDelta.X); //clamp X with this
            //    camera->SetPosition(camera->GetTransform().GetPosition() + camMovement);
            //}
            //else if (isRightMouseTrapped)
            //{
            //    if (myInputManager.IsKeyHeld(0x57)) // W
            //    {
            //        camMovement += camTransform.GetMatrix().GetForward() * 1.0f;
            //    }
            //    if (myInputManager.IsKeyHeld(0x53)) // S
            //    {
            //        camMovement += camTransform.GetMatrix().GetForward() * -1.0f;
            //    }
            //    if (myInputManager.IsKeyHeld(0x41)) // A
            //    {
            //        camMovement += camTransform.GetMatrix().GetRight() * -1.0f;
            //    }
            //    if (myInputManager.IsKeyHeld(0x44)) // D
            //    {
            //        camMovement += camTransform.GetMatrix().GetRight() * 1.0f;
            //    }

            //    camera->SetPosition(camera->GetTransform().GetPosition() + camMovement * camSpeed * timer.GetDeltaTime());

            //    //const Vector2f mouseDelta = myInputManager.GetMouseDelta();

            //    camRotation.X = mouseDelta.Y;
            //    camRotation.Y = mouseDelta.X;

            //    camTransform.AddRotation(camRotation * camRotSpeed);

            //    camera->SetRotation(camTransform.GetRotation());
            //}

            //if (myInputManager.IsKeyPressed(VK_RBUTTON))
            //{
            //    // Capture mouse.
            //    if (!isRightMouseTrapped)
            //    {
            //        myInputManager.HideMouse();
            //        myInputManager.CaptureMouse();
            //        isRightMouseTrapped = true;
            //    }
            //}

            //if (myInputManager.IsKeyReleased(VK_RBUTTON))
            //{
            //    // When we let go, recapture.
            //    if (isRightMouseTrapped)
            //    {
            //        myInputManager.ShowMouse();
            //        myInputManager.ReleaseMouse();
            //        isRightMouseTrapped = false;
            //    }
            //}
            //if (myInputManager.IsKeyPressed(VK_MBUTTON))
            //{
            //    // Capture mouse.
            //    if (!isScrollMouseTrapped)
            //    {
            //        myInputManager.HideMouse();
            //        myInputManager.CaptureMouse();
            //        isScrollMouseTrapped = true;
            //    }
            //}

            //if (myInputManager.IsKeyReleased(VK_MBUTTON))
            //{
            //    // When we let go, recapture.
            //    if (isScrollMouseTrapped)
            //    {
            //        myInputManager.ShowMouse();
            //        myInputManager.ReleaseMouse();
            //        isScrollMouseTrapped = false;
            //    }
            //}
            //if (myInputManager.IsKeyPressed(VK_LBUTTON))
            //{
            //    // Capture mouse.
            //    if (!isLeftMouseTrapped)
            //    {
            //        myInputManager.HideMouse();
            //        myInputManager.CaptureMouse();
            //        isLeftMouseTrapped = true;
            //    }
            //}

            //if (myInputManager.IsKeyReleased(VK_LBUTTON))
            //{
            //    // When we let go, recapture.
            //    if (isLeftMouseTrapped)
            //    {
            //        myInputManager.ShowMouse();
            //        myInputManager.ReleaseMouse();
            //        isLeftMouseTrapped = false;
            //    }
            //}
            //if (myInputManager.IsKeyPressed(0x12)) //Left Alt
            //{
            //    if (!isAltDown)
            //    {
            //        isAltDown = true;
            //    }
            //}

            //if (myInputManager.IsKeyReleased(0x12))//Left Alt
            //{
            //    if (isAltDown)
            //    {
            //        isAltDown = false;
            //    }
            //}

            //if (myInputManager.IsKeyPressed(VK_SHIFT))
            //{
            //    // When we hold shift, "sprint".
            //    camSpeed /= 4;
            //}

            //if (myInputManager.IsKeyReleased(VK_SHIFT))
            //{
            //    // When we let go, "walk".
            //    camSpeed *= 4;
            //}

            //if (myInputManager.IsKeyPressed(VK_ESCAPE))
            //{
            //    PostQuitMessage(0);
            //}
#pragma endregion

            ImGui::End();
            ImGui::EndFrame();
            Render(GlobalRenderData, Tga::Engine::GetInstance()->GetGraphicsEngine());
            Tga::Engine::GetInstance()->EndFrame();

      }

    }
    Tga::Engine::GetInstance()->Shutdown();

    return;
}
void ShowAboutWindow(bool* p_open)
{
    if (!ImGui::Begin("About Modelviewer", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Modelviewer");
    ImGui::Separator();
    ImGui::Text("Made by Folke SN");
    ImGui::Text("Modelviewer is licensed under the TGA License.");
    ImGui::Text("See ---https://wiki-tga.com/license--- for more information.");
    ImGui::Text("");
    ImGui::Separator();
    ImGui::Text("Controls");
    ImGui::Text("ALT + Left click: Rotate");
    ImGui::Text("ALT + Middle click: Move - Up | Down | Left | Right");
    ImGui::Text("ALT + Right click: Move - Forward | Backward");
    ImGui::Separator();
    ImGui::Text("Cubemaps");
    ImGui::Text("Every cubemaps inside the 'data' folder will be");
    ImGui::Text("displayed under the 'cubemaps' drop-down.");
    ImGui::Text("The user can toggle between cubemaps.");
    ImGui::Separator();
    ImGui::Text("Lightning");
    ImGui::Text("The user can choose direction of the directional light.");
    ImGui::Text("Press 'Reset Light Position' to reset the light position.");
    ImGui::End();

}

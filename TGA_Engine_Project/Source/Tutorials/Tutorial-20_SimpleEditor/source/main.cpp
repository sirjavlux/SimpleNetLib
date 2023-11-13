
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/imguiinterface/ImGuiInterface.h>
#include <tge/editor/CommandManager/AbstractCommand.h>
#include <tge/editor/CommandManager/CommandManager.h>
#include <tge/math/FMath.h>

#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/settings/settings.h>

#include <optional>
#include <string>


#ifdef _DEBUG
#pragma comment(lib,"Engine_Debug.lib")
#pragma comment(lib,"External_Debug.lib")
#else
#pragma comment(lib,"Engine_Release.lib")
#pragma comment(lib,"External_Release.lib")
#endif // _DEBUG


void Go( void );
int main( const int /*argc*/, const char * /*argc*/[] )
{
    Go();
    return 0;
}

struct Data
{
	Tga::Vector2f position = { 0.f, 0.f };
	float rotation = 0.f;
};


// To handle undo/redo actions are wrapped in commands
// Commands have functionality to both execute and undo.

class MoveCommand : public Tga::AbstractCommand
{
	std::shared_ptr<Data> myData;
	std::optional<Tga::Vector2f> myFrom;
	Tga::Vector2f myTo;

public:
	MoveCommand(const std::shared_ptr<Data>& data, Tga::Vector2f position) : myData(data), myTo(position), myFrom() {}
	~MoveCommand() override {}

	void SetPosition(Tga::Vector2f position) { myTo = position; }
	Tga::Vector2f GetPosition() const { return myTo; }

	void Execute() override 
	{
		// wait with storing the previous value until execute is called
		// otherwise code creating commands in advance, before executing, would misbehave
		if (!myFrom.has_value())
		{
			myFrom = myData->position;
		}
		myData->position = myTo;
	};
	void Undo() override 
	{
		myData->position = myFrom.value();
	};
};

class RotateCommand : public Tga::AbstractCommand
{
	std::shared_ptr<Data> myData;
	std::optional<float> myFrom;
	float myTo;

public:
	RotateCommand(const std::shared_ptr<Data>& data, float rotation) : myData(data), myTo(rotation), myFrom() {}
	~RotateCommand() override {}

	void SetRotation(float rotation) { myTo = rotation; }
	float GetRotation() const { return myTo; }

	void Execute() override
	{
		if (!myFrom.has_value())
		{
			myFrom = myData->rotation;
		}
		myData->rotation = myTo;
	};
	void Undo() override
	{
		myData->rotation = myFrom.value();
	};
};

void Go( void )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);

	TutorialCommon::Init(L"TGE: Tutorial 20 - Simple Editor");
   
	Tga::Engine& engine = *Tga::Engine::GetInstance();
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	Tga::SpriteSharedData sharedData = {};
	sharedData.myTexture = engine.GetTextureManager().GetTexture(L"sprites/tge_logo_b.dds");
	
	Tga::Sprite2DInstanceData spriteInstance = {};
	spriteInstance.myPivot = Tga::Vector2f(0.5f, 0.5f);
	spriteInstance.mySize = { 300.f, 300.f };

	std::shared_ptr<Data> gameData = std::make_shared<Data>();
	std::shared_ptr<MoveCommand> pendingMoveCommand;
	std::shared_ptr<RotateCommand> pendingRotateCommand;

	{
		Tga::Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();

		gameData->position = { 0.5f * resolution.x, 0.5f * resolution.y };
	}

	float timer = 0;
	while (true)
	{
		if (!engine.BeginFrame())
		{		
			break;
		}

		timer += 1.0f / 60.0f;
		Tga::Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();

#ifndef _RETAIL
		if (ImGui::Begin("Editor"))
		{
			Tga::Vector2f tmpPosition = gameData->position;
			float tmpRotation = gameData->rotation;

			// Sliders and all forms of dragging are a bit tricky with undo!
			// Here the command is created when the dragging starts
			// Then the command is updated while dragging

			if (ImGui::SliderFloat("Position X", &tmpPosition.x, 0.f, (float)resolution.x))
			{
				if (pendingMoveCommand == nullptr)
				{
					// No action in progress, create a new command
					pendingMoveCommand = std::make_shared<MoveCommand>(gameData, tmpPosition);		
					Tga::CommandManager::DoCommand(pendingMoveCommand);
				}
				else
				{
					// We already have a command, update it
					pendingMoveCommand->SetPosition(tmpPosition);
					pendingMoveCommand->Execute();
				}
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				// The user has stopped moving the slider, clear the pending command
				pendingMoveCommand = nullptr;
			}

			if (ImGui::SliderFloat("Position Y", &tmpPosition.y, 0.f, (float)resolution.y))
			{
				if (pendingMoveCommand == nullptr)
				{
					// No action in progress, create a new command
					pendingMoveCommand = std::make_shared<MoveCommand>(gameData, tmpPosition);
					Tga::CommandManager::DoCommand(pendingMoveCommand);
				}
				else
				{
					// We already have a command, update it
					pendingMoveCommand->SetPosition(tmpPosition);
					pendingMoveCommand->Execute();
				}
				pendingMoveCommand->Execute();
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				// The user has stopped moving the slider, clear the pending command
				pendingMoveCommand = nullptr;
			}

			if (ImGui::SliderAngle("Rotation", &tmpRotation))
			{
				if (pendingRotateCommand == nullptr)
				{
					// No action in progress, create a new command
					pendingRotateCommand = std::make_shared<RotateCommand>(gameData, tmpRotation);
					Tga::CommandManager::DoCommand(pendingRotateCommand);
				}
				else
				{
					// We already have a command, update it
					pendingRotateCommand->SetRotation(tmpRotation);
					pendingRotateCommand->Execute();
				}
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				// The user has stopped moving the slider, clear the pending command
				pendingRotateCommand = nullptr;
			}

			// Direct actions are easier
			// Register the command with the CommandManager, and we are done!
			if (ImGui::Button("Rotate 90 CW"))
			{
				float newRotation = tmpRotation - FMath::Pi_Half;
				while (newRotation < -FMath::Tau)
				{
					newRotation += FMath::Tau;
				}
				Tga::CommandManager::DoCommand(std::make_shared<RotateCommand>(gameData, newRotation));
			}

			if (ImGui::Button("Rotate 90 CCW"))
			{
				float newRotation = tmpRotation + FMath::Pi_Half;
				while (newRotation > FMath::Tau)
				{
					newRotation -= FMath::Tau;
				}
				Tga::CommandManager::DoCommand(std::make_shared<RotateCommand>(gameData, newRotation));
			}

			{
				ImGui::BeginDisabled(!Tga::CommandManager::CanUndo());
				
				if (ImGui::Button("Undo"))
				{
					Tga::CommandManager::Undo();
				}

				ImGui::EndDisabled();
			}

			{
				ImGui::BeginDisabled(!Tga::CommandManager::CanRedo());

				if (ImGui::Button("Redo"))
				{
					Tga::CommandManager::Redo();
				}

				ImGui::EndDisabled();

			}
		}

		ImGui::End();
#endif // !_RETAIL

		{
			Tga::SpriteBatchScope batch = spriteDrawer.BeginBatch(sharedData);
			spriteInstance.myPosition = gameData->position;
			spriteInstance.myRotation = gameData->rotation;
			batch.Draw(spriteInstance);
		}

		engine.EndFrame();
	}
	engine.Shutdown();
}

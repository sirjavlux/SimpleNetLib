#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>

#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/settings/settings.h>

void Go( void );
int main( const int /*argc*/, const char * /*argc*/[] )
{
    Go();
    return 0;
}


// This is where the application starts of for real. By keeping this in it's own file
// we will have the same behaviour for both console and windows startup of the
// application.
//
void Go( void )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);

	TutorialCommon::Init(L"TGE: Tutorial 2");
   
	Tga::Engine& engine = *Tga::Engine::GetInstance();

	// Get a sprite drawer, for drawing sprites
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

	// Sprite data is split into two parts. Shared data that is shared across multiple sprites and instance data that is unique per instance.
	// This split is done for performance reasons.

	// Create shared data and assign a texture.
	Tga::SpriteSharedData sharedData = {};
	sharedData.myTexture = engine.GetTextureManager().GetTexture(L"sprites/tge_logo_b.dds");
	
	// Create instance data. 
	Tga::Sprite2DInstanceData spriteInstance = {};

	// Setting the pivot so all operations will be in the middle of the image (rotation, position, etc.)
	spriteInstance.myPivot = Tga::Vector2f(0.5f, 0.5f);

	// Setting the size in pixels
	spriteInstance.mySize = { 300.f, 300.f };


	float timer = 0;
	while (true)
	{
		timer += 1.0f / 60.0f;
		if (!engine.BeginFrame())
		{
			break;
		}

		{
			// Calculate the center of the screen
			Tga::Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();
			Tga::Vector2f center = { (float)resolution.x * 0.5f, (float)resolution.y * 0.5f };

			// Start a sprite batch. This allows multiple sprites to be drawn efficiently as long as they share the same shared data.
			// The batch will be completed when the batch scope goes out of scope
			Tga::SpriteBatchScope batch = spriteDrawer.BeginBatch(sharedData);

			// Set a new position
			spriteInstance.myPosition = center + 300.f * Tga::Vector2f(cosf(timer), sinf(timer));
			// Set the rotation
			spriteInstance.myRotation = cosf(timer);

			// Draw the sprite
			batch.Draw(spriteInstance);

			// using the same instance we reuse the image and set a new position
			spriteInstance.myPosition = center + 300.f * Tga::Vector2f(sinf(timer), cosf(timer));
			spriteInstance.myRotation = 0.f;

			// Draw a second time in a new position
			batch.Draw(spriteInstance);

			// Batch goes out of scope and flushes all rendering
		}

		engine.EndFrame();
	}
	engine.Shutdown();
}

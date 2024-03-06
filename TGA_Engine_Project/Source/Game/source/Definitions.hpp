#pragma once

#define WORLD_SIZE_X 12.f
#define WORLD_SIZE_Y 12.f
#define WORLD_BG_GENERATION_SIZE_X (WORLD_SIZE_X + 2.f)
#define WORLD_BG_GENERATION_SIZE_Y (WORLD_SIZE_Y + 2.f)

enum class EDisconnectType : uint8_t
{
	UsernameTaken = 10,
};
#pragma once

#define WORLD_SIZE_X 12.f
#define WORLD_SIZE_Y 12.f
#define WORLD_BG_GENERATION_SIZE_X (WORLD_SIZE_X + 2.f)
#define WORLD_BG_GENERATION_SIZE_Y (WORLD_SIZE_Y + 2.f)

#define USERNAME_MAX_LENGTH 12

enum class EDisconnectType : char
{
	UsernameTaken = 10,
};
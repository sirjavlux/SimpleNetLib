#pragma once
#include <cstdint>

enum class EPacketComponentType : uint16_t
{
  // Start from 31 to prevent collision with internal net lib packet components
  SpawnEntity = 31,
  DeSpawnEntity = 32,
  RequestSpawnEntity = 33,
  RequestDeSpawnEntity = 34,
};

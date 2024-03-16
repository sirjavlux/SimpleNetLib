#pragma once
#include <cstdint>

enum class EPacketComponentType : uint16_t
{
  // Start from 31 to prevent collision with internal net lib packet components
  SpawnEntity = 31,
  DeSpawnEntity = 32,
  RequestSpawnEntity = 33,
  RequestDeSpawnEntity = 34,
  SpawnBullet = 35,
  SpawnEntityHasBeenReceived = 36,
  AddEntityComponentHasBeenReceived = 37,
  AddEntityComponent = 38,
  
  Input = 51,
  UpdateEntityPosition = 52,
  SetEntityPossessed = 53,
  UpdateEntityControllerPosition = 54,
  UpdateStat = 55,
  ChangeTexture = 56,
};

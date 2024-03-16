#pragma once
#include "stdafx.h"

#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ColliderComponent.h"

class EntityComponent;

class Entity
{
public:
	void NativeOnDestruction();

	void UpdateComponents(float InDeltaTime);
	void FixedUpdateComponents();

	void NativeFixedUpdate();

	void SetShouldReplicatePosition(const bool InShouldReplicatePosition) { bShouldReplicatePosition_ = InShouldReplicatePosition; }
	
	void SetTargetPosition(const Tga::Vector2f& InPos) { targetPosition_ = InPos; }
	void SetPosition(const Tga::Vector2f& InPos, bool InIsTeleport = true);
	Tga::Vector2f GetPosition() const { return position_; }
	Tga::Vector2f GetTargetPosition() const { return targetPosition_; }
	
	uint16_t GetId() const { return id_; }
	uint64_t GetTypeTagHash() const { return typeTagHash_; }

	std::weak_ptr<EntityComponent> GetComponentById(uint16_t InIdentifier);
	
	template<typename ComponentType>
	std::vector<std::weak_ptr<ComponentType>> GetComponents() const;

	template<typename ComponentType>
	std::weak_ptr<ComponentType> GetFirstComponent() const;

	Tga::Vector2f GetDirection() const { return direction_; }
	void SetDirection(const Tga::Vector2f InDirection) { direction_ = InDirection.GetNormalized(); }

	// Server Sided
	virtual void OnSendReplication(DataReplicationPacketComponent& OutComponent) {}
	// Client Sided
	virtual void OnReadReplication(const DataReplicationPacketComponent& InComponent) {} 

	bool HasBeenReceivedByClient() const { return bHasBeenReceived_; }

	bool IsEntityLocal() const { return bIsLocalEntity_; }

	void SetIsStatic(const bool InShouldBeStatic) { bIsStatic_ = InShouldBeStatic; }
	bool IsStatic() const { return bIsStatic_; }
	
	void SetIsOnlyVisual(const bool InShouldOnlyBeVisual) { bIsOnlyVisual_ = InShouldOnlyBeVisual; }
	bool IsOnlyVisual() const { return bIsOnlyVisual_; }
	
protected:
	virtual void OnDestruction() {}
	
	virtual void Init() = 0;
	virtual void InitComponents() = 0;
	
	virtual void UpdateSmoothMovement(const float InDeltaTime)
	{
		if (bIsStatic_)
		{
			return;
		}
		
		// Lerp position
		const Tga::Vector2f newPosition = Tga::Vector2f::Lerp(position_, targetPosition_, targetPositionLerpSpeed_ * InDeltaTime);
		SetPosition(newPosition, false);
	}
	
	virtual void Update(float InDeltaTime) {}
	virtual void FixedUpdate() {}
	
	void UpdateReplication();

	DataReplicationPacketComponent replicationPacketComponent_;
	
	uint16_t componentIdIter_ = 0;
	
	uint16_t id_ = 0;
	Tga::Vector2f position_;
	Tga::Vector2f direction_;

	uint64_t typeTagHash_;

	std::unordered_map<uint16_t, std::shared_ptr<EntityComponent>> componentsMap_;
	std::shared_ptr<EntityComponent> renderComponent_;

	Tga::Vector2f targetPosition_ = {};
	float targetPositionLerpSpeed_ = 8.f;

	bool bShouldReplicatePosition_ = false;

	bool bHasBeenReceived_ = false;
	bool bIsLocalEntity_ = false;
	bool bIsStatic_ = false;
	bool bIsOnlyVisual_ = false;
	
	friend class EntityManager;
};

template <typename ComponentType>
std::vector<std::weak_ptr<ComponentType>> Entity::GetComponents() const
{
	std::vector<std::weak_ptr<ComponentType>> result;
	if (std::is_base_of_v<ComponentType, EntityComponent>)
	{
		return result;
	}

	if (std::is_base_of_v<ComponentType, RenderComponent>)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(renderComponent_))
		{
			result.push_back(castedComponent);
			return result;
		}
	}
	
	for (const auto& component : componentsMap_)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(component.second))
		{
			result.push_back(std::weak_ptr<ComponentType>(castedComponent));
		}
	}

	return result;
}

template <typename ComponentType>
std::weak_ptr<ComponentType> Entity::GetFirstComponent() const
{
	if (std::is_base_of_v<ComponentType, EntityComponent>)
	{
		return std::weak_ptr<ComponentType>();
	}

	if (std::is_base_of_v<ComponentType, RenderComponent>)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(renderComponent_))
		{
			return std::weak_ptr<ComponentType>(castedComponent);
		}
	}
	
	for (auto& component : componentsMap_)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(component.second))
		{
			return std::weak_ptr<ComponentType>(castedComponent);
		}
	}

	return std::weak_ptr<ComponentType>();
}

#pragma once
#include "stdafx.h"

#include "../../../../SimpleNetLib/SimpleNetLib/Utility/NetTag.h"
#include "../../Locator/Locator.h"
#include "../EntityComponents/EntityComponent.hpp"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"
#include "../../PacketComponents/UpdateEntityPositionComponent.hpp"
#include "../Collision/CollisionManager.h"
#include "../EntityComponents/ColliderComponent.h"
#include "Packet/PacketManager.h"
#include "Packet/CorePacketComponents/DataReplicationPacketComponent.hpp"

class RenderComponent;
class EntityComponent;

class Entity
{
public:
	Entity();
	~Entity();

	virtual void Init() = 0;

	virtual void UpdateSmoothMovement(const float InDeltaTime)
	{
		// Lerp position
		const Tga::Vector2f newPosition = Tga::Vector2f::Lerp(position_, targetPosition_, targetPositionLerpSpeed_ * InDeltaTime);
		SetPosition(newPosition, false);
	}
	
	virtual void Update(float InDeltaTime) {}
	virtual void FixedUpdate() {}
	void UpdateComponents(float InDeltaTime);
	void FixedUpdateComponents();

	void NativeFixedUpdate();
	
	void UpdateRender();

	void SetShouldReplicatePosition(const bool InShouldReplicatePosition) { bShouldReplicatePosition_ = InShouldReplicatePosition; }
	
	void SetTargetPosition(const Tga::Vector2f& InPos) { targetPosition_ = InPos; }
	void SetPosition(const Tga::Vector2f& InPos, bool InIsTeleport = true);
	Tga::Vector2f GetPosition() const { return position_; }
	Tga::Vector2f GetTargetPosition() const { return targetPosition_; }
	
	uint16_t GetId() const { return id_; }
	uint64_t GetTypeTagHash() const { return typeTagHash_; }

	template<typename ComponentType>
	std::weak_ptr<ComponentType> AddComponent();

	template<typename ComponentType>
	std::vector<std::weak_ptr<ComponentType>> GetComponents();

	template<typename ComponentType>
	std::weak_ptr<ComponentType> GetFirstComponent();

	Tga::Vector2f GetDirection() const { return direction_; }
	void SetDirection(const Tga::Vector2f InDirection) { direction_ = InDirection.GetNormalized(); }

	// Server Sided
	virtual void OnSendReplication(DataReplicationPacketComponent& OutComponent) {}
	// Client Sided
	virtual void OnReadReplication(const DataReplicationPacketComponent& InComponent) {} 
	
protected:
	void UpdateReplication();

	DataReplicationPacketComponent replicationPacketComponent_;
	
	uint16_t id_ = 0;
	Tga::Vector2f position_;
	Tga::Vector2f direction_;

	uint64_t typeTagHash_;
	
	std::vector<std::shared_ptr<EntityComponent>> components_;
	std::shared_ptr<EntityComponent> renderComponent_;

	Tga::Vector2f targetPosition_ = {};
	float targetPositionLerpSpeed_ = 8.f;

	bool bShouldReplicatePosition_ = false;
	
	friend class EntityManager;
};

template <typename ComponentType>
std::weak_ptr<ComponentType> Entity::AddComponent()
{
	if (std::is_base_of_v<ComponentType, EntityComponent>)
	{
		return std::weak_ptr<ComponentType>();
	}

	static uint16_t componentIdIter = 0;
	const uint16_t newComponentId = ++componentIdIter;
	
	if (std::is_base_of_v<ComponentType, RenderComponent>)
	{
		std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>();
		renderComponent_ = component;
		renderComponent_->SetOwner(*this);
		component->localId_ = newComponentId;
		return std::weak_ptr<ComponentType>(component);
	}

	std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>();
	component->SetOwner(*this);
	component->localId_ = newComponentId;
	components_.push_back(component);
	
	return std::weak_ptr<ComponentType>(component);
}

template <typename ComponentType>
std::vector<std::weak_ptr<ComponentType>> Entity::GetComponents()
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
	
	for (std::shared_ptr<EntityComponent> component : components_)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(component))
		{
			result.push_back(std::weak_ptr<ComponentType>(castedComponent));
		}
	}

	return result;
}

template <typename ComponentType>
std::weak_ptr<ComponentType> Entity::GetFirstComponent()
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
	
	for (std::shared_ptr<EntityComponent> component : components_)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(component))
		{
			return std::weak_ptr<ComponentType>(castedComponent);
		}
	}

	return std::weak_ptr<ComponentType>();
}

inline Entity::Entity()
{
	
}

inline Entity::~Entity()
{
	
}

inline void Entity::UpdateComponents(const float InDeltaTime)
{
	for (std::shared_ptr<EntityComponent> component : components_)
	{
		component->Update(InDeltaTime);
	}
}

inline void Entity::FixedUpdateComponents()
{
	for (std::shared_ptr<EntityComponent> component : components_)
	{
		component->FixedUpdate();
	}
}

inline void Entity::NativeFixedUpdate()
{
	if (Net::PacketManager::Get()->IsServer())
	{
		UpdateReplication();
	}
}

inline void Entity::UpdateRender()
{
	if (renderComponent_ != nullptr)
	{
		renderComponent_->Update(0.f);
	}
}

inline void Entity::SetPosition(const Tga::Vector2f& InPos, const bool InIsTeleport)
{
	position_ = InPos;
	if (InIsTeleport)
	{
		SetTargetPosition(InPos);
	}
}

inline void Entity::UpdateReplication()
{
	// Custom replication
	replicationPacketComponent_.Reset();
	replicationPacketComponent_.identifierDataFirst = id_;
	OnSendReplication(replicationPacketComponent_);
	replicationPacketComponent_.UpdateSize();
	if (replicationPacketComponent_.GetSize() > REPLICATION_COMPONENT_SIZE_EMPTY)
	{
		Net::PacketManager::Get()->SendPacketComponentMulticast<DataReplicationPacketComponent>(replicationPacketComponent_);
	}
	
	// Replicate position if enabled
	if (bShouldReplicatePosition_)
	{
		UpdateEntityPositionComponent positionComponent;
		positionComponent.bIsTeleport = false;
		positionComponent.xPos = targetPosition_.x;
		positionComponent.yPos = targetPosition_.y;
		positionComponent.rotation = std::atan2(direction_.y, direction_.x);
		positionComponent.entityIdentifier = GetId();
		positionComponent.SetOverrideDefiningData(GetId());
		Net::PacketManager::Get()->SendPacketComponentMulticastWithLod<UpdateEntityPositionComponent>(positionComponent, { targetPosition_.x, targetPosition_.y, 0.f });
	}
}

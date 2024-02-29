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

class RenderComponent;
class EntityComponent;

class Entity
{
public:
	virtual ~Entity() = default;

	virtual void Init() = 0;

	virtual void UpdateSmoothMovement(const float InDeltaTime)
	{
		if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
		{
			return;
		}
		
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
	
	void SetTargetPosition(const Tga::Vector2f& InPos) { previousTargetPosition_ = targetPosition_; targetPosition_ = InPos; }
	void SetPosition(const Tga::Vector2f& InPos, bool InIsTeleport = true);
	Tga::Vector2f GetPosition() const { return position_; }

	Tga::Vector2f GetTargetDirection() const { return (targetPosition_ - previousTargetPosition_).GetNormalized(); }
	
	uint16_t GetId() const { return id_; }
	uint64_t GetTypeTagHash() const { return typeTagHash_; }

	template<typename ComponentType>
	ComponentType* AddComponent();

	template<typename ComponentType>
	ComponentType* GetComponent();
	
protected:
	void UpdateReplication();
	
	uint16_t id_ = 0;
	Tga::Vector2f position_;

	uint64_t typeTagHash_;
	
	std::vector<std::shared_ptr<EntityComponent>> components_;
	std::shared_ptr<EntityComponent> renderComponent_;

	Tga::Vector2f targetPosition_ = {};
	Tga::Vector2f previousTargetPosition_ = {};
	float targetPositionLerpSpeed_ = 5.f;

	bool bShouldReplicatePosition_ = false;
	
	friend class EntityManager;
};

template <typename ComponentType>
ComponentType* Entity::AddComponent()
{
	if (std::is_base_of_v<ComponentType, EntityComponent>)
	{
		return nullptr;
	}

	ComponentType* foundComponent = GetComponent<ComponentType>();
	if (foundComponent != nullptr)
	{
		return foundComponent;
	}
	
	if (std::is_base_of_v<ComponentType, RenderComponent>)
	{
		std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>();
		renderComponent_ = component;
		renderComponent_->SetOwner(*this);
		return component.get();
	}

	std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>();
	component->SetOwner(*this);
	components_.push_back(component);
	
	return component.get();
}

template <typename ComponentType>
ComponentType* Entity::GetComponent()
{
	if (std::is_base_of_v<ComponentType, EntityComponent>)
	{
		return nullptr;
	}

	if (std::is_base_of_v<ComponentType, RenderComponent>)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(renderComponent_))
		{
			return castedComponent.get();
		}
	}
	
	for (std::shared_ptr<EntityComponent> component : components_)
	{
		if (auto castedComponent = std::dynamic_pointer_cast<ComponentType>(component))
		{
			return castedComponent.get();
		}
	}

	return nullptr;
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
	UpdateReplication();
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
	if (InIsTeleport && Net::PacketManager::Get()->GetManagerType() != ENetworkHandleType::Server)
	{
		SetTargetPosition(InPos);
	}
}

inline void Entity::UpdateReplication()
{
	// Replicate position if enabled
	if (bShouldReplicatePosition_)
	{
		UpdateEntityPositionComponent positionComponent;
		positionComponent.bIsTeleport = false;
		positionComponent.xPos = position_.x;
		positionComponent.yPos = position_.y;
		positionComponent.entityIdentifier = GetId();
		Net::PacketManager::Get()->SendPacketComponentMulticastWithLod<UpdateEntityPositionComponent>(positionComponent, { position_.x, position_.y, 0.f });
	}
}

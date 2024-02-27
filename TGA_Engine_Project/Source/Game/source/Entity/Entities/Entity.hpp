#pragma once
#include "stdafx.h"

#include "../../../../SimpleNetLib/SimpleNetLib/Utility/NetTag.h"
#include "../EntityComponents/EntityComponent.hpp"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"
#include "Packet/PacketManager.h"

class RenderComponent;
class EntityComponent;

class Entity
{
public:
	virtual ~Entity() = default;

	virtual void Init() = 0;
	
	virtual void Update(float InDeltaTime) {}
	virtual void FixedUpdate(float InDeltaTime) {}
	void UpdateComponents(float InDeltaTime);
	void FixedUpdateComponents(float InDeltaTime);
	
	void UpdateRender();
	
	void SetPosition(const Tga::Vector2f& InPos, bool InIsTeleport = true);
	Tga::Vector2f GetPosition() const { return position_; }

	uint16_t GetId() const { return id_; }
	uint64_t GetTypeTagHash() const { return typeTagHash_; }

	template<typename ComponentType>
	ComponentType* AddComponent();

	template<typename ComponentType>
	ComponentType* GetComponent();
	
private:
	uint16_t id_ = 0;
	Tga::Vector2f position_;

	uint64_t typeTagHash_;
	
	std::vector<std::shared_ptr<EntityComponent>> components_;
	std::shared_ptr<EntityComponent> renderComponent_;

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

inline void Entity::FixedUpdateComponents(float InDeltaTime)
{
	for (std::shared_ptr<EntityComponent> component : components_)
	{
		component->FixedUpdate(InDeltaTime);
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
	if (InIsTeleport && Net::PacketManager::Get()->GetManagerType() != ENetworkHandleType::Server)
	{
		ControllerComponent* controllerComponent = GetComponent<ControllerComponent>();
		if (controllerComponent)
		{
			controllerComponent->targetPosition_ = InPos;
		}
	}
}

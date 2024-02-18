#pragma once
#include "stdafx.h"

#include "../../../../SimpleNetLib/SimpleNetLib/Utility/NetTag.h"
#include "../EntityComponents/EntityComponent.hpp"

class RenderComponent;
class EntityComponent;

class Entity
{
public:
	virtual ~Entity() = default;

	virtual void Init() {}
	
	virtual void Update(float InDeltaTime) = 0;
	void UpdateComponents(float InDeltaTime);

	void UpdateRender();
	
	void SetPosition(const Tga::Vector2f& InPos) { position_ = InPos; }
	Tga::Vector2f GetPosition() const { return position_; }

	uint16_t GetId() const { return id_; }

	template<typename ComponentType>
	ComponentType* AddComponent();

	template<typename ComponentType>
	ComponentType* GetComponent();
	
private:
	uint16_t id_ = 0;
	Tga::Vector2f position_;

	NetTag typeTag_;

	std::vector<std::shared_ptr<EntityComponent>> components_;
	std::shared_ptr<EntityComponent> renderComponent_;
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
		renderComponent_ = std::make_shared<ComponentType>();
		renderComponent_->SetOwner(*this);
		return renderComponent_;
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
		return renderComponent_;
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

inline void Entity::UpdateRender()
{
	if (renderComponent_ != nullptr)
	{
		renderComponent_->Update(0.f);
	}
}

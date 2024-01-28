#include "EventSystem.h"

EventSystem* EventSystem::instance_ = nullptr;

EventSystem* EventSystem::Initialize()
{
    if (instance_ == nullptr)
    {
        instance_ = new EventSystem();
    }
    
    return instance_;
}

EventSystem* EventSystem::Get()
{
    return instance_;
}

void EventSystem::End()
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

EventSystem::EventSystem() = default;

EventSystem::~EventSystem() = default;

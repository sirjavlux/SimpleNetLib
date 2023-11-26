#pragma once

template <typename OwningObjectType, typename... Args>
class DynamicDelegate
{
public:
    DynamicDelegate(const OwningObjectType& InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        Bind(InOwnerObject, InFunction);
    }
    
    DynamicDelegate() = default;
    
    void Execute(Args... InArgs)
    {
        if (ownerObject_.get() != nullptr)
        {
            function_(ownerObject_.get(), InArgs);
        }
    }
    
    void Bind(const OwningObjectType& InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        function_ = InFunction;
        ownerObject_ = std::weak_ptr<OwningObjectType>(&InOwnerObject);
    }
    
private:
    std::weak_ptr<OwningObjectType> ownerObject_;
    
    std::function<void(OwningObjectType*, Args...)> function_;
};

template<typename... Args>
class StaticDelegate
{
public:
    StaticDelegate(const std::function<void(Args...)>& InFunction)
    {
        Bind(InFunction);
    }
    
    StaticDelegate() = default;
    
    void Execute(Args... InArgs)
    {
        function_(InArgs);
    }
    
    void Bind(const std::function<void(Args...)>& InFunction)
    {
        function_ = InFunction;
    }
    
private:
    std::function<void(Args...)> function_;
};
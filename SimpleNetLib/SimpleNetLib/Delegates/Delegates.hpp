#pragma once

template <typename... Args>
class DynamicMulticastDelegate
{
public:
    DynamicMulticastDelegate() = default;

    template<typename OwningObjectType>
    void AddDynamic(const std::shared_ptr<OwningObjectType>& InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        delegates_.emplace_back(InOwnerObject, [InFunction](void* obj, Args... args) {
            InFunction(static_cast<OwningObjectType*>(obj), std::forward<Args>(args)...);
        });
    }

    template<typename OwningObjectType>
    void RemoveDynamic(const OwningObjectType* InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        auto compFunc = [InFunction](const DelegateEntry& entry) -> bool {
            auto storedFunc = entry.function.template target<void(OwningObjectType*, Args...)>();
            auto inputFuncTarget = InFunction.template target<void(OwningObjectType*, Args...)>();
            return storedFunc && inputFuncTarget && *storedFunc == *inputFuncTarget;
        };

        auto it = std::remove_if(delegates_.begin(), delegates_.end(),
            [=](const DelegateEntry& entry) {
                auto owner = entry.ownerObject.lock();
                return owner.get() == InOwnerObject && compFunc(entry);
            });

        delegates_.erase(it, delegates_.end());
    }

    void Execute(Args... InArgs)
    {
        std::vector<DelegateEntry> toRemove;

        for (auto& entry : delegates_)
        {
            if (auto owner = entry.ownerObject.lock())
            {
                entry.function(owner.get(), InArgs...);
            }
            else
            {
                toRemove.push_back(entry);
            }
        }
        
        for (const auto& entryToRemove : toRemove)
        {
            RemoveDynamic(entryToRemove.ownerObject.lock().get(), entryToRemove.function);
        }
    }
    
private:
    struct DelegateEntry
    {
        std::weak_ptr<void> ownerObject;
        std::function<void(void*, Args...)> function;

        DelegateEntry(const std::shared_ptr<void>& owner, const std::function<void(void*, Args...)>& func)
            : ownerObject(owner), function(func) {}

        bool operator==(const DelegateEntry& InComp)
        {
            return ownerObject.lock().get() == InComp.ownerObject.lock().get()
                && function == InComp.function;
        }
    };
    
    std::vector<DelegateEntry> delegates_;
};

template <typename... Args>
class DynamicDelegate
{
public:
    template<typename OwningObjectType>
    DynamicDelegate(const std::shared_ptr<OwningObjectType>& InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        Bind(InOwnerObject, InFunction);
    }
    
    DynamicDelegate() = default;
    
    void Execute(Args... InArgs)
    {
        if (ownerObject_.lock() != nullptr)
        {
            function_(ownerObject_.lock().get(), InArgs);
        }
    }

    template<typename OwningObjectType>
    void Bind(const std::shared_ptr<OwningObjectType>& InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        function_ = [InFunction](void* obj, Args... args) {
            InFunction(static_cast<OwningObjectType*>(obj), std::forward<Args>(args)...);
        };
        
        ownerObject_ = InOwnerObject;
    }
    
private:
    std::weak_ptr<void> ownerObject_;
    
    std::function<void(void*, Args...)> function_;
};

template<typename... Args>
class StaticMulticastDelegate
{
public:
    StaticMulticastDelegate() = default;
    
    void Execute(Args... InArgs)
    {
        for (std::function<void(Args...)>& function : functions_)
        {
            function(InArgs);
        }
    }
    
    void Bind(const std::function<void(Args...)>& InFunction)
    {
        functions_.push_back(InFunction);
    }

    void UnBind(const std::function<void(Args...)>& InFunction)
    {
        functions_.erase(
            std::remove(functions_.begin(), functions_.end(), InFunction),
            functions_.end());
    }
    
private:
    std::vector<std::function<void(Args...)>> functions_;
};

template<typename... Args>
class StaticDelegate
{
public:
    explicit StaticDelegate(const std::function<void(Args...)>& InFunction)
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
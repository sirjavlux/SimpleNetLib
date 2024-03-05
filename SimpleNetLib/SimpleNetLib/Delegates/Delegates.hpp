#pragma once

template <typename... Args>
class DynamicMulticastDelegate
{
public:
    DynamicMulticastDelegate() = default;

    template<typename OwningObjectType>
    void AddDynamic(OwningObjectType* InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        if (InOwnerObject == nullptr)
        {
            return;
        }
        
        delegates_.emplace_back(InOwnerObject, [InFunction](void* InObj, Args... InArgs) {
            InFunction(static_cast<OwningObjectType*>(InObj), std::forward<Args>(InArgs)...);
        });
    }

    template<typename OwningObjectType>
    void RemoveDynamic(const OwningObjectType* InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        auto compFunc = [&InFunction](const DelegateEntry& Entry) -> bool {
            auto storedFunc = Entry.function.template target<void(OwningObjectType*, Args...)>();
            auto inputFuncTarget = InFunction.template target<void(OwningObjectType*, Args...)>();
            return storedFunc == inputFuncTarget;
        }; 
        
        auto it = std::remove_if(delegates_.begin(), delegates_.end(),
            [=](const DelegateEntry& Entry) {
                auto owner = Entry.ownerObject;
                return owner == InOwnerObject && compFunc(Entry);
            });

        delegates_.erase(it, delegates_.end());
    }

    void Execute(Args... InArgs)
    {
        for (auto& entry : delegates_)
        {
            auto owner = entry.ownerObject;
            entry.function(owner, InArgs...);
        }
    }
    
private:
    struct DelegateEntry
    {
        void* ownerObject;
        std::function<void(void*, Args...)> function;

        DelegateEntry(void* Owner, const std::function<void(void*, Args...)>& Func)
            : ownerObject(Owner), function(Func) {}

        bool operator==(const DelegateEntry& InComp)
        {
            return ownerObject == InComp.ownerObject
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
            function_(ownerObject_.lock().get(), InArgs...);
        }
    }

    template<typename OwningObjectType>
    void Bind(const std::shared_ptr<OwningObjectType>& InOwnerObject, const std::function<void(OwningObjectType*, Args...)>& InFunction)
    {
        function_ = [InFunction](void* Obj, Args... InArgs) {
            InFunction(static_cast<OwningObjectType*>(Obj), std::forward<Args>(InArgs)...);
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
            function(InArgs...);
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
        function_(InArgs...);
    }
    
    void Bind(const std::function<void(Args...)>& InFunction)
    {
        function_ = InFunction;
    }
    
private:
    std::function<void(Args...)> function_;
};
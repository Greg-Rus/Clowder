#pragma once
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include "../Logger/Logger.h"
#include <deque>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
    static int nextId;
};

template <typename T>
class Component : public IComponent
{
public:
    static int GetId()
    {
        static auto id = nextId++;
        return id;
    }
};

class Entity
{
private:
    int id;

public:
    Entity(int id) : id(id) {};
    int GetId() const;
    void Kill();
    void Tag(const std::string& tag);
    bool HasTag(const std::string& tag) const;
    void Group(const std::string& gropu);
    bool BelongsToGroup(const std::string& group) const;

    bool operator==(const Entity &other) const
    {
        return id == other.id;
    }

    bool operator!=(const Entity &other) const
    {
        return id != other.id;
    }

    Entity &operator=(const Entity &other) = default;

    bool operator<(const Entity &other) const
    {
        return id < other.id;
    }

    bool operator>(const Entity &other) const
    {
        return id > other.id;
    }

    template <typename TComponent, typename... TArgs>
    void AddComponent(TArgs &&...args);
    template <typename T>
    void RemoveComponent();
    template <typename T>
    bool HasComponent() const;
    template <typename T>
    T &GetComponent();

    class Registry *registry;
};

class System
{
private:
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;
    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature &GetComponentSignature() const;

    template <typename TComponent>
    void RequireComponent();
};

class IPool
{
public:
    virtual ~IPool() = default;
    virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool : public IPool
{
private:
    std::vector<T> data;
    int size;

    std::unordered_map<int, int> entityIdToIndex;
    std::unordered_map<int, int> indexToEntityId;

public:
    Pool(int capacity = 100)
    {
        size = 0;
        data.resize(capacity);
    }
    virtual ~Pool() = default;

    bool IsEmpty() const
    {
        return size == 0;
    }

    int GetSize() const
    {
        return size;
    }

    void Resize(int n)
    {
        data.resize(n);
    }

    void Clear()
    {
        data.clear();
        size = 0;
    }

    void Add(T object)
    {
        data.push_back(object);
    }

    void Set(int entityId, T object)
    {
        if(entityIdToIndex.find(entityId) != entityIdToIndex.end())
        {
            int index = entityIdToIndex[entityId];
            data[index] = object;
        }
        else
        {
            int index = size;
            entityIdToIndex.emplace(entityId, index);
            indexToEntityId.emplace(index, entityId);
            if(index >= static_cast<int>(data.capacity()))
            {
                data.resize(size * 2);
            }
            data[index] = object;
            size++;
        }
        
    }

    void Remove(int entityId)
    {
        int indexOfRemoved = entityIdToIndex[entityId];
        int indexOfLast = size - 1;
        data[indexOfRemoved] = data[indexOfLast];
        
        int entityIdOfLastElement = indexToEntityId[indexOfLast];
        entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
        indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

        entityIdToIndex.erase(entityId);
        indexToEntityId.erase(indexOfLast);

        size--;
    }

    void RemoveEntityFromPool(int entityId) override
    {
        if(entityIdToIndex.find(entityId) != entityIdToIndex.end())
        {
            Remove(entityId);
        }
    }

    T &Get(int entityId)
    {
        int index = entityIdToIndex[entityId];
        return static_cast<T &>(data[index]);
    }

    T &operator[](unsigned int index)
    {
        return data[index];
    }
};

class Registry
{
private:
    int numEntities = 0;
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;
    // Indexed by component ID. Contains vectors of component values indexed by entity IDs.
    std::vector<std::shared_ptr<IPool>> componentPools;
    // Indexed by entity ID. Maps bitsets that represent components active on an entity.
    std::vector<Signature> entityComponentSignatures;
    // Templated system typ to system pointer
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
    std::deque<int> freeIds;

    std::unordered_map<std::string, Entity> entityPerTag;
    std::unordered_map<int, std::string> tagPerEntity;
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<int, std::string> groupPerEntity;

public:
    Registry() = default;
    void Update();
    Entity CreateEntity();
    void KillEntity(Entity entity);

    void AddEntityToSystem(Entity enityt);

    template <typename TComponent, typename... TArgs>
    void AddComponent(Entity entity, TArgs &&...args);
    template <typename T>
    void RemoveComponent(Entity entity);
    template <typename T>
    bool HasComponent(Entity entity) const;
    template <typename T>
    T &GetComponent(Entity entity);

    template <typename TSystem, typename... TArgs>
    void AddSystem(TArgs &&...args);
    template <typename TSystem>
    void RemoveSystem();
    template <typename TSystem>
    bool HasSystem() const;
    template <typename TSystem>
    TSystem &GetSystem() const;

    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);

    void TagEntity(Entity entity, const std::string& tag);
    bool EntityHasTag(Entity entity, const std::string& tag) const;
    Entity GetEntityByTag(const std::string& tag) const;
    void RemoveEntityTag(Entity entity);
    void GroupEntity(Entity entity, const std::string& group);
    bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
    std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
    void RemoveEntityGroup(Entity entity);
};

template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return system != systems.end();
}

template <typename TSystem>
TSystem &Registry::GetSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // In case we are adding a completely new, unknown component
    if (componentId >= static_cast<int>(componentPools.size()))
    {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);

    // Logger::Log("Component id = " + std::to_string(componentId) +
    //             " was added to entity id = " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);

    entityComponentSignatures[entityId].set(componentId, false);
    //Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id = " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent &Registry::GetComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    std::shared_ptr<Pool<TComponent>> pool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return pool->Get(entityId);
}

template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs &&...args)
{
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent &Entity::GetComponent()
{
    return registry->GetComponent<TComponent>(*this);
}
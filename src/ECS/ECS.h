#pragma once
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

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

    bool operator == (const Entity &other) const
    {
        return id == other.id;
    }

    bool operator != (const Entity &other) const
    {
        return id != other.id;
    }

    Entity& operator = (const Entity &other) = default;
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

class IPool{
    public:
    virtual ~IPool(){}
};

template <typename T>
class Pool: public IPool
{
    private:
    std::vector<T> data;

    public:
    Pool(int size = 100)
    {
        data.resize(size);
    }
    virtual ~Pool() = default;

    bool IsEmpty() const{
        return data.empty();
    }

    int GetSize() const{
        return data.size();
    }

    void Resize(int size){
        data.resize(size);
    }

    void Clear()
    {
        data.clear();
    }

    void Add(T object)
    {
        data.push_back(object);
    }

    void Set(int index, T object)
    {
        data[index] = object;
    }

    T& Get(int index)
    {
        return static_cast<T&>(data[index]);
    }

    T& operator[](unsigned int index)
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
        //Indexed by component ID. Contains vectors of component values indexed by entity IDs.
        std::vector<IPool*> componentPools;
        //Indexed by entity ID. Maps bitsets that represent components active on an entity.
        std::vector<Signature> entityComponentSignatures;
        std::unordered_map<std::type_index, System*> systems;
    public:
        Registry() = default;
        void Update();
        Entity CreateEntity();
        void KillEntity(Entity entity);

        void AddEntityToSystem(Entity enityt);
        void AddSystem();
 
        template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename T> void RemoveComponent(Entity entity);
        template <typename T>bool HasComponent(Entity entity) const;
        template <typename T> T& GetComponent(Entity entity) const;

        void AddSystem();
        void RemoveSystem();
        void HasSystem();
        void GetSystem();
};

template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename T, typename ...TArgs> void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    if(componentId >= componentPools.size())
    {
        componentPools.resize(componentId + 1,nullptr);
    }
}
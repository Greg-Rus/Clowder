#include "ECS.h"
#include <algorithm>
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const{
    return id;
}

void System::AddEntityToSystem(Entity entity){
    // auto it = std::find_if(entities.begin(), entities.end(), [&entity](Entity member) { return entity.GetId() == member.GetId();});
    // if(it == entities.end())
    // {
    //     return;
    // }
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity){
    entities.erase(
    std::remove_if(entities.begin(), 
        entities.end(), 
        [&entity](Entity member){return entity == member;}), 
    entities.end());
}
std::vector<Entity> System::GetSystemEntities() const{
    return entities;
}

const Signature& System::GetComponentSignature() const{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    int entityId = numEntities++;
    int signaturesSize = static_cast<int>(entityComponentSignatures.size());
    if(entityId >= signaturesSize)
    {
        entityComponentSignatures.resize(entityId + 1);
    }
    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    Logger::Log("Created Entity with ID: " + std::to_string(entityId));
    return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
    const auto entityId = entity.GetId();
    const auto& entityComponentSignature = entityComponentSignatures[entityId];
    for(auto& system: systems)
    {
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if(isInterested)
        {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::Update()
{
    for(auto entity: entitiesToBeAdded)
    {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();
}

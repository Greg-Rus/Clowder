#include "ECS.h"
#include <algorithm>
#include "../Logger/Logger.h"

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
    if(entityId >= entityComponentSignatures.size())
    {
        entityComponentSignatures.resize(entityId + 1);
    }
    Entity entity(entityId);
    entitiesToBeAdded.insert(entity);

    Logger::Log("Created Entity with ID: " + std::to_string(entityId));
    return entity;
}

void Registry::Update()
{
    
}

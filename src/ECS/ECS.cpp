#include "ECS.h"
#include <algorithm>
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void Entity::Kill()
{
    registry->KillEntity(*this);
}

void System::AddEntityToSystem(Entity entity)
{
    // auto it = std::find_if(entities.begin(), entities.end(), [&entity](Entity member) { return entity.GetId() == member.GetId();});
    // if(it == entities.end())
    // {
    //     return;
    // }
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
    entities.erase(
        std::remove_if(entities.begin(),
                       entities.end(),
                       [&entity](Entity member)
                       { return entity == member; }),
        entities.end());
}
std::vector<Entity> System::GetSystemEntities() const
{
    return entities;
}

const Signature &System::GetComponentSignature() const
{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    int entityId;

    if (freeIds.empty())
    {
        // No free IDs need to expand max entity count
        entityId = numEntities++;
        int signaturesSize = static_cast<int>(entityComponentSignatures.size());
        if (entityId >= signaturesSize)
        {
            entityComponentSignatures.resize(entityId + 1);
        }
    }
    else
    {
        // Reuse previous ID.
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    Logger::Log("Created Entity with ID: " + std::to_string(entityId));
    return entity;
}

void Registry::KillEntity(Entity entity)
{
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity)
{
    const auto entityId = entity.GetId();
    const auto &entityComponentSignature = entityComponentSignatures[entityId];
    for (auto &system : systems)
    {
        const auto &systemComponentSignature = system.second->GetComponentSignature();
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (isInterested)
        {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
    for (auto system : systems)
    {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::Update()
{
    for (auto entity : entitiesToBeAdded)
    {
        AddEntityToSystems(entity);
    }

    entitiesToBeAdded.clear();

    for (auto entity : entitiesToBeKilled)
    {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();
        freeIds.push_back(entity.GetId());
        Logger::Log("Killed entity: " + std::to_string(entity.GetId()));
    }
    entitiesToBeKilled.clear();
}

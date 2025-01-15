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

void Entity::Tag(const std::string &tag)
{
    registry->TagEntity(*this, tag);
}
bool Entity::HasTag(const std::string &tag) const
{
    return registry->EntityHasTag(*this, tag);
}
void Entity::Group(const std::string &group)
{
    registry->GroupEntity(*this, group);
}
bool Entity::BelongsToGroup(const std::string &group) const
{
    return registry->EntityBelongsToGroup(*this, group);
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
        for (auto pool : componentPools)
        {
            if (pool)
            {
                pool->RemoveEntityFromPool(entity.GetId());
            }
        }
        freeIds.push_back(entity.GetId());
        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
        // Logger::Log("Killed entity: " + std::to_string(entity.GetId()));
    }
    entitiesToBeKilled.clear();
}

void Registry::TagEntity(Entity entity, const std::string &tag)
{
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}
bool Registry::EntityHasTag(Entity entity, const std::string &tag) const
{
    if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end())
    {
        return false;
    }
    return tagPerEntity.at(entity.GetId()) == tag;
}
Entity Registry::GetEntityByTag(const std::string &tag) const
{
    return entityPerTag.at(tag);
}
void Registry::RemoveEntityTag(Entity entity)
{
    auto taggedEntity = tagPerEntity.find(entity.GetId());
    if (taggedEntity != tagPerEntity.end())
    {
        auto tag = taggedEntity->second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}
void Registry::GroupEntity(Entity entity, const std::string &group)
{
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.GetId(), group);
}
bool Registry::EntityBelongsToGroup(Entity entity, const std::string &group) const
{
    if (groupPerEntity.find(entity.GetId()) == groupPerEntity.end())
    {
        return false;
    }
    return groupPerEntity.at(entity.GetId()) == group;
}
std::vector<Entity> Registry::GetEntitiesByGroup(const std::string &group) const
{
    auto &setOfEntities = entitiesPerGroup.at(group);
    return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}
void Registry::RemoveEntityGroup(Entity entity)
{
    auto groupedEntity = groupPerEntity.find(entity.GetId());
    if (groupedEntity != groupPerEntity.end())
    {
        auto group = entitiesPerGroup.find(groupedEntity->second);
        if (group != entitiesPerGroup.end())
        {
            auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end())
            {
                group->second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}

#include "ECS.h"
#include <algorithm>


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
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity member){return entity.GetId() == member.GetId();}), entities.end());
}
std::vector<Entity> System::GetSystemEntities() const{
    return entities;
}

const Signature& System::GetComponentSignature() const{
    return componentSignature;
}

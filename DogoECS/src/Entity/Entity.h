#pragma once
#include "DG_Component/DG_Component.h"
static DG_ComponentManager S_ComponentManager;
class Entity
{
public:
    Entity() { m_EntityID = UUID(); }

    template<typename TYPE>
    DG_Component* AddComponent()
    {
        return S_ComponentManager.AddComponent<TYPE>(m_EntityID.GetUUID_ui64());
    }

    // template<typename TYPE>
    // void RemoveComponent()
    // {
    //     DG_ComponentManager::RemoveComponent<TYPE>(m_EntityID.GetUUID_ui64());
    // }

    uint64_t GetID_ui64() const { return m_EntityID.GetUUID_ui64(); }

private:
    UUID m_EntityID;
};

class EntityManager
{
public:
    EntityManager()
    {
        for (int i = 0; i < MAX_ENTITIES; i++)
        {
            Entity e;
            m_AvailableEntities.push(e);
        }
    }

    std::optional<Entity> CreateEntity()
    {
        if (m_LivingEntityCount >= MAX_ENTITIES)
        {
            return std::nullopt;
        }
        Entity id = m_AvailableEntities.front();
        m_AvailableEntities.pop();
        ++m_LivingEntityCount;
        return id;
    }

    void DestroyEntity(Entity entity)
    {
        m_AvailableEntities.push(entity);
        m_LivingEntityCount--;
    }


private:

    uint32_t MAX_ENTITIES = 5000;

    std::queue<Entity> m_AvailableEntities{};

    uint32_t m_LivingEntityCount{ 0 };


};
#pragma once
#include "DG_Component/DG_Component.h"

class Entity
{
public:
    Entity() { m_EntityID = UUID(); }

    ~Entity(){}

    template<typename TYPE>
    TYPE* AddComponent()
    {
        return DG_ComponentManager::AddComponent<TYPE>(m_EntityID.GetUUID_ui64());
    }

     template<typename TYPE>
     void RemoveComponent(uint64_t componentID)
     {
         DG_ComponentManager::RemoveComponent<TYPE>(componentID);
     }

    uint64_t GetID_ui64() const { return m_EntityID.GetUUID_ui64(); }

private:
    UUID m_EntityID;
};

class DG_EntityManager
{
public:
    DG_EntityManager(const DG_EntityManager&) = delete;
    DG_EntityManager& operator=(const DG_EntityManager&) = delete;
    ~DG_EntityManager() {}

    
    static Entity* CreateEntity()
    {
        if (m_LivingEntityCount >= MAX_ENTITIES)
        {
            return nullptr;
        }
        Entity* id = &m_AvailableEntities.front();
        m_AvailableEntities.pop();
        ++m_LivingEntityCount;
        return id;
    }

    static void DestroyEntity(Entity entity)
    {
        m_AvailableEntities.push(entity);
        m_LivingEntityCount--;
    }

protected:
    DG_EntityManager()
    {
        for (int i = 0; i < MAX_ENTITIES; i++)
        {
            Entity e;
            m_AvailableEntities.push(e);
        }
    }


private:    
    static DG_EntityManager* s_Instance;
    static uint32_t MAX_ENTITIES ;

    static std::queue<Entity> m_AvailableEntities;

    static uint32_t m_LivingEntityCount;


};
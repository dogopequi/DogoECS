#pragma once
#include <vector>
#include <cstdint>

namespace DogoECS
{
   class Entity
   {
   public:
       Entity(uint64_t id) : m_EntityID(id) {}
       uint64_t GetID() const { return m_EntityID; }

   private:
       uint64_t m_EntityID;
   };

   class DG_EntityManager
    {
    public:
        DG_EntityManager(uint64_t maxEntities)
            : MAX_ENTITIES(maxEntities)
        {
            m_Entities.reserve(MAX_ENTITIES);
        }

        ~DG_EntityManager() = default;

        Entity* CreateEntity()
        {
            if (m_LivingEntityCount >= MAX_ENTITIES)
                return nullptr;

            m_Entities.emplace_back(m_LivingEntityCount);
            m_LivingEntityCount++;
            return &m_Entities.back();
        }

        void DestroyEntity(Entity* entity)
        {
            if (!entity) return;

            auto it = std::find_if(m_Entities.begin(), m_Entities.end(),
                [entity](const Entity& e) { return &e == entity; });
            if (it != m_Entities.end())
            {
                std::swap(*it, m_Entities.back());
                m_Entities.pop_back();
                m_LivingEntityCount--;
            }
        }

        uint64_t GetLivingEntityCount() const { return m_LivingEntityCount; }

        std::vector<Entity>& GetAllEntities() { return m_Entities; }

    private:
        uint64_t MAX_ENTITIES;
        uint64_t m_LivingEntityCount = 0;
        std::vector<Entity> m_Entities;
    };
}

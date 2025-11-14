#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <algorithm>
#include "UUID.h"
namespace DogoECS
{
    struct DG_Component
    {
    public:
        DG_Component() : m_ComponentID(++ID_COUNTER){  }
        DG_Component(uint64_t id) : m_EntityID(id), m_ComponentID(++ID_COUNTER) {}

        uint64_t GetEntityID_ui64() const { return m_EntityID; }
        uint64_t GetComponentID_ui64() const { return m_ComponentID; }

        void SetEntityID(uint64_t e) { m_EntityID = e; }

    protected:
        uint64_t m_EntityID;
        uint64_t m_ComponentID;
        inline static std::atomic<uint64_t> ID_COUNTER{ 0 };
    };

    template<typename ComponentType>
    class ComponentTracker {
    public:
        ComponentTracker(size_t maxComponents, size_t maxEntities)
            : m_Components(maxComponents), m_Active(maxComponents, 0)
        {
            activeCount = 0;
            m_EntityIndices.resize(maxEntities);
            for (auto& vec : m_EntityIndices)
                vec.reserve(8);
        }

        ComponentType* AddComponent(size_t entityID)
        {
            if (activeCount >= m_Components.size())
                throw std::runtime_error("No available component slots.");

            ComponentType& comp = m_Components[activeCount];
            comp = ComponentType();
            comp.SetEntityID(entityID);
            m_Active[activeCount] = 1;

            m_EntityIndices[entityID].push_back(activeCount);
            activeCount++;

            return &comp;
        }

        void RemoveComponents(size_t entityID)
        {
            for (size_t idx : m_EntityIndices[entityID])
                m_Active[idx] = 0;

            m_EntityIndices[entityID].clear();
        }

        struct ActiveIterator {
            ComponentType* ptr;
            uint8_t* active;
            size_t index;
            size_t maxIndex;

            ActiveIterator(ComponentType* p, uint8_t* a, size_t i, size_t m)
                : ptr(p), active(a), index(i), maxIndex(m)
            {
                advanceToNextActive();
            }

            ComponentType& operator*() { return ptr[index]; }
            ComponentType* operator->() { return &ptr[index]; }

            ActiveIterator& operator++() {
                index++;
                advanceToNextActive();
                return *this;
            }

            bool operator!=(const ActiveIterator& other) const { return index != other.index; }

        private:
            void advanceToNextActive() {
                while (index < maxIndex && !active[index]) index++;
            }
        };

        ActiveIterator begin() { return ActiveIterator(m_Components.data(), m_Active.data(), 0, m_Components.size()); }
        ActiveIterator end() { return ActiveIterator(m_Components.data(), m_Active.data(), m_Components.size(), m_Components.size()); }

    private:
        std::vector<ComponentType> m_Components;
        std::vector<uint8_t> m_Active;
        std::vector<std::vector<size_t>> m_EntityIndices;
        size_t activeCount = 0;
    };





    class DG_ComponentManager {
    public:
        DG_ComponentManager(uint64_t maxcomponents, uint64_t maxentities) : m_MaxComponents(maxcomponents), m_MaxEntities(maxentities) {}

        template<typename ComponentType>
        void RegisterComponent()
        {
            std::type_index typeIndex(typeid(ComponentType));
            m_Trackers[typeIndex] = std::make_shared<ComponentTracker<ComponentType>>(m_MaxComponents, m_MaxEntities);
        }

        template<typename ComponentType>
        ComponentType* AddComponent(uint64_t entityID)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return nullptr;
            return tracker->AddComponent(entityID);
        }

        template<typename ComponentType>
        std::vector<ComponentType*> GetComponents(uint64_t entityID)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return {};
            return tracker->GetComponents(entityID);
        }

        template<typename ComponentType>
        std::vector<ComponentType>& GetAllActivem_Components()
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) throw std::runtime_error("Component not registered");
            return tracker->GetAllActiveComponents();
        }
        template<typename ComponentType>
        std::shared_ptr<ComponentTracker<ComponentType>> GetTracker()
        {
            auto it = m_Trackers.find(typeid(ComponentType));
            if (it != m_Trackers.end())
                return std::static_pointer_cast<ComponentTracker<ComponentType>>(it->second);
            return nullptr;
        }

    private:
        uint64_t m_MaxComponents;
        uint64_t m_MaxEntities;
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_Trackers;
    };

}

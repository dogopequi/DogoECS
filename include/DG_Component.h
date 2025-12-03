#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <algorithm>
#include <functional>
#include <Entity.h>

namespace DogoECS
{
    struct DG_Component
    {
    public:
        DG_Component() : m_ComponentID(++ID_COUNTER){  }
        DG_Component(uint64_t id) : m_EntityID(id), m_ComponentID(++ID_COUNTER) {}

        uint64_t GetEntityID() const { return m_EntityID; }
        uint64_t GetComponentID() const { return m_ComponentID; }

        void SetEntityID(uint64_t e) { m_EntityID = e; }

        void SetIndex(size_t i) { m_Index = i; }
        size_t GetIndex() const { return m_Index; }

    protected:
        uint64_t m_EntityID;
        uint64_t m_ComponentID;
        size_t   m_Index;
        inline static std::atomic<uint64_t> ID_COUNTER{ 0 };
    };

    template<typename ComponentType>
    class ComponentTracker {
    public:
        ComponentTracker(size_t maxComponents, size_t maxEntities)
            : m_Components(maxComponents), m_EntityToComponent(maxEntities), m_Active(maxComponents)
        {
            memset(m_EntityToComponent.data(), -1, maxEntities * sizeof(int64_t));
            m_ActiveCount = 0;
        }

        std::vector<ComponentType*> GetAllActiveComponents()
        {
            std::vector<ComponentType*> activeComponents;
            activeComponents.reserve(m_ActiveCount);

            for (size_t i = 0; i < m_Components.size(); ++i)
            {
                if (m_Active[i])
                    activeComponents.push_back(&m_Components[i]);
            }

            return activeComponents;
        }


        template<typename... Args>
        ComponentType* AddComponent(uint32_t entityID, Args&&... args)
        {
            if (m_ActiveCount >= m_Components.size())
            {
                std::cout << "No available component slots. Skipping." << std::endl;
                return nullptr;
            }
            int existing = m_EntityToComponent[entityID];
            if (existing != -1)
                return &m_Components[existing];
            size_t idx = m_ActiveCount++;
            ComponentType& comp = *new (&m_Components[idx]) ComponentType(std::forward<Args>(args)...);
            comp.SetEntityID(entityID);
            comp.SetIndex(idx);
            m_Active[idx] = 1;
            m_EntityToComponent[entityID] = idx;

            return &comp;
        }

        bool RemoveComponent(uint64_t entityID)
        {
            if (entityID > m_EntityToComponent.size())
                return false;
            ComponentType* comp = &m_Components[m_EntityToComponent[entityID]];
            m_Active[comp->GetIndex()] = 0;
            m_ActiveCount--;
            m_Components[m_EntityToComponent[entityID]].~ComponentType();
            new (&m_Components[m_EntityToComponent[entityID]]) ComponentType{};
            return true;
        }

        ComponentType* GetComponent(uint64_t entityID)
        {
            size_t idx = m_EntityToComponent[entityID];
            if (idx == -1 || !m_Active[idx])
                return nullptr;
            return &m_Components[idx];
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

        size_t Size() const { return m_Components.size(); }
        bool IsActive(size_t idx) const { return m_Active[idx] != 0; }
        ComponentType& GetComponentAt(size_t idx) { return m_Components[idx]; }
        ActiveIterator Begin() { return ActiveIterator(m_Components.data(), m_Active.data(), 0, m_Components.size()); }
        ActiveIterator End() { return ActiveIterator(m_Components.data(), m_Active.data(), m_Components.size(), m_Components.size()); }

    private:
        std::vector<int64_t> m_EntityToComponent;
        std::vector<ComponentType> m_Components;
        std::vector<uint8_t> m_Active;
        size_t m_ActiveCount = 0;
    };

    class DG_ComponentManager {
        struct RemoveEntry {
            void(*func)(void*, uint64_t);
            void* tracker;
        };
    public:
        DG_ComponentManager(uint64_t maxcomponents, uint64_t maxentities) : m_MaxComponents(maxcomponents), m_MaxEntities(maxentities), m_RemoveEntries(maxentities) 
        {
            for (auto& vec : m_RemoveEntries)
                vec.reserve(8);
        }
        template<typename ComponentType>
        static size_t TypeID() {
            static size_t id = s_TypeCounter++;
            return id;
        }
        template<typename ComponentType>
        void RegisterComponent()
        {
            std::type_index typeIndex(typeid(ComponentType));
            auto tracker = std::make_shared<ComponentTracker<ComponentType>>(m_MaxComponents, m_MaxEntities);
            size_t id = TypeID<ComponentType>();
            if (id >= m_Trackers.size())
                m_Trackers.resize(id + 1);
            m_Trackers[id] = tracker;

        }

        template<typename ComponentType>
        ComponentType* AddComponent(uint64_t entityID)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return nullptr;
            RemoveEntry re;
            re.func = [](void* tracker, uint64_t id) {
                static_cast<ComponentTracker<ComponentType>*>(tracker)->RemoveComponent(id);
                };
            re.tracker = tracker.get();
            m_RemoveEntries[entityID].push_back(re);
            return tracker->AddComponent(entityID);
        }
        template<typename ComponentType>
        ComponentType* AddComponent(Entity* entity)
        {
            return AddComponent<ComponentType>(entity->GetID());
        }

        template<typename ComponentType, typename... Args>
        ComponentType* AddComponent(uint64_t entityID, Args&&... args)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return nullptr;
            /*m_EntityToComponentTrackers[entityID].push_back(tracker);*/
            return tracker->AddComponent(entityID, std::forward<Args>(args)...);
        }
        template<typename ComponentType, typename... Args>
        ComponentType* AddComponent(Entity* entity, Args&&... args)
        {
            return AddComponent<ComponentType>(entity->GetID(), std::forward<Args>(args)...);
        }

        template<typename ComponentType>
        auto AllActiveBegin()
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) throw std::runtime_error("Component not registered");
            return tracker->Begin();
        }

        template<typename ComponentType>
        auto AllActiveEnd()
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) throw std::runtime_error("Component not registered");
            return tracker->End();
        }

        template<typename ComponentType>
        auto ComponentsBegin(Entity* entity)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return tracker->End();
            return tracker->Begin();
        }

        template<typename ComponentType>
        auto ComponentsEnd(Entity* entity)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return tracker->End();
            return tracker->End();
        }

        template<typename ComponentType>
        std::shared_ptr<ComponentTracker<ComponentType>> GetTracker()
        {
            size_t id = TypeID<ComponentType>();
            return std::static_pointer_cast<ComponentTracker<ComponentType>>(m_Trackers[id]);
        }
        template<typename ComponentType>
        bool RemoveComponent(ComponentType* component)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return false;
            auto& v = m_RemoveEntries[component->GetEntityID()];
            if (v == nullptr)
                return false;
            for (auto it = v.begin(); it != v.end();)
            {
                if (*it == tracker)
                    it = v.erase(it);
                else
                    ++it;
            }
            return tracker->RemoveComponent(component->GetEntityID());
        }
        bool RemoveComponents(uint64_t entityID)
        {
            if (entityID >= m_MaxEntities)
                return false;
            auto& funcs = m_RemoveEntries[entityID];
            if (funcs.empty())
                return false;
            for (auto& f : funcs)
                f.func(f.tracker, entityID);
            funcs.clear();
            return true;
        }
        bool RemoveComponents(Entity* entity)
        {
            return RemoveComponents(entity->GetID());
        }
        template<typename ComponentType>
        ComponentType* GetComponent(uint64_t entityID)
        {
            auto tracker = GetTracker<ComponentType>();
            if (!tracker) return nullptr;
            return tracker->GetComponent(entityID);
        }
        template<typename ComponentType>
        ComponentType* GetComponent(Entity* entity)
        {
            return GetComponent<ComponentType>(entity->GetID());
        }

        template<typename... Components>
        class MultiComponentIterator {
        public:
            MultiComponentIterator(size_t current, size_t max, ComponentTracker<Components>*... trackers)
                : index(current), maxIndex(max), trackers(std::make_tuple(trackers...))
            {
                advanceToNextValid();
            }

            MultiComponentIterator& operator++() {
                ++index;
                advanceToNextValid();
                return *this;
            }

            bool operator!=(const MultiComponentIterator& other) const {
                return index != other.index;
            }

            auto operator*() {
                return std::make_tuple(std::get<ComponentTracker<Components>*>(trackers)->GetComponent(index)...);
            }


        private:
            size_t index;
            size_t maxIndex;
            std::tuple<ComponentTracker<Components>*...> trackers;

            void advanceToNextValid() {
                while (index < maxIndex) {
                    if (allComponentsActive(index)) break;
                    ++index;
                }
            }

            bool allComponentsActive(size_t idx) {
                return (std::get<ComponentTracker<Components>*>(trackers)->IsActive(idx) && ...);
            }
        };


        template<typename... Components>
        auto GetComponentGroup() {
            size_t maxEntities = m_MaxEntities;
            using Iterator = MultiComponentIterator<Components...>;

            return std::make_pair(
                Iterator(0, maxEntities, GetTracker<Components>().get()...),
                Iterator(maxEntities, maxEntities, GetTracker<Components>().get()...)
            );
        }
        static inline size_t s_TypeCounter = 0;

    private:
        uint64_t m_MaxComponents;
        uint64_t m_MaxEntities;
        std::vector<std::shared_ptr<void>> m_Trackers;
        std::vector<std::vector<RemoveEntry>> m_RemoveEntries;
    };

}

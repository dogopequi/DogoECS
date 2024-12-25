#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>

#include "UUID.h"
namespace DogoECS
{
    class DG_Component
    {
    public:
        DG_Component() : m_ComponentID(UUID()) {  }
        DG_Component(uint64_t id) : m_EntityID(id), m_ComponentID(UUID()) {}

        virtual ~DG_Component() {  }

        virtual void Update() {}

        bool GetUse() const { return m_InUse; }
        void SetUse(bool setter) { m_InUse = setter; }

        uint64_t GeEntityID_ui64() const { return m_EntityID; }
        uint64_t GetComponentID_ui64() const { return m_ComponentID.GetUUID_ui64(); }

        void SetEntityID(uint64_t e) { m_EntityID = e; }

    protected:
        bool m_InUse;
        uint64_t m_EntityID;
        UUID m_ComponentID;
    };

    class ComponentTrackerPARENT
    {
    public:
        virtual ~ComponentTrackerPARENT() = default;
        virtual void UpdateComponents() = 0;
    };

    template<typename TYPE>
    struct ComponentTracker : public ComponentTrackerPARENT
    {
        ComponentTracker() {}
        ComponentTracker(const ComponentTracker& other) = delete;
        ComponentTracker(ComponentTracker&& other) noexcept : VectorPointer(std::move(other.VectorPointer)), VectorLastUsed(std::move(other.VectorLastUsed)), ComponentMapPointer(std::move(other.ComponentMapPointer)) {}
        std::unique_ptr<std::vector<TYPE>> VectorPointer = std::make_unique<std::vector<TYPE>>();
        int32_t VectorLastUsed;
        std::unique_ptr<std::unordered_map<uint64_t, TYPE*>> ComponentMapPointer = std::make_unique<std::unordered_map<uint64_t, TYPE*>>();

        void UpdateComponents() override
        {
            auto& vector = *VectorPointer;
            for (size_t j = 0; j < vector.size(); j++)
            {
                if (j > VectorLastUsed)
                    break;
                auto& component = vector[j];
                if (component.GetUse())
                {
                    component.Update();
                }
            }
        }

        ~ComponentTracker() {}
    };

    class DG_ComponentManager
    {
    public:
        static DG_ComponentManager& GetInstance();

        DG_ComponentManager(const DG_ComponentManager&) = delete;
        DG_ComponentManager& operator=(const DG_ComponentManager&) = delete;
        ~DG_ComponentManager() {}


        static void Update()
        {
            DG_ComponentManager::GetInstance().UpdateImpl();
        }

    private:
        static void UpdateImpl()
        {
            for (const auto& tracker : m_ComponentTrackers)
            {
                if (tracker)
                {
                    tracker->UpdateComponents();
                }
                else
                {
                    std::cout << "Tracker is invalid" << std::endl;
                }
            }
        }

    public:
        template<typename Type>
        static void UpdateComponents()
        {
            DG_ComponentManager::GetInstance().UpdateComponentsImpl<Type>();
        }
    private:
        template<typename Type>
        void UpdateComponentsImpl()
        {
            std::cout << "No template specialization defined. Please use the macro UPDATE_COMPONENTS_TEMPLATE(YourComponent)" << std::endl;
        }
    public:
        template<typename Type>
        static void RegisterComponent()
        {
            DG_ComponentManager::GetInstance().RegisterComponentImpl<Type>();
        }
    private:
        template<typename Type>
        void RegisterComponentImpl()
        {
            std::cout << "No template specialization defined. Please use the macro REGISTER_COMPONENT_TEMPLATE(YourComponent)" << std::endl;
        }
    public:
        template <typename T>
        static T* AddComponent(uint64_t EntityID)
        {
            return DG_ComponentManager::GetInstance().AddComponentImpl<T>(EntityID);
        }
    private:
        template <typename T>
        T* AddComponentImpl(uint64_t EntityID)
        {
            std::cout << "Type not registered" << std::endl;
            return nullptr;
        }
    public:
        template <typename T>
        static void RemoveComponent(uint64_t componentID)
        {
            return DG_ComponentManager::GetInstance().RemoveComponentImpl<T>(componentID);
        }
    private:
        template <typename T>
        void RemoveComponentImpl(uint64_t componentID)
        {
            std::cout << "No template specialization defined. Please use the macro REMOVE_COMPONENT_TEMPLATE(YourComponent)" << std::endl;
        }


    public:
        static uint32_t GetMaxComponents() { return MAX_COMPONENTS; }

 
    private:
        DG_ComponentManager(){}
        static std::unique_ptr<DG_ComponentManager> s_Instance;
        static uint32_t MAX_COMPONENTS;
        static std::vector<std::shared_ptr<ComponentTrackerPARENT>> m_ComponentTrackers;
        static std::unordered_map<std::type_index, std::shared_ptr<ComponentTrackerPARENT>> m_ComponentTrackerMap;

    };
#define UPDATE_COMPONENTS_TEMPLATE(TYPE) \
void DG_ComponentManager::UpdateComponentsImpl<TYPE>() \
{ \
    std::type_index typeIndex(typeid(TYPE)); \
    if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()) \
    { \
        auto it = m_ComponentTrackerMap.find(typeIndex); \
        it->second->UpdateComponents(); \
        return; \
    } \
    std::cout << "Type not found: " << typeIndex.name() << std::endl; \
}


#define REGISTER_COMPONENT_TEMPLATE(TYPE) \
void DG_ComponentManager::RegisterComponentImpl<TYPE>() \
{\
    std::type_index typeIndex(typeid(TYPE));                                                                                                 \
    if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end())                                                                \
    {                                                                                                                                        \
        std::cout << "Type: " << typeIndex.name() << " already registered." << std::endl;                                                    \
        return;                                                                                                                              \
    }                                                                                                                                        \
                                                                                                                                             \
    m_ComponentTrackers.emplace_back(std::make_shared<ComponentTracker<TYPE>>());                                                            \
    auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<TYPE>>(m_ComponentTrackers.back());                                   \
    componentTracker->VectorPointer->resize(DG_ComponentManager::GetMaxComponents());                                                        \
                                                                                                                                             \
    for (int i = 0; i < DG_ComponentManager::GetMaxComponents(); i++)                                                                        \
    {                                                                                                                                        \
        componentTracker->VectorPointer->at(i).SetUse(false);                                                                                \
                                                                                                                                             \
        std::shared_ptr<TYPE> ptr = std::make_shared<TYPE>(componentTracker->VectorPointer->at(i));                            \
        (*componentTracker->ComponentMapPointer)[componentTracker->VectorPointer->at(i).GetComponentID_ui64()] = ptr.get();                  \
    }                                                                                                                                        \
    componentTracker->VectorLastUsed = -1;                                                                                                   \
                                                                                                                                             \
    m_ComponentTrackerMap[typeIndex] = m_ComponentTrackers.back();                                                                           \
                                                                                                                                             \
    auto it = m_ComponentTrackerMap.find(typeIndex);                                                                                         \
    if (it == m_ComponentTrackerMap.end())                                                                                                   \
    {                                                                                                                                        \
        std::cout << "Type not found in map" << std::endl;                                                                                   \
    }                                                                                                                                        \
}

#define ADD_COMPONENT_TEMPLATE(TYPE)                                                                                                      \
TYPE* DG_ComponentManager::AddComponentImpl<TYPE>(uint64_t EntityID)                                       \
{                                                                                                                                         \
    std::type_index typeIndex(typeid(TYPE));                                                                                \
    if (!(m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()))                                                          \
    {                                                                                                                                     \
        std::cout << "Type not registered" << std::endl;                                                                                  \
        return nullptr;                                                                                                                   \
    }                                                                                                                                     \
                                                                                                                                          \
    const auto& it = m_ComponentTrackerMap.find(typeIndex);                                                                               \
    auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<TYPE>>(it->second);                                  \
    componentTracker->VectorLastUsed++;                                                                                                   \
                                                                                                                                          \
    auto& component = componentTracker->VectorPointer->at(componentTracker->VectorLastUsed);                                              \
    component.SetUse(true);                                                                                                               \
    component.SetEntityID(EntityID);                                                                                                      \
    (*componentTracker->ComponentMapPointer)[component.GetComponentID_ui64()] = &component;                                               \
    return &component;                                                                                                                    \
}

#define REMOVE_COMPONENT_TEMPLATE(TYPE)                                                                                                   \
void DG_ComponentManager::RemoveComponentImpl<TYPE>(uint64_t componentID)                                                \
{                                                                                                                                         \
    std::type_index typeIndex(typeid(TYPE));                                                                                \
    const auto& map = m_ComponentTrackerMap.find(typeIndex);                                                                              \
    auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<TYPE>>(map->second);                                 \
    auto MapIT = componentTracker->ComponentMapPointer->find(componentID);                                                                \
                                                                                                                                          \
    if (MapIT != componentTracker->ComponentMapPointer->end())                                                                            \
    {                                                                                                                                     \
        componentTracker->ComponentMapPointer->erase(MapIT);                                                                              \
    }                                                                                                                                     \
    auto VectorIT = std::find_if(componentTracker->VectorPointer->begin(), componentTracker->VectorPointer->end(),                        \
        [componentID](TYPE& component) {                                                                                    \
            return component.GetComponentID_ui64() == componentID;                                                                        \
        });                                                                                                                               \
                                                                                                                                          \
    if (VectorIT != componentTracker->VectorPointer->end())                                                                               \
    {                                                                                                                                     \
        *VectorIT = TYPE();                                                                                                 \
        (*VectorIT).SetUse(false);                                                                                                        \
        (*VectorIT).SetEntityID(0);                                                                                                       \
    }                                                                                                                                     \
}
}
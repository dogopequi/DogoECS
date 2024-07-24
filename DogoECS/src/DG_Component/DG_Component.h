#pragma once
#include "UUID.h"
class DG_Component
{
public:
    DG_Component() : m_ComponentID(UUID()) { /*std::cout << "DG Component created" << std::endl;*/ }
    DG_Component(uint64_t id) : m_EntityID(id), m_ComponentID(UUID()) {}

    virtual ~DG_Component() { /*std::cout << "DG Component deleted" << std::endl;*/ }

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

struct ComponentTracker
{
    ComponentTracker() {}
    ComponentTracker(const ComponentTracker& other) = delete;
    ComponentTracker(ComponentTracker&& other) noexcept : VectorPointer(std::move(other.VectorPointer)), VectorLastUsed(std::move(other.VectorLastUsed)), /*EntityMapPointer(std::move(other.EntityMapPointer)),*/ ComponentMapPointer(std::move(other.ComponentMapPointer)) {}
    std::unique_ptr<std::vector<std::shared_ptr<DG_Component>>> VectorPointer;
    int32_t VectorLastUsed;
    std::unique_ptr<std::unordered_map<uint64_t, DG_Component*>> ComponentMapPointer;

    ~ComponentTracker() {}
};

class DG_ComponentManager
{
public:
    DG_ComponentManager(const DG_ComponentManager&) = delete;
    DG_ComponentManager& operator=(const DG_ComponentManager&) = delete;
    ~DG_ComponentManager(){}

 
    static void Update()
    {
        for (const auto& tracker : m_ComponentTrackers)
        {
            if (tracker)
            {
                for (int j = 0; j < (*tracker->VectorPointer).size(); j++)
                {
                    if (j > tracker->VectorLastUsed)
                        break;
                    if (!(*tracker->VectorPointer).at(j)->GetUse())
                        continue;
                    (*tracker->VectorPointer).at(j)->Update();
                }
            }
            else
            {
                std::cout << "Tracker is invalid" << std::endl;
            }
        }
    }


    template<typename Type>
    static void RegisterComponent()
    {
        std::type_index typeIndex(typeid(Type));
        if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end())
        {
            std::cout << "Type already exists" << std::endl;
            return;
        }
        m_ComponentTrackers.push_back(std::shared_ptr<ComponentTracker>(new ComponentTracker(),
            [](ComponentTracker* track) { std::cout << "Deleted shared_ptr " << std::endl; delete track; }));
        m_ComponentTrackers.back()->VectorPointer.reset(new std::vector<std::shared_ptr<DG_Component>>());
        m_ComponentTrackers.back()->VectorPointer->resize(DG_ComponentManager::GetMaxComponents());
        m_ComponentTrackers.back()->ComponentMapPointer = std::make_unique<std::unordered_map<uint64_t, DG_Component*>>();
        for (int i = 0; i < DG_ComponentManager::GetMaxComponents(); i++)
        {
            (*m_ComponentTrackers.back()->VectorPointer)[i] = std::make_shared<Type>();
            (*m_ComponentTrackers.back()->VectorPointer)[i]->SetUse(false);
            DG_Component* ptr = (*m_ComponentTrackers.back()->VectorPointer)[i].get();
            (*m_ComponentTrackers.back()->ComponentMapPointer)[(*m_ComponentTrackers.back()->VectorPointer)[i]->GetComponentID_ui64()] = ptr;
        }
        m_ComponentTrackers.back()->VectorLastUsed = -1;

        m_ComponentTrackerMap[typeIndex] = m_ComponentTrackers.back();

        auto it = m_ComponentTrackerMap.find(typeIndex);
        if (it == m_ComponentTrackerMap.end())
        {
            std::cout << "Type not found in map" << std::endl;
        }
    }

    //template <typename T>
    //T* AddComponent(uint64_t EntityID);
    template <typename T>
    static T* AddComponent(uint64_t EntityID)
    {
        std::type_index typeIndex(typeid(T));
        if (!(m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()))
        {
            std::cout << "Type not registered" << std::endl;
            return nullptr;
        }


        const auto& it = m_ComponentTrackerMap.find(typeIndex);
        it->second->VectorLastUsed++;


        (*it->second->VectorPointer).emplace(it->second->VectorPointer->begin() + it->second->VectorLastUsed, std::make_shared<T>(EntityID));
        auto& component = (*it->second->VectorPointer)[it->second->VectorLastUsed];
        component->SetUse(true);
        component->SetEntityID(EntityID);
        (*it->second->ComponentMapPointer)[component->GetComponentID_ui64()] = component.get();
        T* derivedPtr = dynamic_cast<T*>((*it->second->ComponentMapPointer)[component->GetComponentID_ui64()]);
        return derivedPtr;
    }

    template <typename T>
    static void RemoveComponent(uint64_t componentID);



    static uint32_t GetMaxComponents() { return MAX_COMPONENTS; }

protected:
    DG_ComponentManager() = default;

private:
    static DG_ComponentManager* s_Instance;
    static uint32_t MAX_COMPONENTS;
    static std::vector<std::shared_ptr<ComponentTracker>> m_ComponentTrackers;

    static std::unordered_map<std::type_index, std::shared_ptr<ComponentTracker>> m_ComponentTrackerMap;

};

#define REGISTER_COMPONENT_TEMPLATE(TYPE) \
static void DG_ComponentManager::RegisterComponent<TYPE>()                                                                                       \
{                                                                                                                                                        \
std::type_index typeIndex(typeid(TYPE));                                                                                               \
if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end())                                                                            \
{                                                                                                                                                    \
std::cout << "Type: " << typeIndex.name() << " already registered." << std::endl;                                                                                                 \
return;                                                                                                                                          \
}                                                                                                                                                    \
m_ComponentTrackers.push_back(std::shared_ptr<ComponentTracker>(new ComponentTracker(), \
    [](ComponentTracker* track) { /*std::cout << "Deleted shared_ptr " << std::endl;*/ delete track; }));                                                \
    m_ComponentTrackers.back()->VectorPointer.reset(new std::vector<std::shared_ptr<DG_Component>>());                                                   \
    m_ComponentTrackers.back()->VectorPointer->resize(DG_ComponentManager::GetMaxComponents());                                                                                   \
    m_ComponentTrackers.back()->ComponentMapPointer = std::make_unique<std::unordered_map<uint64_t, DG_Component*>>();                                            \
    for (int i = 0; i < DG_ComponentManager::GetMaxComponents(); i++)                                                                                                             \
    {                                                                                                                                                    \
        (*m_ComponentTrackers.back()->VectorPointer)[i] = std::make_shared<TYPE>();                                                        \
        (*m_ComponentTrackers.back()->VectorPointer)[i]->SetUse(false);                                                                                  \
        std::shared_ptr<TYPE> ptr = std::dynamic_pointer_cast<TYPE>((*m_ComponentTrackers.back()->VectorPointer)[i]);                                                     \
        (*m_ComponentTrackers.back()->ComponentMapPointer)[(*m_ComponentTrackers.back()->VectorPointer)[i]->GetComponentID_ui64()] = ptr.get();                       \
    }                                                                                                                                                    \
        m_ComponentTrackers.back()->VectorLastUsed = -1;                                                                                                     \
                                                                                                                                                                     \
        m_ComponentTrackerMap[typeIndex] = m_ComponentTrackers.back();                                                                                       \
                                                                                                                                                                            \
        auto it = m_ComponentTrackerMap.find(typeIndex);                                                                                                     \
        if (it == m_ComponentTrackerMap.end())                                                                                                               \
        {                                                                                                                                                    \
            std::cout << "Type not found in map" << std::endl;                                                                                               \
        }                                                                                                                                                    \
}    

#define ADD_COMPONENT_TEMPLATE(TYPE)                                                                                                                                     \
static TYPE* DG_ComponentManager::AddComponent<TYPE>(uint64_t EntityID)                                                                                                               \
{                                                                                                                                                                           \
    std::type_index typeIndex(typeid(TYPE));                                                                                                                                \
    if (!(m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()))                                                                                            \
    {                                                                                                                                                                       \
        std::cout << "Type not registered" << std::endl;                                                                                                                    \
        return nullptr;                                                                                                                                                     \
    }                                                                                                                                                                       \
                                                                                                                                                                            \
    const auto& it = m_ComponentTrackerMap.find(typeIndex);                                                                                                                 \
    it->second->VectorLastUsed++;                                                                                                                                           \
                                                                                                                                                                            \
                                                                                                                                                                            \
    (*it->second->VectorPointer).emplace(it->second->VectorPointer->begin() + it->second->VectorLastUsed, std::make_shared<TYPE>(EntityID));                  \
    auto& component = (*it->second->VectorPointer)[it->second->VectorLastUsed];                                                                                             \
    component->SetUse(true);  \
    component->SetEntityID(EntityID); \
    (*it->second->ComponentMapPointer)[component->GetComponentID_ui64()] = component.get();                                                                                 \
    TYPE* derivedPtr = static_cast<TYPE*>((*it->second->ComponentMapPointer)[component->GetComponentID_ui64()]);   \
    return derivedPtr;                                                                                                                                                      \
}  

#define REMOVE_COMPONENT_TEMPLATE(TYPE)                                                                                                     \
static void DG_ComponentManager::RemoveComponent<TYPE>(uint64_t componentID)                                                                       \
{                                                                                                                                           \
    std::type_index typeIndex(typeid(TYPE));                                                                                                \
    const auto& map = m_ComponentTrackerMap.find(typeIndex);                                                                                \
    auto MapIT = map->second->ComponentMapPointer->find(componentID);                                                                       \
                                                                                                                                            \
    if (MapIT != map->second->ComponentMapPointer->end())                                                                                   \
    {                                                                                                                                       \
        map->second->ComponentMapPointer->erase(MapIT);                                                                                     \
    }                                                                                                                                       \
    auto VectorIT = std::find_if(map->second->VectorPointer->begin(), map->second->VectorPointer->end(),                                    \
        [componentID](const std::shared_ptr<DG_Component>& component) {                                                                     \
            return component->GetComponentID_ui64() == componentID;                                                                         \
        });                                                                                                                                 \
    if (VectorIT != map->second->VectorPointer->end())                                                                                      \
    {                                                                                                                                       \
        *VectorIT = std::make_shared<TYPE>();                                                                                               \
        (*VectorIT)->SetUse(false); \
        (*VectorIT)->SetEntityID(0); \
    }                                                                                                                                       \
}


//(*map->second->VectorPointer). = std::make_shared<TransformComponent>();                                                              \
    //(*map->second->VectorPointer)[map->second->VectorLastUsed]->SetUse(false);                                                            \
    //map->second->VectorLastUsed--;                                                                                                        \
#pragma once
#include "Components/Components.h"

void customDeleter(int* ptr) {
    std::cout << "Custom deleter called for ptr: " << ptr << std::endl;
    delete ptr;  // Properly delete the managed object
}
struct ComponentTracker
{
    ComponentTracker() {}
    ComponentTracker(const ComponentTracker& other) = delete;
    ComponentTracker(ComponentTracker&& other) noexcept : VectorPointer(std::move(other.VectorPointer)), VectorLastUsed(std::move(other.VectorLastUsed)), MapPointer(std::move(other.MapPointer)) {}
    std::unique_ptr<std::vector<std::any>> VectorPointer;
    int32_t VectorLastUsed;
    std::unique_ptr<std::unordered_map<uint64_t, DG_Component*>> MapPointer;

    ~ComponentTracker() {}
};

class DG_ComponentManager
{
public:
    DG_ComponentManager()
    {

    }
    ~DG_ComponentManager(){}

 
    void Update()
    {
        for (const auto& tracker : m_ComponentTrackers)
        {
            int32_t i = 0;
            if (tracker)
            {
                for (int j = 0; j < (*tracker->VectorPointer).size(); j++)
                {
                    if (i > tracker->VectorLastUsed)
                        break;
                    i++;
                    /*(*tracker->VectorPointer).at(j).GetUse();*/
                    //if ((*tracker->VectorPointer)->GetUse())
                    //    continue;
                    //(*tracker->VectorPointer)->Update();
                }
            }
            else
            {
                std::cout << "Tracker is invalid" << std::endl;
            }
        }
    }


    template<typename Type>
    void RegisterComponent()
    {
        std::type_index typeIndex(typeid(Type));
        if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end())
        {
            std::cout << "Type already exists" << std::endl;
            return;
        }
        m_ComponentTrackers.push_back(std::shared_ptr<ComponentTracker>(new ComponentTracker(),
            [](ComponentTracker* track) { std::cout << "Deleted shared_ptr " << std::endl; delete track; }));
        m_ComponentTrackers.back()->VectorPointer.reset(new std::vector<std::any>());
        m_ComponentTrackers.back()->VectorPointer->resize(MAX_COMPONENTS);
        m_ComponentTrackers.back()->MapPointer = std::make_unique<std::unordered_map<uint64_t, DG_Component*>>();
        for (int i = 0; i < MAX_COMPONENTS; i++)
        {
            Type component;
            component.SetUse(false);

            (*m_ComponentTrackers.back()->VectorPointer)[i] = std::move(component);
            Type& storedComponent = std::any_cast<Type&>((*m_ComponentTrackers.back()->VectorPointer)[i]);
            (*m_ComponentTrackers.back()->MapPointer)[storedComponent.GeEntityID_ui64()] = &storedComponent;
        }
        m_ComponentTrackers.back()->VectorLastUsed = -1;
        std::cout << "last vector used: " << m_ComponentTrackers.back()->VectorLastUsed << std::endl;
        std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&m_ComponentTrackers.back()->VectorLastUsed) << std::endl;

        std::cout << "Address of vector: " << &(*m_ComponentTrackers.back()->VectorPointer).at(0) << std::endl;

        //for (const auto& tracker : m_ComponentTrackers)
        //{
        //    std::cout << "size of trackers: " << m_ComponentTrackers.size() << std::endl;
        //    int32_t i = 0;
        //    std::cout << "Address of vector: " << &(*tracker->VectorPointer).at(0) << std::endl;
        //    for (int j = 0; j < (*tracker->VectorPointer).size(); j++)
        //    {
        //        Type& c = std::any_cast<Type&>((*tracker->VectorPointer)[j]);
        //        if (i > tracker->VectorLastUsed)
        //            break;
        //        i++;
        //        if (!c.GetUse())
        //            continue;
        //        c.Update();
        //    }
        //}

        m_ComponentTrackerMap[typeIndex] = m_ComponentTrackers.back();

        auto it = m_ComponentTrackerMap.find(typeIndex);
        if (it != m_ComponentTrackerMap.end())
        {
            std::cout << std::dec << "last vector used: " << it->second->VectorLastUsed << std::endl;
            std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&it->second->VectorLastUsed) << std::endl;
            std::cout << "Address of vector: " << &(*it->second->VectorPointer).at(0) << std::endl;
        }
        else
        {
            std::cout << "Type not found in map" << std::endl;
        }
    }

    template <typename T>
    DG_Component* AddComponent(uint64_t EntityID)
    {
        std::type_index typeIndex(typeid(T));
        if (!(m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()))
        {
            std::cout << "Type not registered" << std::endl;
            return nullptr;
        }


        const auto& it = m_ComponentTrackerMap.find(typeIndex);
        it->second->VectorLastUsed++;


        (*it->second->VectorPointer).emplace(it->second->VectorPointer->begin() + it->second->VectorLastUsed, T(EntityID));
        auto& component = std::any_cast<T>((*it->second->VectorPointer)[it->second->VectorLastUsed]);
        component.SetUse(true);
        T& storedComponent = std::any_cast<T&>((*it->second->VectorPointer)[it->second->VectorLastUsed]);
        (*it->second->MapPointer)[EntityID] = &storedComponent;
        DG_Component* derivedPtr = static_cast<DG_Component*>((*it->second->MapPointer)[EntityID]);
        return derivedPtr;
    }

    //template <typename T>
    //void RemoveComponent(uint64_t EntityID);


public:

    uint32_t MAX_COMPONENTS = 5000;
    std::vector<std::shared_ptr<ComponentTracker>> m_ComponentTrackers;

    std::unordered_map<std::type_index, std::shared_ptr<ComponentTracker>> m_ComponentTrackerMap;

};

//template <>
//TransformComponent* DG_ComponentManager::AddComponent<TransformComponent>(uint64_t EntityID)
//{
//    m_TC_LastUsed++;
//    m_TransformComponentArray[m_TC_LastUsed] = TransformComponent(EntityID);
//    m_TransformComponentArray[m_TC_LastUsed].SetUse(true);
//    m_TransformComponentMap[EntityID] = &m_TransformComponentArray[m_TC_LastUsed];
//
//    return m_TransformComponentMap[EntityID];
//}
//template <>
//AudioComponent* DG_ComponentManager::AddComponent<AudioComponent>(uint64_t EntityID)
//{
//    m_AC_LastUsed++;
//    m_AudioComponentArray[m_AC_LastUsed] = AudioComponent(EntityID);
//    m_AudioComponentArray[m_AC_LastUsed].SetUse(true);
//    m_AudioComponentMap[EntityID] = &m_AudioComponentArray[m_AC_LastUsed];
//
//    return m_AudioComponentMap[EntityID];
//}
//
//template <>
//void DG_ComponentManager::RemoveComponent<TransformComponent>(uint64_t EntityID)
//{
//    auto it = m_TransformComponentMap.find(EntityID);
//    if (it != m_TransformComponentMap.end())
//    {
//        m_TransformComponentMap.erase(it);
//    }
//
//    m_TransformComponentArray[m_AC_LastUsed] = TransformComponent();
//    m_TransformComponentArray[m_AC_LastUsed].SetUse(false);
//
//    m_TC_LastUsed--;
//}
//
//template <>
//void DG_ComponentManager::RemoveComponent<AudioComponent>(uint64_t EntityID)
//{
//    auto it = m_AudioComponentMap.find(EntityID);
//    if (it != m_AudioComponentMap.end())
//    {
//        m_AudioComponentMap.erase(it);
//    }
//
//    m_AudioComponentArray[m_AC_LastUsed] = AudioComponent();
//    m_AudioComponentArray[m_AC_LastUsed].SetUse(false);
//
//    m_AC_LastUsed--;
//}

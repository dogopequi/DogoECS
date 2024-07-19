#pragma once
#include "Components/Components.h"

struct ComponentTracker
{
    ComponentTracker(){}
    ComponentTracker(const ComponentTracker& other) = delete;
    ComponentTracker(ComponentTracker&& other) noexcept : VectorPointer(std::move(other.VectorPointer)), VectorLastUsed(std::move(other.VectorLastUsed)), MapPointer(std::move(other.MapPointer)){}
    std::unique_ptr<std::vector<DG_Component>> VectorPointer;
    int32_t VectorLastUsed;
    std::unique_ptr<std::unordered_map<uint64_t, DG_Component*>> MapPointer;

    ~ComponentTracker(){}
};
//namespace std
//{
//    template<>
//    struct hash<
//}

class DG_ComponentManager
{
public:
    DG_ComponentManager()
    {
        //m_TransformComponentArray.resize(MAX_COMPONENTS);
        //m_AudioComponentArray.resize(MAX_COMPONENTS);
        //for (int i = 0; i < MAX_COMPONENTS; i++)
        //{
        //    m_TransformComponentArray[i] = TransformComponent();
        //    m_TransformComponentArray[i].SetUse(false);
        //    /* m_TransformComponentMap[m_TransformComponentArray[i].m_EntityID] = &m_TransformComponentArray[i];*/

        //    m_AudioComponentArray[i] = AudioComponent();
        //    m_AudioComponentArray[i].SetUse(false);
        //    /*m_AudioComponentMap[m_AudioComponentArray[i].m_EntityID] = &m_AudioComponentArray[i];*/
        //}
    }
    ~DG_ComponentManager(){}

    //void UpdateTransform()
    //{
    //    int32_t i = 0;
    //    for (auto& x : m_TransformComponentArray)
    //    {
    //        if (i > m_TC_LastUsed)
    //        {
    //            break;
    //        }
    //        i++;
    //        if (!x.GetUse())
    //            continue;
    //        x.Update();
    //    }
    //}

    //void UpdateAudio()
    //{
    //    int32_t i = 0;
    //    for (auto& x : m_AudioComponentArray)
    //    {
    //        if (i > m_AC_LastUsed)
    //            break;
    //        i++;
    //        if (!x.GetUse())
    //            continue;
    //        x.Update();
    //    }
    //}

    void Update()
    {
        for (const auto& tracker : m_ComponentTrackers)
        {
            int32_t i = 0;
            for (auto& component : *tracker->VectorPointer)
            {
                if (i > tracker->VectorLastUsed)
                    break;
                i++;
                if (!component.GetUse())
                    continue;
                component.Update();
            }
        }
    }

    template<typename Type>
    void RegisterComponent()
    {

        auto tracker = std::make_unique<ComponentTracker>();
        std::type_index typeIndex(typeid(Type));
        if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end())
        {
            std::cout << "Type already exists" << std::endl;
            return;
        }
        tracker->VectorPointer.reset(new std::vector<DG_Component>());
        tracker->VectorPointer->resize(MAX_COMPONENTS);
        tracker->MapPointer = std::make_unique<std::unordered_map<uint64_t, DG_Component*>>();
        for (int i = 0; i < MAX_COMPONENTS; i++)
        {
            (*tracker->VectorPointer)[i] = Type();
            (*tracker->VectorPointer)[i].SetUse(false);
            (*tracker->MapPointer)[(*tracker->VectorPointer)[i].GeEntityID_ui64()] = &(*tracker->VectorPointer)[i];
        }
        tracker->VectorLastUsed = -1;
        std::cout << tracker->VectorLastUsed << std::endl;
        std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&tracker->VectorLastUsed) << std::endl;

        m_ComponentTrackers.push_back(std::move(tracker));
        m_ComponentTrackerMap[typeIndex] = std::move(m_ComponentTrackers.back());

        auto it = m_ComponentTrackerMap.find(typeIndex);
        if (it != m_ComponentTrackerMap.end())
        {
            std::cout << std::dec << it->second->VectorLastUsed << std::endl;
            std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&it->second->VectorLastUsed) << std::endl;
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
        auto& it = m_ComponentTrackerMap.find(typeIndex);
        it->second->VectorLastUsed++;
        std::cout << std::dec << it->second->VectorLastUsed << std::endl;
        std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&it->second->VectorLastUsed) << std::endl;
        (*it->second->VectorPointer)[it->second->VectorLastUsed] = T(EntityID);
        (*it->second->VectorPointer)[it->second->VectorLastUsed].SetUse(true);
        (*it->second->MapPointer)[EntityID] = &(*it->second->VectorPointer)[it->second->VectorLastUsed];
        DG_Component* derivedPtr = static_cast<DG_Component*>((*it->second->MapPointer)[EntityID]);
        return derivedPtr;
    }

    //template <typename T>
    //void RemoveComponent(uint64_t EntityID);


private:

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

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
    class DG_Component
    {
    public:
        DG_Component() : m_ComponentID(UUID()) {  }
        DG_Component(uint64_t id) : m_EntityID(id), m_ComponentID(UUID()) {}

        virtual ~DG_Component() {  }

        virtual void Update() {}

        bool GetUse() const { return m_InUse; }
        void SetUse(bool setter) { m_InUse = setter; }

        uint64_t GetEntityID_ui64() const { return m_EntityID; }
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
    public:
        ComponentTracker() {}
        ComponentTracker(const ComponentTracker& other) = delete;
        ComponentTracker(ComponentTracker&& other) noexcept : VectorPointer(std::move(other.VectorPointer)), VectorLastUsed(std::move(other.VectorLastUsed)), ComponentMapPointerByEntityID(std::move(other.ComponentMapPointerByEntityID)),  ComponentMapPointerByComponentID(std::move(other.ComponentMapPointerByComponentID)) {}
        std::unique_ptr<std::vector<TYPE>> VectorPointer = std::make_unique<std::vector<TYPE>>();
        int32_t VectorLastUsed;
        std::unique_ptr<std::unordered_map<uint64_t, TYPE*>> ComponentMapPointerByEntityID = std::make_unique<std::unordered_map<uint64_t, TYPE*>>();
        std::unique_ptr<std::unordered_map<uint64_t, TYPE*>> ComponentMapPointerByComponentID = std::make_unique<std::unordered_map<uint64_t, TYPE*>>();

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

    public:
        static uint32_t GetMaxComponents() { return MAX_COMPONENTS; }

        template<typename Type>
        static void RegisterComponent()
        {
            std::type_index TypeIndex(typeid(Type));                                                                               
            if (m_ComponentTrackerMap.find(TypeIndex) != m_ComponentTrackerMap.end())                                              
            {                                                                                                                      
                std::cout << "Type: " << TypeIndex.name() << " already registered." << std::endl;                                  
                return;                                                                                                            
            }                                                                                                                      
                                                                                                                                   
            m_ComponentTrackers.emplace_back(std::make_shared<ComponentTracker<Type>>());                                          
            auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<Type>>(m_ComponentTrackers.back());                 
            componentTracker->VectorPointer->resize(DG_ComponentManager::GetMaxComponents());                                      
                                                                                                                                   
            for (int i = 0; i < DG_ComponentManager::GetMaxComponents(); i++)                                                      
            {                                                                                                                      
                componentTracker->VectorPointer->at(i).SetUse(false);                                                              
                                                                                                                                   
                std::shared_ptr<Type> ptr = std::make_shared<Type>(componentTracker->VectorPointer->at(i));                        
                (*componentTracker->ComponentMapPointerByComponentID)[componentTracker->VectorPointer->at(i).GetComponentID_ui64()] = ptr.get();
            }                                                                                                                      
            componentTracker->VectorLastUsed = -1;                                                                                 
                                                                                                                                   
            m_ComponentTrackerMap[TypeIndex] = m_ComponentTrackers.back();                                                         
                                                                                                                                   
            auto it = m_ComponentTrackerMap.find(TypeIndex);                                                                       
            if (it == m_ComponentTrackerMap.end())                                                                                 
            {                                                                                                                      
                std::cout << "Type not found in map" << std::endl;                                                                 
            }                                                                                                                      
        }
        
        template<typename TYPE>
        static TYPE* AddComponent(uint64_t EntityID)                   
        {                                                                                              
            std::type_index typeIndex(typeid(TYPE));                                                   
            if (!(m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()))               
            {                                                                                          
                std::cout << "Type not registered" << std::endl;                                       
                return nullptr;                                                                        
            }                                                                                          
                                                                                                       
            const auto& it = m_ComponentTrackerMap.find(typeIndex);                                    
            auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<TYPE>>(it->second);     
            componentTracker->VectorLastUsed++;                                                        
                                                                                                       
            auto& component = componentTracker->VectorPointer->at(componentTracker->VectorLastUsed);   
            component.SetUse(true);                                                                    
            component.SetEntityID(EntityID);                                                           
            (*componentTracker->ComponentMapPointerByEntityID)[component.GetEntityID_ui64()] = &component;
            return &component;                                                                         
        }

        template<typename TYPE>
        void RemoveComponent(uint64_t componentID)                                      
        {                                                                                                                      
            std::type_index typeIndex(typeid(TYPE));                                                                           
            const auto& map = m_ComponentTrackerMap.find(typeIndex);                                                           
            auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<TYPE>>(map->second); 
            auto MapIT = componentTracker->ComponentMapPointerByComponentID->find(componentID);                                             
            uint64_t eID = 0;
            if (MapIT != componentTracker->ComponentMapPointerByComponentID->end())
            {                                
                eID = MapIT->second->GetEntityID_ui64();
                componentTracker->ComponentMapPointerByComponentID->erase(MapIT);
            }
            if(eID != 0)
            {
                auto MapIT1 = componentTracker->ComponentMapPointerByEntityID->find(eID);
                if (MapIT1 != componentTracker->ComponentMapPointerByEntityID->end())
                {
                    componentTracker->ComponentMapPointerByEntityID->erase(MapIT1);
                }
            }
            auto VectorIT = std::find_if(componentTracker->VectorPointer->begin(), componentTracker->VectorPointer->end(),     
                [componentID](TYPE& component) -> bool{                                                                               
                    return component.GetComponentID_ui64() == componentID;                                                     
                });                                                                                                                                                                          
            if (VectorIT != componentTracker->VectorPointer->end())                                                            
            {                                                                                                                  
                *VectorIT = TYPE();                                                                                            
                (*VectorIT).SetUse(false);                                                                                     
                (*VectorIT).SetEntityID(0);                                                                                    
            }                                                                                                                  
        }


        template<typename TYPE>
        static TYPE* GetComponent(uint64_t id) {
            std::type_index typeIndex(typeid(TYPE));
            if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()) {
                auto it = m_ComponentTrackerMap.find(typeIndex);
                auto trackerPtr = it->second;

                auto componentTracker = std::dynamic_pointer_cast<ComponentTracker<TYPE>>(trackerPtr);
                if (componentTracker) {
                    if (componentTracker->ComponentMapPointerByEntityID->find(id) != componentTracker->ComponentMapPointerByEntityID->end()) {
                        auto it = componentTracker->ComponentMapPointerByEntityID->find(id);
                        return it->second;
                    }
                }
            }
            return nullptr;
        }

        template<typename TYPE>
        static void UpdateComponents() 
        { 
            std::type_index typeIndex(typeid(TYPE)); 
            if (m_ComponentTrackerMap.find(typeIndex) != m_ComponentTrackerMap.end()) 
            { 
                auto it = m_ComponentTrackerMap.find(typeIndex); 
                it->second->UpdateComponents(); 
                return; 
            } 
            std::cout << "Type not found: " << typeIndex.name() << std::endl; 
        }


    private:
        DG_ComponentManager() {}
        static std::unique_ptr<DG_ComponentManager> s_Instance;
        static uint32_t MAX_COMPONENTS;
        static std::vector<std::shared_ptr<ComponentTrackerPARENT>> m_ComponentTrackers;
        static std::unordered_map<std::type_index, std::shared_ptr<ComponentTrackerPARENT>> m_ComponentTrackerMap;
    };
}

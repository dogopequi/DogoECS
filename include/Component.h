#pragma once
#include "DG_Component.h"
namespace DogoECS {
    class AudioComponent : public DG_Component
            {
            public:
                AudioComponent(uint64_t id) : DG_Component(id) {}
                AudioComponent() : DG_Component() {}
        
                ~AudioComponent() override { }
        
        
                void Update() override
                {
        
                    std::cout << "AUDIO --------------------------------------------------- " << std::endl;
                    std::cout << "Name: " << name << std::endl;
                    std::cout << "Component ID: " << m_ComponentID.GetUUID_ui64() << std::endl;
                    std::cout << "EntityID: " << m_EntityID << std::endl;
                }
        
                void SetName(std::string name) { this->name = name; }
                std::string GetName() const { return name; }
        
            private:
                std::string name;
            };
            //template<>
            //GET_COMPONENT_TEMPLATE(AudioComponent)
            //    
            //template<>
            //REGISTER_COMPONENT_TEMPLATE(AudioComponent)
        
            //template<>
            //ADD_COMPONENT_TEMPLATE(AudioComponent)
        
            //template <>
            //REMOVE_COMPONENT_TEMPLATE(AudioComponent)
        
            //template<>
            //UPDATE_COMPONENTS_TEMPLATE(AudioComponent)
}
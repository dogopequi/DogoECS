//// FOR// TESTS....!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
#include "../include/DogoECS.h"
#include <iostream>
#include "../include/Component.h"

    void main()
    {
        using namespace DogoECS;
        Init();
        Entity* E1 = DG_EntityManager::CreateEntity();
        DG_ComponentManager::RegisterComponent<AudioComponent>();
        DG_ComponentManager::RegisterComponent<AudioComponent>();
        AudioComponent* a1 = E1->AddComponent<AudioComponent>();
        std::cout << a1->GetEntityID_ui64();
        DG_ComponentManager::UpdateComponents<AudioComponent>();
        AudioComponent* a2 = DG_ComponentManager::GetComponent<AudioComponent>(E1->GetID_ui64());
        std::cout << a2->GetEntityID_ui64() << std::endl;
        std::cout << a2->GetComponentID_ui64() << std::endl;

        E1->RemoveComponent<AudioComponent>(a1->GetComponentID_ui64());
        DG_ComponentManager::UpdateComponents<AudioComponent>();
    }
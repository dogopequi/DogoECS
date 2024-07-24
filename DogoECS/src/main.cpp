#include "dgpch.h"
#include "UUID.h"
#include "Entity/Entity.h"
#include "DG_Component/DG_Component.h"
#include "Components/Components.h"
static EntityManager s_EntityManager;

int main()
{
	Entity* E1 = s_EntityManager.CreateEntity();
	if (E1) {
		std::cout << "Entity created successfully.\n";
	}
	Entity* E2 = s_EntityManager.CreateEntity();
	if (E2) {
		std::cout << "Entity created successfully.\n";
	}
	S_ComponentManager.RegisterComponent<TransformComponent>();
	S_ComponentManager.RegisterComponent<TransformComponent>();
	S_ComponentManager.RegisterComponent<AudioComponent>();

	TransformComponent* TCE1 = E1->AddComponent<TransformComponent>();
	TCE1->SetX(1.0f);
	TCE1->SetY(1.0f);
	TCE1->SetZ(1.0f);


	TransformComponent* TCE2 = E2->AddComponent<TransformComponent>();
	TCE2->SetX(2.0f);
	TCE2->SetY(2.0f);
	TCE2->SetZ(2.0f);

	AudioComponent* ACE1 = (AudioComponent*)(E2->AddComponent<AudioComponent>());
	ACE1->SetName("YAY1");

	AudioComponent* ACE2 = (AudioComponent*)(E1->AddComponent<AudioComponent>());
	ACE2->SetName("YAY2");

	std::cout << "UPDATE 1 -----------------------------------------------------------" << std::endl;

	S_ComponentManager.Update();

	E1->RemoveComponent<TransformComponent>(TCE1->GetComponentID_ui64());

	std::cout << "UPDATE 2 -----------------------------------------------------------" << std::endl;

	S_ComponentManager.Update();

	E1->RemoveComponent<TransformComponent>(TCE2->GetComponentID_ui64());
	std::cout << "UPDATE 3 -----------------------------------------------------------" << std::endl;

	S_ComponentManager.Update();

	E1->RemoveComponent<AudioComponent>(ACE2->GetComponentID_ui64());
	std::cout << "UPDATE 4 -----------------------------------------------------------" << std::endl;

	S_ComponentManager.Update();

	E1->RemoveComponent<AudioComponent>(ACE1->GetComponentID_ui64());
	std::cout << "UPDATE 5 -----------------------------------------------------------" << std::endl;

	S_ComponentManager.Update();

}

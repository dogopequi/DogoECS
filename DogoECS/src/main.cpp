#include "dgpch.h"
#include "UUID.h"
#include "Entity/Entity.h"
#include "DG_Component/DG_Component.h"
#include "Components/Components.h"
static EntityManager s_EntityManager;

int main()
{
	Entity E1, E2;
	std::optional<Entity> optionalEntity1 = s_EntityManager.CreateEntity();
	if (optionalEntity1.has_value()) {
		Entity E1 = optionalEntity1.value();
		std::cout << "Entity created successfully.\n";
	}
	S_ComponentManager.RegisterComponent<TransformComponent>();
	S_ComponentManager.RegisterComponent<TransformComponent>();

	TransformComponent* TCE1 = static_cast<TransformComponent*>(E1.AddComponent<TransformComponent>());
	TCE1->SetX(1.0f);
	std::cout << TCE1->GetX() << std::endl;
	//TCE1->SetY(1.0f);
	//TCE1->SetZ(1.0f);

	//S_ComponentManager.Update();
}

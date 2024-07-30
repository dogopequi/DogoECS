# DogoECS
## An Entity Component System

 I'm developing this for my game engine.

## Build Instructions
Run "GenerateProjects.bat" or "GenerateProjectsLinux.sh".

It's made to create a VS2022 solution file on Windows and makefiles on Linux by default, alter as you wish by consulting premake's supported targets:

[Premake](https://premake.github.io/docs/using-premake)

## Goals
- Fast
- Easy to use

## Implementation details

The entities by themselves will be packed in an array by the Entity Manager, keeping track of the available entities.
An entity will have member functions to add or remove components, which will call Entity Manager's add function with the Entity's ID, assigning it to the component's EntityID variable.

The components will be packed, for now, in arrays sorted by type. The goal is to update each component array at once, so the CPU doesn't have to do cold reads.

## Usage


Create an entity by calling `DG_EntityManager::CreateEntity();`:
``` cpp
Entity* E1 = DG_EntityManager::CreateEntity();
```
This function returns a pointer to an Entity. To check if it's valid just place it in an if() check.

To add a component first you need to register them.
This is done by calling `DG_ComponentManager::RegisterComponent<YourComponent>();`

This function returns nothing, it simply allocates memory for the components.
 
If you try to call it twice for the same class of component, it exits since it has already allocated the memory. Soon I will add a parameter to let the user decide how many slots to store.

To add a component then you call `DG_ComponentManager::AddComponent<YourClassType>();`

``` cpp
YourClassType* Component = YourEntity->AddComponent<YourClassType>();
```


This function returns the pointer to the first available component in memory and associates it with the entity its called from.

To remove the component it's very similar as adding one:
``` cpp
YourEntity->RemoveComponent<YourClassType>(Component->GetComponentID_ui64());
```

This function doesn't actually deallocate memory, it simply tells the ECS to not use this component and deassociates the EntityID to a 0. 

Soon I want to add a mechanicsm to sort the vector by its Use flag. I will probably remove the UUID parameter and just make it a const auto& Component.

Calling `DG_ComponentManager::Update()` will call every used component in memory by order of type. Soon I want to add a way for the user to define an order of Update.

That's it for now.

## License

[MIT](https://choosealicense.com/licenses/mit/)

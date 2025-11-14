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
Create the EntityManager and the ComponentManager. ComponentManager needs the maximum number of entities to optimize the lookups for entity ID.
``` cpp
DG_EntityManager entityManager(NUM_ENTITIES);
DG_ComponentManager componentManager(NUM_COMPONENTS, NUM_ENTITIES);
```

Create an entity by calling `entityManager.CreateEntity();`:
``` cpp
Entity* YourEntity = entityManager.CreateEntity()
```
This function returns a pointer to an Entity.

To add a component first you need to register them.
This is done by calling `componentManager.RegisterComponent<YourComponent>();` Your Component needs to inherit from DG_Component.

This function returns nothing, it simply allocates memory for the components.
 
If you try to call it twice for the same class of component, it exits since it has already allocated the memory.
To add a component then you do:
``` cpp
YourComponent* component = componentManager.AddComponent<YourComponent>(YourEntity);
```


To remove the component it's very similar as adding one:
``` cpp
componentManager.RemoveComponent<YourComponent>(component);
```


This function doesn't actually deallocate memory, the ComponentManager reuses components.

You can also remove all the components associated with an entity:
``` cpp
componentManager.RemoveComponents<YourComponent>(YourEntity);
```

The stress test with 5000 entities and 10000 components produced these results on my machine:
Time(ms) :
    Entity creation : 0
    Add components : 6
    Access / modify components : 0
    Remove single components : 6
    Remove remaining components : 4

## License

[MIT](https://choosealicense.com/licenses/mit/)

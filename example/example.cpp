// STRESS TEST
//Created 5000 entities.
//Modified 10000 components.
//Time(ms) :
//    Entity creation : 1
//    Add components : 5
//    Access / modify components : 3
//    Remove single components : 2
//    Remove remaining components : 2
#include <iostream>
#include <chrono>
#include <vector>
#include "../include/DogoECS.h"

using namespace DogoECS;

struct Position : public DG_Component { float x = 0, y = 0, z = 0; };
struct Velocity : public DG_Component { float vx = 0, vy = 0, vz = 0; };

int main()
{
    constexpr size_t NUM_ENTITIES = 5000;
    constexpr size_t NUM_COMPONENTS = 10000;

    DG_EntityManager entityManager(NUM_ENTITIES);
    DG_ComponentManager componentManager(NUM_COMPONENTS, NUM_ENTITIES);

    componentManager.RegisterComponent<Position>();
    componentManager.RegisterComponent<Velocity>();

    std::vector<Entity*> allEntities;

    auto start = std::chrono::high_resolution_clock::now();

    // Create entities
    for (size_t i = 0; i < NUM_ENTITIES; i++)
        allEntities.push_back(entityManager.CreateEntity());

    auto mid1 = std::chrono::high_resolution_clock::now();

    // Add components
    for (auto* e : allEntities) {
        componentManager.AddComponent<Position>(e);
        componentManager.AddComponent<Velocity>(e);
    }

    auto mid2 = std::chrono::high_resolution_clock::now();

    // access/modify all Position components
    size_t count = 0;
    for (auto it = componentManager.AllActiveBegin<Position>();
        it != componentManager.AllActiveEnd<Position>(); ++it)
    {
        it->x = 1.f;
        it->y = 2.f;
        it->z = 3.f;
        count++;
    }

    // Access/modify all Velocity components
    for (auto it = componentManager.AllActiveBegin<Velocity>();
        it != componentManager.AllActiveEnd<Velocity>(); ++it)
    {
        it->vx = 0.1f;
        it->vy = 0.2f;
        it->vz = 0.3f;
        count++;
    }
    auto mid3 = std::chrono::high_resolution_clock::now();
    //remove components by entity
    for (size_t i = 0; i < NUM_ENTITIES; i++) {
        componentManager.RemoveComponents(allEntities[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Created " << NUM_ENTITIES << " entities.\n";
    std::cout << "Modified " << count << " components.\n";

    std::cout << "Time (ms):\n";
    std::cout << "  Entity creation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid1 - start).count() << "\n";
    std::cout << "  Add components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid2 - mid1).count() << "\n";
    std::cout << "  Access/modify components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid3 - mid2).count() << "\n";
    std::cout << "  Remove remaining components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - mid3).count() << "\n";

    return 0;
}

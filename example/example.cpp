// STRESS TEST
//Created 5000 entities.
//Modified 10000 components.
//Time(ms) :
//    Entity creation : 0
//    Add components : 6
//    Access / modify components : 0
//    Remove single components : 6
//    Remove remaining components : 4
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

    //Create entities
    for (size_t i = 0; i < NUM_ENTITIES; i++)
        allEntities.push_back(entityManager.CreateEntity());

    auto mid1 = std::chrono::high_resolution_clock::now();

    // Add components
    std::vector<Position*> allPositions;
    std::vector<Velocity*> allVelocities;
    for (auto* e : allEntities) {
        allPositions.push_back(componentManager.AddComponent<Position>(e));
        allVelocities.push_back(componentManager.AddComponent<Velocity>(e));
    }

    auto mid2 = std::chrono::high_resolution_clock::now();

   //Modify components
    size_t count = 0;
    for (auto* pos : allPositions) {
        pos->x = 1.f; pos->y = 2.f; pos->z = 3.f;
        count++;
    }
    for (auto* vel : allVelocities) {
        vel->vx = 0.1f; vel->vy = 0.2f; vel->vz = 0.3f;
        count++;
    }

    auto mid3 = std::chrono::high_resolution_clock::now();

    // Remove half the components individually
    for (size_t i = 0; i < NUM_ENTITIES; i += 2) {
        componentManager.RemoveComponent(allPositions[i]);
        componentManager.RemoveComponent(allVelocities[i]);
    }

    auto mid4 = std::chrono::high_resolution_clock::now();

    // remove remaining components by entity
    for (size_t i = 1; i < NUM_ENTITIES; i += 2) {
        componentManager.RemoveComponents<Position>(allEntities[i]);
        componentManager.RemoveComponents<Velocity>(allEntities[i]);
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
    std::cout << "  Remove single components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid4 - mid3).count() << "\n";
    std::cout << "  Remove remaining components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - mid4).count() << "\n";

    return 0;
}

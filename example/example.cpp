// STRESS TEST
//Created 5000 entities.Modified 10000 components.Time(ms) : 
// Entity creation : 1 
// Add components : 8 
// Access / modify components : 4 
// Remove components : 3

#include <iostream>
#include <vector>
#include <chrono>
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

    std::vector<DG_EntityManager::Entity*> allEntities;

    auto start = std::chrono::high_resolution_clock::now();

    // Create entities
    for (size_t i = 0; i < NUM_ENTITIES; i++)
        allEntities.push_back(entityManager.CreateEntity());

    auto mid1 = std::chrono::high_resolution_clock::now();

    // Add components
    for (auto* e : allEntities)
    {
        componentManager.AddComponent<Position>(e->GetID_ui64());
        componentManager.AddComponent<Velocity>(e->GetID_ui64());
    }

    auto mid2 = std::chrono::high_resolution_clock::now();

    //Access and modify components
    size_t modifiedCount = 0;

    auto posTracker = componentManager.GetTracker<Position>();
    for (auto it = posTracker->begin(); it != posTracker->end(); ++it)
    {
        it->x = 1.0f; it->y = 2.0f; it->z = 3.0f;
        modifiedCount++;
    }

    auto velTracker = componentManager.GetTracker<Velocity>();
    for (auto it = velTracker->begin(); it != velTracker->end(); ++it)
    {
        it->vx = 0.1f; it->vy = 0.2f; it->vz = 0.3f;
        modifiedCount++;
    }

    auto mid3 = std::chrono::high_resolution_clock::now();

    //remove components
    for (auto* e : allEntities)
    {
        posTracker->RemoveComponents(e->GetID_ui64());
        velTracker->RemoveComponents(e->GetID_ui64());
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Created " << NUM_ENTITIES << " entities.\n";
    std::cout << "Modified " << modifiedCount << " components.\n";
    std::cout << "Time (ms):\n";
    std::cout << "  Entity creation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid1 - start).count() << "\n";
    std::cout << "  Add components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid2 - mid1).count() << "\n";
    std::cout << "  Access/modify components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(mid3 - mid2).count() << "\n";
    std::cout << "  Remove components: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - mid3).count() << "\n";

    return 0;
}

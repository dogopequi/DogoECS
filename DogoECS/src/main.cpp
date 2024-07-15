#include "dgpch.h"

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

class UUID
{
public:
    UUID() : m_Integer(s_UniformDistribution(s_Engine))
    {

    }
    UUID(uint64_t id) : m_Integer(id)
    {

    }
    uint64_t m_Integer;
};

class Component
{
public:
    Component() : m_ComponentID(UUID()) {}
    Component(uint64_t id) : m_EntityID(id), m_ComponentID(UUID()) {}
    uint64_t m_EntityID;
    bool inUse;
private:
    UUID m_ComponentID;
};

class TransformComponent : public Component
{
public:
    TransformComponent(uint64_t id) : Component(id) {}
    TransformComponent() : Component() {}

    float x, y, z;

};
class AudioComponent : public Component
{
public:
    AudioComponent(uint64_t id) : Component(id) {}
    AudioComponent() : Component() {}
    std::string name;
};


class ComponentManager
{
public:
    ComponentManager()
    {
        m_TransformComponentArray.resize(MAX_COMPONENTS);
        m_AudioComponentArray.resize(MAX_COMPONENTS);
        for (int i = 0; i < MAX_COMPONENTS; i++)
        {
            m_TransformComponentArray[i] = TransformComponent();
            m_TransformComponentArray[i].inUse = false;
            /* m_TransformComponentMap[m_TransformComponentArray[i].m_EntityID] = &m_TransformComponentArray[i];*/

            m_AudioComponentArray[i] = AudioComponent();
            m_AudioComponentArray[i].inUse = false;
            /*m_AudioComponentMap[m_AudioComponentArray[i].m_EntityID] = &m_AudioComponentArray[i];*/
        }
    }

    void UpdateTransform()
    {
        int32_t i = 0;
        for (auto& x : m_TransformComponentArray)
        {
            std::cout << "I: " << i << std::endl;
            std::cout << "last TC: " << m_TC_LastUsed << std::endl;
            if (i > m_TC_LastUsed)
            {
                std::cout << "breaking" << std::endl;
                break;
            }
            i++;
            if (!x.inUse)
                continue;
            std::cout << "X: " << x.x << std::endl;
            std::cout << "Y: " << x.y << std::endl;
            std::cout << "Z: " << x.z << std::endl;
            std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&x) << std::endl;
        }
    }

    void UpdateAudio()
    {
        int32_t i = 0;
        for (auto& x : m_AudioComponentArray)
        {
            std::cout << "I: " << i << std::endl;
            std::cout << "last AC: " << m_AC_LastUsed << std::endl;
            if (i > m_AC_LastUsed)
                break;
            i++;
            if (!x.inUse)
                continue;
            std::cout << "Name: " << x.name << std::endl;
            std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(&x) << std::endl;
        }
    }

    template <typename T>
    T* AddComponent(uint64_t EnityID);

    template <typename T>
    void RemoveComponent(uint64_t EntityID);


    std::vector<TransformComponent> m_TransformComponentArray{};
    std::unordered_map<uint64_t, TransformComponent*> m_TransformComponentMap{};
    int32_t m_TC_LastUsed = -1;

    std::vector<AudioComponent> m_AudioComponentArray{};
    std::unordered_map<uint64_t, AudioComponent*> m_AudioComponentMap{};
    int32_t m_AC_LastUsed = -1;

    uint32_t MAX_COMPONENTS = 5000;

};

template <>
TransformComponent* ComponentManager::AddComponent<TransformComponent>(uint64_t EntityID)
{
    m_TC_LastUsed++;
    m_TransformComponentArray[m_TC_LastUsed] = TransformComponent(EntityID);
    m_TransformComponentArray[m_TC_LastUsed].inUse = true;
    m_TransformComponentMap[EntityID] = &m_TransformComponentArray[m_TC_LastUsed];

    return m_TransformComponentMap[EntityID];
}
template <>
AudioComponent* ComponentManager::AddComponent<AudioComponent>(uint64_t EntityID)
{
    m_AC_LastUsed++;
    m_AudioComponentArray[m_AC_LastUsed] = AudioComponent(EntityID);
    m_AudioComponentArray[m_AC_LastUsed].inUse = true;
    m_AudioComponentMap[EntityID] = &m_AudioComponentArray[m_AC_LastUsed];

    return m_AudioComponentMap[EntityID];
}

template <>
void ComponentManager::RemoveComponent<TransformComponent>(uint64_t EntityID)
{
    auto it = m_TransformComponentMap.find(EntityID);
    if (it != m_TransformComponentMap.end())
    {
        m_TransformComponentMap.erase(it);
    }

    m_TransformComponentArray[m_AC_LastUsed] = TransformComponent();
    m_TransformComponentArray[m_AC_LastUsed].inUse = false;

    m_TC_LastUsed--;
}

template <>
void ComponentManager::RemoveComponent<AudioComponent>(uint64_t EntityID)
{
    auto it = m_AudioComponentMap.find(EntityID);
    if (it != m_AudioComponentMap.end())
    {
        m_AudioComponentMap.erase(it);
    }

    m_AudioComponentArray[m_AC_LastUsed] = AudioComponent();
    m_AudioComponentArray[m_AC_LastUsed].inUse = false;

    m_AC_LastUsed--;
}

static ComponentManager S_ComponentManager;

class Entity
{
public:
    Entity() { m_EntityID = UUID(); }

    template<typename RETURN>
    RETURN* AddComponent()
    {
        return S_ComponentManager.AddComponent<RETURN>(m_EntityID.m_Integer);
    }

    template<typename RETURN>
    void RemoveComponent()
    {
        S_ComponentManager.RemoveComponent<RETURN>(m_EntityID.m_Integer);
    }

    UUID m_EntityID;
};

class EntityManager
{
public:
    EntityManager()
    {
        for (int i = 0; i < MAX_ENTITIES; i++)
        {
            Entity e;
            m_AvailableEntities.push(e);
        }
    }

    std::optional<Entity> CreateEntity()
    {
        if (m_LivingEntityCount >= MAX_ENTITIES)
        {
            return std::nullopt;
        }
        Entity id = m_AvailableEntities.front();
        m_AvailableEntities.pop();
        ++m_LivingEntityCount;
        return id;
    }

    void DestroyEntity(Entity entity)
    {
        m_AvailableEntities.push(entity);
        m_LivingEntityCount--;
    }

    uint32_t MAX_ENTITIES = 5000;

private:

    std::queue<Entity> m_AvailableEntities{};

    uint32_t m_LivingEntityCount{ 0 };


};

static EntityManager s_EntityManager;

int main()
{
    Entity E1, E2;
    std::optional<Entity> optionalEntity1 = s_EntityManager.CreateEntity();
    std::optional<Entity> optionalEntity2 = s_EntityManager.CreateEntity();

    if (optionalEntity1.has_value()) {
        // Entity creation succeeded, use the entity
        Entity E1 = optionalEntity1.value();
        std::cout << "Entity created successfully.\n";
    }
    if (optionalEntity2.has_value()) {
        // Entity creation succeeded, use the entity
        Entity E2 = optionalEntity2.value();
        std::cout << "Entity created successfully.\n";
    }
    TransformComponent* TCE1 = E1.AddComponent<TransformComponent>();
    TCE1->x = 1.0f;
    TCE1->y = 1.0f;
    TCE1->z = 1.0f;
    TransformComponent* TCE2 = E2.AddComponent<TransformComponent>();
    TCE2->x = 2.0f;
    TCE2->y = 2.0f;
    TCE2->z = 2.0f;
    AudioComponent* ACE1 = E1.AddComponent<AudioComponent>();
    ACE1->name = "audio1";
    AudioComponent* ACE2 = E2.AddComponent<AudioComponent>();
    ACE2->name = "audio2";

    std::cout << "---------------------- 1" << std::endl;

    S_ComponentManager.UpdateAudio();

    S_ComponentManager.UpdateTransform();

    std::cout << "---------------------- 2" << std::endl;

    E1.RemoveComponent<TransformComponent>();

    S_ComponentManager.UpdateAudio();

    S_ComponentManager.UpdateTransform();

    std::cout << "---------------------- 3" << std::endl;
    E2.RemoveComponent<AudioComponent>();
    S_ComponentManager.UpdateAudio();

    S_ComponentManager.UpdateTransform();

    std::cout << "---------------------- 4" << std::endl;

    E2.RemoveComponent<TransformComponent>();

    S_ComponentManager.UpdateAudio();

    S_ComponentManager.UpdateTransform();

    std::cout << "---------------------- 5" << std::endl;
    E1.RemoveComponent<AudioComponent>();
    S_ComponentManager.UpdateAudio();

    S_ComponentManager.UpdateTransform();
}

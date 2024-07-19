#pragma once

class DG_Component
{
public:
    DG_Component() : m_ComponentID(UUID()) {}
    DG_Component(uint64_t id) : m_EntityID(id), m_ComponentID(UUID()) {}

    virtual ~DG_Component() {}

    virtual void Update(){}

    bool GetUse() const { return m_InUse; }
    void SetUse(bool setter) { m_InUse = setter; }

    uint64_t GeEntityID_ui64() const { return m_EntityID; }
    uint64_t GetComponentID_ui64() const { return m_ComponentID.GetUUID_ui64(); }

private:
    bool m_InUse;
    uint64_t m_EntityID;
    UUID m_ComponentID;
};

class TransformComponent : public DG_Component
{
public:
    TransformComponent(uint64_t id) : DG_Component(id) {}
    TransformComponent() : DG_Component() {}

    virtual ~TransformComponent() {}

    void Update() override 
    {
        std::cout << "X: " << x << std::endl;
        std::cout << "Y: " << y << std::endl;
        std::cout << "Z: " << z << std::endl;
        std::cout << "Address of transform: " << std::hex << reinterpret_cast<void*>(this) << std::endl;
    }

    void SetX(float x) { this->x = x; }
    void SetY(float y) { this->y = y; }
    void SetZ(float z) { this->z = z; }

    float GetX() { return x; }
    float GetY() { return y; }
    float GetZ() { return z; }

private:
    float x, y, z;

};
class AudioComponent : public DG_Component
{
public:
    AudioComponent(uint64_t id) : DG_Component(id) {}
    AudioComponent() : DG_Component() {}

    virtual ~AudioComponent() {}

    void Update() override
    {
        std::cout << "Name: " << name << std::endl;
        std::cout << "Address of audio: " << std::hex << reinterpret_cast<void*>(this) << std::endl;
    }

    void SetName(std::string name) { this->name = name; }
    std::string GetName() const { return name; }

private:
    std::string name;
};

#pragma once
#include "DG_Component.h"

namespace DogoECS
{

    class TransformComponent : public DG_Component
    {
    public:
        TransformComponent(uint64_t id) : DG_Component(id) { /*std::cout << "Transform Component created" << std::endl;*/ }
        TransformComponent() : DG_Component() {}
        ~TransformComponent() override {/* std::cout << "Transform Component deleted" << std::endl;*/ }


        void Update() override
        {
            std::cout << "TRANSFORM --------------------------------------------------" << std::endl;
            std::cout << "X: " << x << std::endl;
            std::cout << "Y: " << y << std::endl;
            std::cout << "Z: " << z << std::endl;
            std::cout << "ID: " << m_ComponentID.GetUUID_ui64() << std::endl;
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

    template<>
    REGISTER_COMPONENT_TEMPLATE(TransformComponent)

        template<>
    ADD_COMPONENT_TEMPLATE(TransformComponent)

        template <>
    REMOVE_COMPONENT_TEMPLATE(TransformComponent)


        class AudioComponent : public DG_Component
    {
    public:
        AudioComponent(uint64_t id) : DG_Component(id) {}
        AudioComponent() : DG_Component() {}

        ~AudioComponent() override { }


        void Update() override
        {

            std::cout << "AUDIO --------------------------------------------------- " << std::endl;
            std::cout << "Name: " << name << std::endl;
            std::cout << "ID: " << m_ComponentID.GetUUID_ui64() << std::endl;
        }

        void SetName(std::string name) { this->name = name; }
        std::string GetName() const { return name; }

    private:
        std::string name;
    };

    template<>
    REGISTER_COMPONENT_TEMPLATE(AudioComponent)

        template<>
    ADD_COMPONENT_TEMPLATE(AudioComponent)

        template <>
    REMOVE_COMPONENT_TEMPLATE(AudioComponent)


}
#pragma once
#include <random>
 

namespace DogoECS
{
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

        uint64_t GetUUID_ui64() const { return m_Integer; }
    private:

        uint64_t m_Integer;
    };

}
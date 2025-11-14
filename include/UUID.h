#pragma once
#include <random>
 

namespace DogoECS
{

    class UUID
    {
    public:
        UUID()
        {
            static std::random_device rd;
            static std::mt19937_64 eng(rd());
            static std::uniform_int_distribution<uint64_t> dist(1, UINT64_MAX);
            m_Integer = dist(eng);
        }
        UUID(uint64_t id) : m_Integer(id)
        {

        }

        uint64_t GetUUID_ui64() const { return m_Integer; }
    private:

        uint64_t m_Integer;
    };

}
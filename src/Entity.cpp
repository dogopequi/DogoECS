#include "../include/Entity.h"
namespace DogoECS
{
	std::unique_ptr<DG_EntityManager> DG_EntityManager::s_Instance = nullptr;
	uint32_t DG_EntityManager::MAX_ENTITIES = 5000;

	std::queue<Entity> DG_EntityManager::m_AvailableEntities{};

	uint32_t DG_EntityManager::m_LivingEntityCount = 0;


	DG_EntityManager& DG_EntityManager::GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<DG_EntityManager>(new DG_EntityManager());
		}
		return *s_Instance;
	}
}


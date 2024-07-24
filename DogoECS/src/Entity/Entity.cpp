#include "dgpch.h"
#include "Entity.h"

uint32_t DG_EntityManager::MAX_ENTITIES = 5000;

std::queue<Entity> DG_EntityManager::m_AvailableEntities{};

uint32_t DG_EntityManager::m_LivingEntityCount = 0;

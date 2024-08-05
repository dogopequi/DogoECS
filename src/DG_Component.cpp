#include "../include/DG_Component.h"
namespace DogoECS
{
	std::unique_ptr<DG_ComponentManager> DG_ComponentManager::s_Instance = nullptr;
	uint32_t DG_ComponentManager::MAX_COMPONENTS = 5000;
	std::vector<std::shared_ptr<ComponentTrackerPARENT>> DG_ComponentManager::m_ComponentTrackers;

	std::unordered_map<std::type_index, std::shared_ptr<ComponentTrackerPARENT>> DG_ComponentManager::m_ComponentTrackerMap;
	DG_ComponentManager& DG_ComponentManager::GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<DG_ComponentManager>(new DG_ComponentManager());
		}
		return *s_Instance;
	}
}
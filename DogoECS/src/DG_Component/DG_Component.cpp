#include "dgpch.h"
#include "DG_Component.h"

uint32_t DG_ComponentManager::MAX_COMPONENTS = 5000;
std::vector<std::shared_ptr<ComponentTracker>> DG_ComponentManager::m_ComponentTrackers;

std::unordered_map<std::type_index, std::shared_ptr<ComponentTracker>> DG_ComponentManager::m_ComponentTrackerMap;
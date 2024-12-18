#pragma once
#include "UUID.h"
#include "Entity.h"
#include "DG_Component.h"
namespace DogoECS
{
	void Init()
	{
		DG_EntityManager::GetInstance();
		DG_ComponentManager::GetInstance();
	}
}
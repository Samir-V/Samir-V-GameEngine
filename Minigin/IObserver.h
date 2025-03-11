#pragma once

#include "EventType.h"

namespace dae
{
	class GameObject;

	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(EventType event, GameObject* observedGameObject) = 0;
	};
}


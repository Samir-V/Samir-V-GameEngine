#pragma once

#include "EventType.h"

namespace dae
{
	class GameObject;

	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(const Event& event, GameObject* observedGameObject) = 0;
	};
}


#pragma once
#include <memory>

namespace dae
{
	class GameObject;

	class ComponentBase
	{
	public:

		ComponentBase() = default;
		virtual ~ComponentBase() = default;

		ComponentBase(const ComponentBase& other) = delete;
		ComponentBase(ComponentBase&& other) = default;
		ComponentBase& operator=(const ComponentBase& other) = delete;
		ComponentBase& operator=(ComponentBase&& other) = default;

		virtual void Update(float elapsedSec) = 0;
		virtual void FixedUpdate(float fixedTimeStep) = 0;
		virtual void Render() const = 0;

		virtual void SetPosition(float x, float y) = 0;
		virtual void SetOwner(std::weak_ptr<GameObject> owner) = 0;
	};
}


#pragma once

namespace dae
{
	class GameObject;

	class ComponentBase
	{
	public:
		virtual ~ComponentBase() = default;

		ComponentBase(const ComponentBase& other) = default;
		ComponentBase(ComponentBase&& other) = default;
		ComponentBase& operator=(const ComponentBase& other) = default;
		ComponentBase& operator=(ComponentBase&& other) = default;

		virtual void Update(float elapsedSec) = 0;
		virtual void FixedUpdate(float fixedTimeStep) = 0;
		virtual void Render() const = 0; // Not every component will implement the Render Function

		virtual void SetPosition(float x, float y) = 0;
		virtual void SetOwner(const GameObject* owner) = 0;

		virtual void Destroy() = 0;
		virtual bool GetMarkedToDestroy() const = 0;

	protected:
		ComponentBase() = default;
	};
}


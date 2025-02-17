#pragma once

namespace dae
{
	class GameObject;

	class ComponentBase
	{
	public:
		virtual ~ComponentBase() = default;

		ComponentBase(const ComponentBase& other) = delete;
		ComponentBase(ComponentBase&& other) = delete;
		ComponentBase& operator=(const ComponentBase& other) = delete;
		ComponentBase& operator=(ComponentBase&& other) = delete;

		virtual void Update(float elapsedSec) = 0;
		virtual void FixedUpdate(float fixedTimeStep) = 0;
		virtual void LateUpdate(float elapsedSec) = 0;
		virtual void Render() const = 0; // Not every component will implement the Render Function

		virtual void SetPosition(float x, float y) = 0;
		virtual void Destroy() = 0;
		virtual bool GetMarkedToDestroy() const = 0;

	protected:
		explicit ComponentBase(GameObject* ownerPtr) : m_OwnerPtr(ownerPtr) {}
		GameObject* GetOwner() const { return m_OwnerPtr; }

	private:
		GameObject* m_OwnerPtr;
	};
}


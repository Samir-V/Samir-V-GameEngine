#pragma once

namespace dae
{
	class GameObject;
	// Add the identifier method that allows to get the component by string.
	class ComponentBase
	{
	public:
		virtual ~ComponentBase() = default;

		ComponentBase(const ComponentBase& other) = delete;
		ComponentBase(ComponentBase&& other) = delete;
		ComponentBase& operator=(const ComponentBase& other) = delete;
		ComponentBase& operator=(ComponentBase&& other) = delete;

		virtual void Update(float elapsedSec) = 0;
		virtual void LateUpdate(float elapsedSec) = 0;
		virtual void Render() const = 0; // Not every component will implement the Render Function

		virtual void SetLocalPosition(float x, float y) = 0;
		virtual void Destroy() { m_MarkedToDestroy = true; }
		virtual bool IsMarkedToDestroy() const { return m_MarkedToDestroy; }

	protected:
		explicit ComponentBase(GameObject* ownerPtr) : m_OwnerPtr(ownerPtr), m_MarkedToDestroy(false) {}
		GameObject* GetOwner() const { return m_OwnerPtr; }

	private:
		GameObject* m_OwnerPtr;
		bool m_MarkedToDestroy;
	};
}


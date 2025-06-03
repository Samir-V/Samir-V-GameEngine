#pragma once
#include "Transform.h"

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

		virtual void Start() = 0;
		virtual void Update(float elapsedSec) = 0;
		virtual void LateUpdate(float elapsedSec) = 0;
		virtual void Render() const = 0; // Not every component will implement the Render Function

		virtual void SetLocalPosition(float x, float y) { m_LocalTransform.SetPosition(x, y, 0.0f); }
		virtual void Destroy() { m_MarkedToDestroy = true; }
		virtual bool IsMarkedToDestroy() const { return m_MarkedToDestroy; }
		virtual void SetIsActive(bool newIsActive) { m_IsActive = newIsActive; }
		virtual bool IsActive() const { return m_IsActive; }
		virtual const Transform& GetLocalTransform() const { return m_LocalTransform; }

	protected:
		explicit ComponentBase(GameObject* ownerPtr) : m_OwnerPtr(ownerPtr), m_LocalTransform{}, m_MarkedToDestroy(false), m_IsActive(true) {}
		GameObject* GetOwner() const { return m_OwnerPtr; }

	private:
		GameObject* m_OwnerPtr;
		Transform m_LocalTransform;
		bool m_MarkedToDestroy;
		bool m_IsActive;
	};
}


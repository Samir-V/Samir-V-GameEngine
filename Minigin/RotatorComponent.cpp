#include "RotatorComponent.h"
#include "GameObject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/rotate_vector.hpp>

dae::RotatorComponent::RotatorComponent(GameObject* ownerPtr, const glm::vec3& rotPoint, float angularSpeed, float radius):
	ComponentBase(ownerPtr),
	m_RotationPoint	{rotPoint},
	m_AngularSpeed	{angularSpeed},
	m_Radius		{radius},
	m_Angle			{ }
{
}

void dae::RotatorComponent::Render() const
{
	
}

void dae::RotatorComponent::Update(float elapsedSec)
{
	m_Angle += m_AngularSpeed * elapsedSec;

	const glm::vec3 newPos{ m_RotationPoint.x + m_Radius * cos(m_Angle), m_RotationPoint.y + m_Radius * sin(m_Angle), 0.0f };

	GetOwner()->SetLocalPosition(newPos);
}

void dae::RotatorComponent::LateUpdate(float)
{
	
}

void dae::RotatorComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}


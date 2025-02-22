#include "RotatorComponent.h"
#include "GameObject.h"

dae::RotatorComponent::RotatorComponent(GameObject* ownerPtr, const glm::vec3& rotPoint, float angularSpeed):
	ComponentBase(ownerPtr),
	m_RotationPoint	{rotPoint},
	m_AngularSpeed	{angularSpeed},
	m_Radius		{ },
	m_Angle			{ }
{
	if (GetOwner()->GetParent() == nullptr)
	{
		const glm::vec3 ownerWorldPos = GetOwner()->GetWorldTransform().GetPosition();
		m_Radius = glm::distance(m_RotationPoint, ownerWorldPos);
	}
	else
	{
		const glm::vec3 ownerWorldPos = GetOwner()->GetWorldTransform().GetPosition();
		const glm::vec3 parentWorldPos = GetOwner()->GetParent()->GetWorldTransform().GetPosition();
		m_Radius = glm::distance(parentWorldPos + m_RotationPoint, ownerWorldPos);
	}
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

void dae::RotatorComponent::SetRotationPoint(float x, float y)
{
	m_RotationPoint = glm::vec3(x, y, 0.0f);
}

void dae::RotatorComponent::SetRotationPoint(const glm::vec3& pos)
{
	m_RotationPoint = pos;
}



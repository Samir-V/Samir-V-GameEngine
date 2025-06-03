#include "RotatorComponent.h"
#include "GameObject.h"

dae::RotatorComponent::RotatorComponent(GameObject* ownerPtr, float radius, float angularSpeed):
	ComponentBase(ownerPtr),
	m_AngularSpeed	{angularSpeed},
	m_Radius		{ radius },
	m_Angle			{ }
{

}

void dae::RotatorComponent::Render() const
{
}

void dae::RotatorComponent::Start()
{
}


void dae::RotatorComponent::Update(float elapsedSec)
{
	m_Angle += m_AngularSpeed * elapsedSec;

	const glm::vec3 newPos{m_Radius * cos(m_Angle), m_Radius * sin(m_Angle), 0.0f };

	GetOwner()->SetLocalPosition(newPos);
}

void dae::RotatorComponent::LateUpdate(float)
{
}


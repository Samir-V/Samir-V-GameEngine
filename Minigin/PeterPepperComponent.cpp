#include "PeterPepperComponent.h"

#include "GameObject.h"

dae::PeterPepperComponent::PeterPepperComponent(GameObject* ownerPtr, float maxSpeed):
	ComponentBase(ownerPtr),
	m_MaxSpeed(maxSpeed)
{
}

void dae::PeterPepperComponent::Update(float elapsedSec)
{
	const auto pepperPos = GetOwner()->GetWorldTransform().GetPosition();

	const auto newPepperPosX = pepperPos.x + m_Velocity.x * elapsedSec;
	const auto newPepperPosY = pepperPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetLocalPosition(newPepperPosX, newPepperPosY);

	//m_Velocity = glm::vec2{};
}

void dae::PeterPepperComponent::LateUpdate(float)
{
	
}

void dae::PeterPepperComponent::Render() const
{
	
}

void dae::PeterPepperComponent::RenderImGui()
{
	
}

void dae::PeterPepperComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::PeterPepperComponent::SetXVelocity(float x)
{
	m_Velocity.x = x;
}

void dae::PeterPepperComponent::SetYVelocity(float y)
{
	m_Velocity.y = y;
}

float dae::PeterPepperComponent::GetMaxSpeed() const
{
	return m_MaxSpeed;
}








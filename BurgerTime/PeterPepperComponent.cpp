#include "PeterPepperComponent.h"

#include <algorithm>
#include <iostream>

#include "GameObject.h"

dae::PeterPepperComponent::PeterPepperComponent(GameObject* ownerPtr, float maxSpeed):
	ComponentBase(ownerPtr),
	m_MaxSpeed(maxSpeed)
{
	m_ObjectDeathEvent = std::make_unique<Subject>();
	m_EnemyKilledEvent = std::make_unique<Subject>();
}

void dae::PeterPepperComponent::Update(float elapsedSec)
{
	m_Velocity = m_Direction * m_MaxSpeed;

	m_Direction = glm::vec2{ 0.0f, 0.0f };

	const auto pepperPos = GetOwner()->GetWorldTransform().GetPosition();

	const auto newPepperPosX = pepperPos.x + m_Velocity.x * elapsedSec;
	const auto newPepperPosY = pepperPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetLocalPosition(newPepperPosX, newPepperPosY);
}

void dae::PeterPepperComponent::LateUpdate(float)
{
}

void dae::PeterPepperComponent::Render() const
{
}

void dae::PeterPepperComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::PeterPepperComponent::AddInputToDirection(const glm::vec2& direction)
{
	m_Direction += direction;

	if (glm::dot(m_Direction, m_Direction) > std::numeric_limits<float>::epsilon())
	{
		m_Direction = glm::normalize(m_Direction);
	}
}

dae::Subject* dae::PeterPepperComponent::GetObjectDeathEvent() const
{
	return m_ObjectDeathEvent.get();
}

int dae::PeterPepperComponent::GetRemainingHealth() const
{
	return m_Health;
}

void dae::PeterPepperComponent::Damage(int damage)
{
	m_Health -= damage;

	m_Health = std::max(m_Health, 0);

	m_ObjectDeathEvent->NotifyObservers(Event(make_sdbm_hash("PlayerDamaged")), GetOwner());
}


dae::Subject* dae::PeterPepperComponent::GetEnemyKilledEvent() const
{
	return m_EnemyKilledEvent.get();
}

void dae::PeterPepperComponent::Notify(const Event& event, GameObject*)
{
	/*if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		std::cout << "Colliding! \n";
	}*/

	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		std::cout << "Entered collision! \n";
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		std::cout << "Exited collision! \n";
	}
}







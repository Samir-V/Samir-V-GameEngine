#include "PeterPepperComponent.h"

#include <algorithm>

#include "GameObject.h"

dae::PeterPepperComponent::PeterPepperComponent(GameObject* ownerPtr):
	ComponentBase(ownerPtr)
{
	m_ObjectDeathEvent = std::make_unique<Subject>();
	m_EnemyKilledEvent = std::make_unique<Subject>();
}

void dae::PeterPepperComponent::Start()
{
	m_State = std::make_unique<WalkingState>();
	m_State->OnEnter(GetOwner());
}

void dae::PeterPepperComponent::Update(float elapsedSec)
{
	auto newState = m_State->Update(GetOwner(), elapsedSec);

	if (newState != nullptr)
	{
		ChangeState(std::move(newState));
	}
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

void dae::PeterPepperComponent::ChangeState(std::unique_ptr<PeterPepperState> newState)
{
	m_State->OnExit(GetOwner());

	m_State = std::move(newState);

	m_State->OnEnter(GetOwner());
}






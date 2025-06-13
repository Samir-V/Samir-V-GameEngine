#include "PeterPepperComponent.h"

#include <algorithm>

#include "GameObject.h"

dae::PeterPepperComponent::PeterPepperComponent(GameObject* ownerPtr):
	ComponentBase(ownerPtr)
	, m_PlayerDiedEvent{std::make_unique<Subject>()}
	, m_PepperAmountChangedEvent{std::make_unique<Subject>()}
	, m_State{std::make_unique<WalkingState>()}
	, m_Peppers{5}
	, m_Lives{3}
{
	m_State->OnEnter(GetOwner());
}

void dae::PeterPepperComponent::Start()
{
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

dae::Subject* dae::PeterPepperComponent::GetPlayerDiedEvent() const
{
	return m_PlayerDiedEvent.get();
}

dae::Subject* dae::PeterPepperComponent::GetPepperAmountChangedEvent() const
{
	return m_PepperAmountChangedEvent.get();
}


int dae::PeterPepperComponent::GetRemainingLives() const
{
	return m_Lives;
}

void dae::PeterPepperComponent::DecreaseLives(int amount)
{
	m_Lives -= amount;
	m_Lives = std::max(m_Lives, 0);
}

void dae::PeterPepperComponent::IncreasePepper(int amount)
{
	const int deltaPepper = std::clamp(amount, 0, 5);
	m_Peppers += deltaPepper;
}


void dae::PeterPepperComponent::ChangeState(std::unique_ptr<PeterPepperState> newState)
{
	if (newState && typeid(*m_State) == typeid(*newState))
	{
		return;
	}

	m_State->OnExit(GetOwner());

	m_State = std::move(newState);

	m_State->OnEnter(GetOwner());
}

void dae::PeterPepperComponent::AssertVictory()
{
	ChangeState(std::make_unique<WinningState>());
}

void dae::PeterPepperComponent::Resurrect()
{
	if (typeid(*m_State) == typeid(DyingState))
	{
		ChangeState(std::make_unique<DeadState>());
	}

	ChangeState(std::make_unique<WalkingState>());
}

void dae::PeterPepperComponent::SprayPepper()
{
	if (typeid(*m_State) != typeid(WalkingState))
	{
		return;
	}

	if (m_Peppers > 0)
	{
		m_Peppers -= 1;
		ChangeState(std::make_unique<SprayingState>());
	}
}

void dae::PeterPepperComponent::FullRespawn()
{
	ChangeState(std::make_unique<WalkingState>());
	m_Peppers = 5;
	m_Lives = 3;

	m_PlayerDiedEvent->NotifyObservers(Event(make_sdbm_hash("PlayerRespawned")), GetOwner());
	m_PepperAmountChangedEvent->NotifyObservers(Event(make_sdbm_hash("PepperAmountChanged")), GetOwner());
}

int dae::PeterPepperComponent::GetRemainingPeppers() const
{
	return m_Peppers;
}









#include "PeterPepperComponent.h"

#include <algorithm>

#include "GameObject.h"

svengine::PeterPepperComponent::PeterPepperComponent(GameObject* ownerPtr):
	ComponentBase(ownerPtr)
	, m_PlayerDiedEvent{std::make_unique<Subject>()}
	, m_PepperAmountChangedEvent{std::make_unique<Subject>()}
	, m_State{std::make_unique<WalkingState>()}
	, m_Peppers{5}
	, m_Lives{4}
{
	m_State->OnEnter(GetOwner());
}

void svengine::PeterPepperComponent::Start()
{
}

void svengine::PeterPepperComponent::Update(float elapsedSec)
{
	auto newState = m_State->Update(GetOwner(), elapsedSec);

	if (newState != nullptr)
	{
		ChangeState(std::move(newState));
	}
}

void svengine::PeterPepperComponent::LateUpdate(float)
{
}

void svengine::PeterPepperComponent::Render() const
{
}

svengine::Subject* svengine::PeterPepperComponent::GetPlayerDiedEvent() const
{
	return m_PlayerDiedEvent.get();
}

svengine::Subject* svengine::PeterPepperComponent::GetPepperAmountChangedEvent() const
{
	return m_PepperAmountChangedEvent.get();
}


int svengine::PeterPepperComponent::GetRemainingLives() const
{
	return m_Lives;
}

void svengine::PeterPepperComponent::DecreaseLives(int amount)
{
	m_Lives -= amount;
	m_Lives = std::max(m_Lives, 0);
}

void svengine::PeterPepperComponent::IncreasePepper(int amount)
{
	const int deltaPepper = std::clamp(amount, 0, 5);
	m_Peppers += deltaPepper;
}


void svengine::PeterPepperComponent::ChangeState(std::unique_ptr<PeterPepperState> newState)
{
	if (newState && typeid(*m_State) == typeid(*newState))
	{
		return;
	}

	m_State->OnExit(GetOwner());

	m_State = std::move(newState);

	m_State->OnEnter(GetOwner());
}

void svengine::PeterPepperComponent::AssertVictory()
{
	ChangeState(std::make_unique<WinningState>());
}

void svengine::PeterPepperComponent::Resurrect()
{
	if (typeid(*m_State) == typeid(DyingState))
	{
		ChangeState(std::make_unique<DeadState>());
	}

	ChangeState(std::make_unique<WalkingState>());
}

void svengine::PeterPepperComponent::SprayPepper()
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

void svengine::PeterPepperComponent::FullRespawn()
{
	ChangeState(std::make_unique<WalkingState>());
	m_Peppers = 5;
	m_Lives = 4;

	m_PlayerDiedEvent->NotifyObservers(Event(make_sdbm_hash("PlayerRespawned")), GetOwner());
	m_PepperAmountChangedEvent->NotifyObservers(Event(make_sdbm_hash("PepperAmountChanged")), GetOwner());
}

int svengine::PeterPepperComponent::GetRemainingPeppers() const
{
	return m_Peppers;
}









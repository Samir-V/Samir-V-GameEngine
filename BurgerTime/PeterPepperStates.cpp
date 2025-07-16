#include "PeterPepperStates.h"

#include "PeterPepperComponent.h"

void svengine::WalkingState::OnEnter(GameObject* peterPepperObject)
{
	m_MoveComponentPtr = peterPepperObject->GetComponent<MoveComponent>();
	m_SpritesheetComponentPtr = peterPepperObject->GetComponent<SpritesheetComponent>();
}

std::unique_ptr<svengine::PeterPepperState> svengine::WalkingState::Update(GameObject*, float)
{
	const auto velocity = m_MoveComponentPtr->GetVelocity();
	const auto lastNonZeroDirection = m_MoveComponentPtr->GetLastDirection();

	if (velocity.x < 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingLeft"));
	}
	else if (velocity.x > 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingRight"));
	}
	else if (velocity.y > 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingDown"));
	}
	else if (velocity.y < 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingUp"));
	}
	else
	{
		if (lastNonZeroDirection.y < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPIdleUp"));
		}
		else
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPIdleDown"));
		}
	}

	return nullptr;
}

void svengine::WalkingState::OnExit(GameObject*)
{
}


void svengine::SprayingState::OnEnter(GameObject* peterPepperObject)
{
	peterPepperObject->GetComponent<PeterPepperComponent>()->GetPepperAmountChangedEvent()->NotifyObservers(Event(make_sdbm_hash("PepperAmountChanged")), peterPepperObject);

	const auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
	m_LastDirection = moveComponent->GetLastDirection();

	moveComponent->SetDirection(glm::vec2(0.0f, 0.0f));
	moveComponent->SetIsActive(false);

	m_SpritesheetComponentPtr = peterPepperObject->GetComponent<SpritesheetComponent>();

	// Here also needed to get the child object of PeterPepper and activate it, which is the pepper spray object
	const auto it = std::ranges::find_if(peterPepperObject->GetChildrenVector(), [](GameObject* child)
		{
			return child->GetTag() == make_sdbm_hash("PepperSpray");
		});

	m_PepperSprayObjectPtr = *it;

	assert(m_PepperSprayObjectPtr != nullptr);

	m_PepperSprayObjectPtr->SetIsActive(true);

	const auto spraySpriteSheetComp = m_PepperSprayObjectPtr->GetComponent<SpritesheetComponent>();
	spraySpriteSheetComp->ResetSpriteTiming();

	if (m_LastDirection.y > 0)
	{
		spraySpriteSheetComp->Play(make_sdbm_hash("SprayDown"));
		m_PepperSprayObjectPtr->SetLocalPosition(0.0f, 16.0f);
	}
	else if (m_LastDirection.y < 0)
	{
		spraySpriteSheetComp->Play(make_sdbm_hash("SprayUp"));
		m_PepperSprayObjectPtr->SetLocalPosition(0.0f, -16.0f);
	}
	else if (m_LastDirection.x < 0)
	{
		spraySpriteSheetComp->Play(make_sdbm_hash("SpraySideways"));
		m_PepperSprayObjectPtr->SetLocalPosition(-16.0f, 0.0f);
	}
	else if (m_LastDirection.x > 0)
	{
		spraySpriteSheetComp->Play(make_sdbm_hash("SpraySideways"));
		m_PepperSprayObjectPtr->SetLocalPosition(16.0f, 0.0f);
	}


	auto& sound = ServiceLocator::GetSoundSystem();
	sound.Play("PepperShake.wav", 0.8f);
}

std::unique_ptr<svengine::PeterPepperState> svengine::SprayingState::Update(GameObject*, float elapsedSec)
{
	if (m_LastDirection.x < 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPSprayLeft"));
	}
	else if (m_LastDirection.x > 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPSprayRight"));
	}
	else if (m_LastDirection.y > 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPSprayDown"));
	}
	else if (m_LastDirection.y < 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPSprayUp"));
	}

	m_Timer -= elapsedSec;

	if (m_Timer < 0.0f)
	{
		return std::make_unique<WalkingState>();
	}

	return nullptr;
}

void svengine::SprayingState::OnExit(GameObject* peterPepperObject)
{
	const auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
	moveComponent->SetIsActive(true);

	// Here also needed to get the child object of PeterPepper and deactivate it, which is the pepper spray object
	m_PepperSprayObjectPtr->SetIsActive(false);
}


void svengine::WinningState::OnEnter(GameObject* peterPepperObject)
{
	const auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();

	moveComponent->SetDirection(glm::vec2(0.0f, 0.0f));
	moveComponent->SetIsActive(false);

	const auto spriteSheetComp = peterPepperObject->GetComponent<SpritesheetComponent>();
	spriteSheetComp->ResetSpriteTiming();

	spriteSheetComp->Play(make_sdbm_hash("PPWin"));
}

std::unique_ptr<svengine::PeterPepperState> svengine::WinningState::Update(GameObject*, float elapsedSec)
{
	m_Timer -= elapsedSec;

	if (m_Timer < 0.0f)
	{
		return std::make_unique<WalkingState>();
	}

	return nullptr;
}

void svengine::WinningState::OnExit(GameObject* peterPepperObject)
{
	const auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
	moveComponent->SetIsActive(true);
}


void svengine::DyingState::OnEnter(GameObject* peterPepperObject)
{
	const auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();

	moveComponent->SetDirection(glm::vec2(0.0f, 0.0f));
	moveComponent->SetIsActive(false);

	const auto spriteSheetComp = peterPepperObject->GetComponent<SpritesheetComponent>();
	spriteSheetComp->ResetSpriteTiming();

	spriteSheetComp->Play(make_sdbm_hash("PPDying"));

	auto& sound = ServiceLocator::GetSoundSystem();
	sound.Play("Death.wav", 0.8f);
}

std::unique_ptr<svengine::PeterPepperState> svengine::DyingState::Update(GameObject*, float elapsedSec)
{
	m_Timer -= elapsedSec;

	if (m_Timer < 0.0f)
	{
		return std::make_unique<DeadState>();
	}

	return nullptr;
}

void svengine::DyingState::OnExit(GameObject* )
{
	
}


void svengine::DeadState::OnEnter(GameObject* peterPepperObject)
{
	peterPepperObject->SetIsActive(false);
	const auto peterPepperComponent = peterPepperObject->GetComponent<PeterPepperComponent>();
	peterPepperComponent->DecreaseLives();
	peterPepperComponent->GetPlayerDiedEvent()->NotifyObservers(Event(make_sdbm_hash("PlayerDied")), peterPepperObject);
}

std::unique_ptr<svengine::PeterPepperState> svengine::DeadState::Update(GameObject*, float)
{
	return nullptr;
}

void svengine::DeadState::OnExit(GameObject* peterPepperObject)
{
	const auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
	moveComponent->SetIsActive(true);
}
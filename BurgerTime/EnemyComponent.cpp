#include "EnemyComponent.h"

#include "RectCollider2DComponent.h"
#include "BurgerPartComponent.h"
#include "EnemyStates.h"
#include "GameObject.h"
#include "EnemyMoveComponent.h"
#include "Utils.h"

dae::EnemyComponent::EnemyComponent(GameObject* ownerPtr, EnemyType enemyType) :
	ComponentBase(ownerPtr)
	, m_EnemyDyingEvent{std::make_unique<Subject>()}
	, m_EnemyType{ enemyType }
{
}

void dae::EnemyComponent::Start()
{
	m_State = std::make_unique<EnemyWalkingState>();
	m_State->OnEnter(GetOwner());

	auto rectColliderComp = GetOwner()->GetComponent<RectCollider2DComponent>();

	rectColliderComp->GetCollisionEnterEvent()->AddObserver(this);
	rectColliderComp->GetCollisionStayEvent()->AddObserver(this);
	rectColliderComp->GetCollisionExitEvent()->AddObserver(this);
}

void dae::EnemyComponent::Update(float elapsedSec)
{
	auto newState = m_State->Update(GetOwner(), elapsedSec);

	if (newState != nullptr)
	{
		ChangeState(std::move(newState));
	}
}

void dae::EnemyComponent::LateUpdate(float)
{
}

void dae::EnemyComponent::Render() const
{
}

void dae::EnemyComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();

			if (burgerComp->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
			{
				switch (m_EnemyType)
				{
				case EnemyType::HotDog:
					m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), observedGameObject);
					break;
				case EnemyType::Pickle:
					m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), observedGameObject);
					break;
				case EnemyType::Egg:
					m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("EggKilled")), observedGameObject);
					break;
				}

				m_CollidedBurgerPartComponents.clear();
				ChangeState(std::make_unique<EnemyDyingState>());
			}
			else
			{
				m_CollidedBurgerPartComponents.insert(burgerComp);
			}
		}

		if (observedGameObject->GetTag() == make_sdbm_hash("PepperSpray"))
		{
			if (typeid(*m_State) != typeid(StunnedState))
			{
				// This algorithm differs since the state needs to be preserved and then returned
				m_State->OnExit(GetOwner());

				auto stunnedState = std::make_unique<StunnedState>(std::move(m_State));

				m_State = std::move(stunnedState);

				m_State->OnEnter(GetOwner());
			}
		}

		auto newState = m_State->OnCollisionEnter(GetOwner(), observedGameObject);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			for (auto burgerPartComponent : m_CollidedBurgerPartComponents)
			{
				if (burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
				{
					switch (m_EnemyType)
					{
					case EnemyType::HotDog:
						m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), observedGameObject);
						break;
					case EnemyType::Pickle:
						m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), observedGameObject);
						break;
					case EnemyType::Egg:
						m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("EggKilled")), observedGameObject);
						break;
					}

					ChangeState(std::make_unique<EnemyDyingState>());
					break;
				}
			}
		}

		auto newState = m_State->OnCollisionStay(GetOwner(), observedGameObject);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();

			m_CollidedBurgerPartComponents.erase(burgerComp);
		}
	}
}

void dae::EnemyComponent::ChangeState(std::unique_ptr<EnemyState> newState)
{
	if (newState && typeid(*m_State) == typeid(*newState))
	{
		return;
	}

	m_State->OnExit(GetOwner());

	m_State = std::move(newState);

	m_State->OnEnter(GetOwner());
}


dae::EnemyState::EnemyType dae::EnemyComponent::GetEnemyType() const
{
	return m_EnemyType;
}

std::type_index dae::EnemyComponent::GetCurrentStateType() const
{
	return typeid(*m_State);
}

void dae::EnemyComponent::StartFalling(GameObject* burgerPart)
{
	switch (m_EnemyType)
	{
	case EnemyType::HotDog:
		m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), burgerPart);
		break;
	case EnemyType::Pickle:
		m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), burgerPart);
		break;
	case EnemyType::Egg:
		m_EnemyDyingEvent->NotifyObservers(Event(make_sdbm_hash("EggKilled")), burgerPart);
		break;
	}

	m_CollidedBurgerPartComponents.clear();
	ChangeState(std::make_unique<FallingState>());
}

dae::Subject* dae::EnemyComponent::GetEnemyDyingEvent() const
{
	return m_EnemyDyingEvent.get();
}

void dae::EnemyComponent::Resurrect()
{
	auto enemyMoveComp = GetOwner()->GetComponent<EnemyMoveComponent>();
	enemyMoveComp->Start();

	ChangeState(std::make_unique<EnemyWalkingState>());
}



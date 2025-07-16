#include "EnemyComponent.h"

#include "RectCollider2DComponent.h"
#include "BurgerPartComponent.h"
#include "EnemyStates.h"
#include "GameObject.h"
#include "EnemyMoveComponent.h"
#include "Utils.h"

svengine::EnemyComponent::EnemyComponent(GameObject* ownerPtr, EnemyType enemyType) :
	ComponentBase(ownerPtr)
	, m_EnemyDyingEvent{std::make_unique<Subject>()}
	, m_EnemyType{ enemyType }
	, m_CollidedBurgerPartComponents{}
{
}

void svengine::EnemyComponent::Start()
{
	m_State = std::make_unique<EnemyWalkingState>();
	m_State->OnEnter(GetOwner());

	const auto rectColliderComp = GetOwner()->GetComponent<RectCollider2DComponent>();

	rectColliderComp->GetCollisionEnterEvent()->AddObserver(this);
	rectColliderComp->GetCollisionStayEvent()->AddObserver(this);
	rectColliderComp->GetCollisionExitEvent()->AddObserver(this);
}

void svengine::EnemyComponent::Update(float elapsedSec)
{
	auto newState = m_State->Update(GetOwner(), elapsedSec);

	if (newState != nullptr)
	{
		ChangeState(std::move(newState));
	}
}

void svengine::EnemyComponent::LateUpdate(float)
{
}

void svengine::EnemyComponent::Render() const
{
}

void svengine::EnemyComponent::Notify(const Event& event, GameObject* gameObjectCausingEvent)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		auto newState = m_State->OnCollisionEnter(GetOwner(), gameObjectCausingEvent);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{

		auto newState = m_State->OnCollisionStay(GetOwner(), gameObjectCausingEvent);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		auto newState = m_State->OnCollisionExit(GetOwner(), gameObjectCausingEvent);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
		}
	}
}

void svengine::EnemyComponent::ChangeState(std::unique_ptr<EnemyState> newState)
{
	if (newState && typeid(*m_State) == typeid(*newState))
	{
		return;
	}

	m_State->OnExit(GetOwner());

	m_State = std::move(newState);

	m_State->OnEnter(GetOwner());
}


svengine::EnemyState::EnemyType svengine::EnemyComponent::GetEnemyType() const
{
	return m_EnemyType;
}

std::type_index svengine::EnemyComponent::GetCurrentStateType() const
{
	return typeid(*m_State);
}

void svengine::EnemyComponent::StartFalling(GameObject* burgerPart)
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
	ChangeState(std::make_unique<EnemyFallingState>());
}

svengine::Subject* svengine::EnemyComponent::GetEnemyDyingEvent() const
{
	return m_EnemyDyingEvent.get();
}

void svengine::EnemyComponent::Resurrect()
{
	const auto enemyMoveComp = GetOwner()->GetComponent<EnemyMoveComponent>();
	enemyMoveComp->Reset();

	ChangeState(std::make_unique<EnemyWalkingState>());
}

std::set<svengine::BurgerPartComponent*>& svengine::EnemyComponent::GetCollidedBurgerPartsRef()
{
	return m_CollidedBurgerPartComponents;
}


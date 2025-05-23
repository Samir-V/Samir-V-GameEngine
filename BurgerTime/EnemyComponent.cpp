#include "EnemyComponent.h"
#include "EnemyStates.h"
#include "Utils.h"

dae::EnemyComponent::EnemyComponent(GameObject* ownerPtr, EnemyType enemyType) : ComponentBase(ownerPtr), m_EnemyType{ enemyType }
{
}

void dae::EnemyComponent::Start()
{
	m_State = std::make_unique<EnemyWalkingState>();
	m_State->OnEnter(GetOwner());
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

void dae::EnemyComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::EnemyComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		auto newState = m_State->OnCollisionEnter(GetOwner(), observedGameObject);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		auto newState = m_State->OnCollisionStay(GetOwner(), observedGameObject);

		if (newState != nullptr)
		{
			ChangeState(std::move(newState));
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





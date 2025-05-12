#include "EnemyComponent.h"
#include "EnemyStates.h"

dae::EnemyComponent::EnemyComponent(GameObject* ownerPtr): ComponentBase(ownerPtr)
{
}

void dae::EnemyComponent::Start()
{
}

void dae::EnemyComponent::Update(float)
{
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


dae::EnemyComponent::EnemyType dae::EnemyComponent::GetEnemyType() const
{
	return m_EnemyType;
}





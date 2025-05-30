#include "EnemyStates.h"

#include <algorithm>
#include <iostream>

#include "EnemyComponent.h"
#include "EnemyMoveComponent.h"
#include "SpritesheetComponent.h"
#include "RectCollider2DComponent.h"
#include "Utils.h"
#include "GameObject.h"

void dae::EnemyWalkingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();
	m_RectCollider2DComponentPtr = enemyObject->GetComponent<RectCollider2DComponent>();

	// Handling animations
	auto direction = m_EnemyMoveComponentPtr->GetDirection();

	switch (m_EnemyComponentPtr->GetEnemyType())
	{
	case EnemyType::Egg:
		if (direction.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingLeft"));
		}
		else if (direction.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingRight"));
		}
		break;
	case EnemyType::HotDog:
		if (direction.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingLeft"));
		}
		else if (direction.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingRight"));
		}
		break;
	case EnemyType::Pickle:
		if (direction.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingLeft"));
		}
		else if (direction.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingRight"));
		}
		break;
	}
}

std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::Update(GameObject*, float)
{
	// Handling Behaviour Logic of the Enemy finding an opportunity to go down

	auto ownerColliderRect = m_RectCollider2DComponentPtr->GetCollisionRect();
	glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
	glm::vec2 rayDirection{ 0.0f, 1.0f };

	auto intersectedGameObjects = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOrigin, rayDirection, 5.0f);

	bool ladderFound{ false };

	for (auto intersectedGameObject : intersectedGameObjects)
	{
		if (intersectedGameObject->GetTag() == make_sdbm_hash("Ladder"))
		{
			ladderFound = true;
			m_LadderBelowPtr = intersectedGameObject;
		}
	}

	if (ladderFound)
	{
		auto directives = m_EnemyMoveComponentPtr->CalculateDirectionDirectives();

		if (directives.verDirective == EnemyMoveComponent::VerticalDirective::Up)
		{
			if (!m_CurrentLadderColliders.empty())
			{
				m_EnemyMoveComponentPtr->SetDirection({ 0.0f, -1.0f });
			}
			else
			{
				m_EnemyMoveComponentPtr->SetDirection({ 0.0f, 1.0f });
			}
		}
		else // Decision is made, in this case if enemy and the player are on the same level - enemy will go down. 
		{
			m_EnemyMoveComponentPtr->SetDirection({ 0.0f, 1.0f });
		}

		return std::make_unique<ClimbingState>();
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::OnCollisionEnter(GameObject*, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
	{
		m_CurrentLadderColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());
	}

	return nullptr;
}


std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
	{
		auto distance = enemyObject->GetWorldTransform().GetPosition().x - observedGameObject->GetWorldTransform().GetPosition().x;

		if ( abs(distance) <= 0.25f )
		{
			auto directives = m_EnemyMoveComponentPtr->CalculateDirectionDirectives();

			if (directives.verDirective == EnemyMoveComponent::VerticalDirective::Down)
			{
				if (m_LadderBelowPtr)
				{
					m_EnemyMoveComponentPtr->SetDirection({ 0.0f, 1.0f });
				}
				else
				{
					m_EnemyMoveComponentPtr->SetDirection({ 0.0f, -1.0f });
				}
			}
			else // Decision is made, in this case if enemy and the player are on the same level - enemy will go up. 
			{
				m_EnemyMoveComponentPtr->SetDirection({ 0.0f, -1.0f });
			}

			return std::make_unique<ClimbingState>();
		}
	}

	return nullptr;
}


// Climbing State


void dae::ClimbingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();
	m_RectCollider2DComponentPtr = enemyObject->GetComponent<RectCollider2DComponent>();

	auto direction = m_EnemyMoveComponentPtr->GetDirection();

	switch (m_EnemyComponentPtr->GetEnemyType())
	{
	case EnemyType::Egg:
		if (direction.y > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingDown"));
		}
		else if (direction.y < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingUp"));
		}
		break;
	case EnemyType::HotDog:
		if (direction.y > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingDown"));
		}
		else if (direction.y < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingUp"));
		}
		break;
	case EnemyType::Pickle:
		if (direction.y > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingDown"));
		}
		else if (direction.y < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingUp"));
		}
		break;
	}
}

std::unique_ptr<dae::EnemyState> dae::ClimbingState::Update(GameObject*, float elapsedSec)
{

	if (m_Timer > 0.0f)
	{
		m_Timer -= elapsedSec;
	}

	// Handling Behaviour Logic of the Enemy

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::ClimbingState::OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Platform"))
	{
		if (m_Timer > 0.0f)
		{
			return nullptr;
		}

		auto ownerColliderRect = m_RectCollider2DComponentPtr->GetCollisionRect();
		glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
		glm::vec2 rayDirection{ 0.0f, 1.0f };
		auto intersectedGameObjects = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOrigin, rayDirection, 2.0f);

		if (std::ranges::none_of(intersectedGameObjects, [](GameObject* gameObject)
			{
				return gameObject->GetTag() == make_sdbm_hash("Ladder");
			}))
		{

			auto overlapShift = RectCollider2DComponent::GetCollisionOverlapShift(ownerColliderRect, observedGameObject->GetComponent<RectCollider2DComponent>()->GetCollisionRect());

			auto& ownerPos = enemyObject->GetWorldTransform().GetPosition();
			enemyObject->SetWorldPosition(ownerPos.x + overlapShift.x, ownerPos.y + overlapShift.y);

			auto directives = m_EnemyMoveComponentPtr->CalculateDirectionDirectives();

			if (directives.horDirective == EnemyMoveComponent::HorizontalDirective::Right)
			{
				m_EnemyMoveComponentPtr->SetDirection({ 1.0f, 0.0f });
			}
			else
			{
				m_EnemyMoveComponentPtr->SetDirection({ -1.0f, 0.0f });
			}

			return std::make_unique<FinishedClimbingState>();
		}
	}
	return nullptr;
}



void dae::FinishedClimbingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();

	// Handling animations
	auto direction = m_EnemyMoveComponentPtr->GetDirection();

	switch (m_EnemyComponentPtr->GetEnemyType())
	{
	case EnemyType::Egg:
		if (direction.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingLeft"));
		}
		else if (direction.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingRight"));
		}
		break;
	case EnemyType::HotDog:
		if (direction.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingLeft"));
		}
		else if (direction.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingRight"));
		}
		break;
	case EnemyType::Pickle:
		if (direction.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingLeft"));
		}
		else if (direction.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingRight"));
		}
		break;
	}
}

std::unique_ptr<dae::EnemyState> dae::FinishedClimbingState::Update(GameObject*, float elapsedSec)
{
	m_Timer -= elapsedSec;

	if (m_Timer < 0.0f)
	{
		return std::make_unique<EnemyWalkingState>();
	}

	return nullptr;
}


dae::StunnedState::StunnedState(std::unique_ptr<EnemyState> previousState)
{
	m_PreviousState = std::move(previousState);
}

void dae::StunnedState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();

	m_EnemyMoveComponentPtr->SetIsActive(false);

	switch (m_EnemyComponentPtr->GetEnemyType())
	{
	case EnemyType::Egg:
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggStunned"));
		break;
	case EnemyType::HotDog:
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogStunned"));
		break;
	case EnemyType::Pickle:
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleStunned"));
		break;
	}
}

std::unique_ptr<dae::EnemyState> dae::StunnedState::Update(GameObject*, float elapsedSec)
{
	if (m_Timer > 0.0f)
	{
		m_Timer -= elapsedSec;
		return nullptr;
	}

	return std::move(m_PreviousState);
}

void dae::StunnedState::OnExit(GameObject*)
{
	m_EnemyMoveComponentPtr->SetIsActive(true);
}





void dae::FallingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_EnemyMoveComponentPtr->SetDirection({ 0.0f, 1.0f });
}

std::unique_ptr<dae::EnemyState> dae::FallingState::OnCollisionEnter(GameObject*, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Platform"))
	{
		return std::make_unique<EnemyDyingState>();
	}

	return nullptr;
}






void dae::EnemyDyingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();

	m_EnemyMoveComponentPtr->SetIsActive(false);

	switch (m_EnemyComponentPtr->GetEnemyType())
	{
	case EnemyType::Egg:
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggDying"));
		break;
	case EnemyType::HotDog:
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogDying"));
		break;
	case EnemyType::Pickle:
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleDying"));
		break;
	}
}

std::unique_ptr<dae::EnemyState> dae::EnemyDyingState::Update(GameObject*, float elapsedSec)
{
	if (m_Timer > 0.0f)
	{
		m_Timer -= elapsedSec;
		return nullptr;
	}

	return std::make_unique<EnemyWalkingState>();
}

void dae::EnemyDyingState::OnExit(GameObject* enemyObject)
{
	m_EnemyMoveComponentPtr->SetIsActive(true);

	enemyObject->SetIsActive(false);
}










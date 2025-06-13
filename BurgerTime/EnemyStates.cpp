#include "EnemyStates.h"

#include <algorithm>
#include <iostream>

#include "EnemyComponent.h"
#include "EnemyMoveComponent.h"
#include "SpritesheetComponent.h"
#include "RectCollider2DComponent.h"
#include "Utils.h"
#include "GameObject.h"
#include "ServiceLocator.h"
#include "BurgerPartComponent.h"


void dae::EnemyWalkingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();
	m_RectCollider2DComponentPtr = enemyObject->GetComponent<RectCollider2DComponent>();

	// Handling animations
	const auto direction = m_EnemyMoveComponentPtr->GetDirection();

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

std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::Update(GameObject* enemyObject, float)
{

	const auto ownerColliderRect = m_RectCollider2DComponentPtr->GetCollisionRect();
	const glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height + 1.0f };
	constexpr glm::vec2 rayDirection{ 0.0f, 1.0f };

	const auto intersectedGameObjects = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOrigin, rayDirection, 4.0f);

	bool ladderFound{ false };

	for (const auto intersectedGameObject : intersectedGameObjects)
	{
		if (intersectedGameObject->GetTag() == make_sdbm_hash("Ladder"))
		{
			ladderFound = true;
			m_LadderBelowPtr = intersectedGameObject;
		}
	}

	if (ladderFound)
	{
		const auto directives = m_EnemyMoveComponentPtr->CalculateDirectionDirectives();

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

		const auto ladderWidth = m_LadderBelowPtr->GetComponent<RectCollider2DComponent>()->GetCollisionRect().width;
		const auto widthDifferenceSplit = (m_RectCollider2DComponentPtr->GetCollisionRect().width - ladderWidth) / 2.0f;

		const float xWorld = m_LadderBelowPtr->GetWorldTransform().GetPosition().x - widthDifferenceSplit;
		auto& characterPos = enemyObject->GetWorldTransform().GetPosition();
		enemyObject->SetWorldPosition(xWorld, characterPos.y);

		return std::make_unique<ClimbingState>();
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::OnCollisionEnter(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
	{
		m_CurrentLadderColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());
	}

	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();
		if (burgerComp->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
		{
			switch (m_EnemyComponentPtr->GetEnemyType())
			{
			case EnemyType::HotDog:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
				break;
			case EnemyType::Pickle:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
				break;
			case EnemyType::Egg:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
				break;
			}

			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().clear();
			return std::make_unique<EnemyDyingState>();
		}
		else
		{
			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().insert(burgerComp);
			return nullptr;
		}
	}

	if (observedGameObject->GetTag() == make_sdbm_hash("PepperSpray"))
	{
		return std::make_unique<StunnedState>(this->Clone());
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
	{
		const auto distance = enemyObject->GetWorldTransform().GetPosition().x - observedGameObject->GetWorldTransform().GetPosition().x;

		if ( abs(distance) <= 0.25f )
		{
			const auto directives = m_EnemyMoveComponentPtr->CalculateDirectionDirectives();

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

			const auto ladderWidth = observedGameObject->GetComponent<RectCollider2DComponent>()->GetCollisionRect().width;
			const auto widthDifferenceSplit = (m_RectCollider2DComponentPtr->GetCollisionRect().width - ladderWidth) / 2.0f;

			const float xWorld = observedGameObject->GetWorldTransform().GetPosition().x - widthDifferenceSplit;
			auto& characterPos = enemyObject->GetWorldTransform().GetPosition();
			enemyObject->SetWorldPosition(xWorld, characterPos.y);

			return std::make_unique<ClimbingState>();
		}
	}

	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		for (const auto burgerPartComponent : m_EnemyComponentPtr->GetCollidedBurgerPartsRef())
		{
			if (burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
			{
				switch (m_EnemyComponentPtr->GetEnemyType())
				{
				case EnemyType::HotDog:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
					break;
				case EnemyType::Pickle:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
					break;
				case EnemyType::Egg:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
					break;
				}

				return std::make_unique<EnemyDyingState>();
				break;
			}
		}
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::EnemyWalkingState::OnCollisionExit(GameObject* , GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();

		m_EnemyComponentPtr->GetCollidedBurgerPartsRef().erase(burgerComp);
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

	const auto direction = m_EnemyMoveComponentPtr->GetDirection();

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

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::ClimbingState::OnCollisionEnter(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();
		if (burgerComp->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
		{
			switch (m_EnemyComponentPtr->GetEnemyType())
			{
			case EnemyType::HotDog:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
				break;
			case EnemyType::Pickle:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
				break;
			case EnemyType::Egg:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
				break;
			}

			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().clear();
			return std::make_unique<EnemyDyingState>();
		}
		else
		{
			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().insert(burgerComp);
			return nullptr;
		}
	}

	if (observedGameObject->GetTag() == make_sdbm_hash("PepperSpray"))
	{
		return std::make_unique<StunnedState>(this->Clone());
	}

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

		const auto ownerColliderRect = m_RectCollider2DComponentPtr->GetCollisionRect();
		const glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
		constexpr glm::vec2 rayDirection{ 0.0f, 1.0f };
		constexpr float rayLength{ 2.0f };
		auto intersectedGameObjects = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOrigin, rayDirection, rayLength);

		if (std::ranges::none_of(intersectedGameObjects, [](const GameObject* gameObject)
			{
				return gameObject->GetTag() == make_sdbm_hash("Ladder");
			}))
		{

			const auto overlapShift = RectCollider2DComponent::GetCollisionOverlapShift(ownerColliderRect, observedGameObject->GetComponent<RectCollider2DComponent>()->GetCollisionRect());
			auto& ownerPos = enemyObject->GetWorldTransform().GetPosition();
			enemyObject->SetWorldPosition(ownerPos.x + overlapShift.x, ownerPos.y + overlapShift.y);

			const auto directives = m_EnemyMoveComponentPtr->CalculateDirectionDirectives();

			constexpr float offset = 4.0f;

			glm::vec2 rayOriginDownRight{ownerColliderRect.posX + ownerColliderRect.width + offset, ownerColliderRect.posY + ownerColliderRect.height };
			glm::vec2 rayOriginDownLeft{ ownerColliderRect.posX - offset, ownerColliderRect.posY + ownerColliderRect.height };

			auto objectsBelowRight = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOriginDownRight, rayDirection, rayLength);
			auto objectsBelowLeft = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOriginDownLeft, rayDirection, rayLength);

			bool platformBelowRight = std::ranges::any_of(objectsBelowRight, [](const GameObject* gameObject)
				{
					return gameObject->GetTag() == make_sdbm_hash("Platform");
				});
			bool platformBelowLeft = std::ranges::any_of(objectsBelowLeft, [](const GameObject* gameObject)
				{
					return gameObject->GetTag() == make_sdbm_hash("Platform");
				});

			if (directives.horDirective == EnemyMoveComponent::HorizontalDirective::Right && platformBelowRight)
			{

				m_EnemyMoveComponentPtr->SetDirection({ 1.0f, 0.0f });
				return std::make_unique<FinishedClimbingState>();
			}

			if (directives.horDirective == EnemyMoveComponent::HorizontalDirective::Left && platformBelowLeft)
			{
				m_EnemyMoveComponentPtr->SetDirection({ -1.0f, 0.0f });
				return std::make_unique<FinishedClimbingState>();
			}

			if (directives.horDirective == EnemyMoveComponent::HorizontalDirective::Right && !platformBelowRight && platformBelowLeft)
			{
				m_EnemyMoveComponentPtr->SetDirection({ -1.0f, 0.0f });
				return std::make_unique<FinishedClimbingState>();
			}

			if (directives.horDirective == EnemyMoveComponent::HorizontalDirective::Left && !platformBelowLeft && platformBelowRight)
			{
				m_EnemyMoveComponentPtr->SetDirection({ 1.0f, 0.0f });
				return std::make_unique<FinishedClimbingState>();
			}

			// If this case occurs - continue climbing
			std::cout << "No way to go - occured" << "\n";
			return nullptr;
		}
	}

	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		for (auto burgerPartComponent : m_EnemyComponentPtr->GetCollidedBurgerPartsRef())
		{
			if (burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
			{
				switch (m_EnemyComponentPtr->GetEnemyType())
				{
				case EnemyType::HotDog:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
					break;
				case EnemyType::Pickle:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
					break;
				case EnemyType::Egg:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
					break;
				}

				return std::make_unique<EnemyDyingState>();
				break;
			}
		}
	}
	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::ClimbingState::OnCollisionExit(GameObject* , GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();

		m_EnemyComponentPtr->GetCollidedBurgerPartsRef().erase(burgerComp);
	}

	return nullptr;
}



void dae::FinishedClimbingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_SpritesheetComponentPtr = enemyObject->GetComponent<SpritesheetComponent>();

	// Handling animations
	auto& direction = m_EnemyMoveComponentPtr->GetDirection();

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

std::unique_ptr<dae::EnemyState> dae::FinishedClimbingState::OnCollisionEnter(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();
		if (burgerComp->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
		{
			switch (m_EnemyComponentPtr->GetEnemyType())
			{
			case EnemyType::HotDog:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
				break;
			case EnemyType::Pickle:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
				break;
			case EnemyType::Egg:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
				break;
			}

			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().clear();
			return std::make_unique<EnemyDyingState>();
		}
		else
		{
			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().insert(burgerComp);
			return nullptr;
		}
	}

	if (observedGameObject->GetTag() == make_sdbm_hash("PepperSpray"))
	{
		return std::make_unique<StunnedState>(this->Clone());
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::FinishedClimbingState::OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		for (const auto burgerPartComponent : m_EnemyComponentPtr->GetCollidedBurgerPartsRef())
		{
			if (burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
			{
				switch (m_EnemyComponentPtr->GetEnemyType())
				{
				case EnemyType::HotDog:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
					break;
				case EnemyType::Pickle:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
					break;
				case EnemyType::Egg:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
					break;
				}

				return std::make_unique<EnemyDyingState>();
				break;
			}
		}
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::FinishedClimbingState::OnCollisionExit(GameObject*, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();

		m_EnemyComponentPtr->GetCollidedBurgerPartsRef().erase(burgerComp);
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


	auto& sound = ServiceLocator::GetSoundSystem();
	sound.Play("EnemySprayed.wav", 0.8f);
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

std::unique_ptr<dae::EnemyState> dae::StunnedState::OnCollisionEnter(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();
		if (burgerComp->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
		{
			switch (m_EnemyComponentPtr->GetEnemyType())
			{
			case EnemyType::HotDog:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
				break;
			case EnemyType::Pickle:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
				break;
			case EnemyType::Egg:
				m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
				break;
			}

			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().clear();
			return std::make_unique<EnemyDyingState>();
		}
		else
		{
			m_EnemyComponentPtr->GetCollidedBurgerPartsRef().insert(burgerComp);
			return nullptr;
		}
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::StunnedState::OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		for (const auto burgerPartComponent : m_EnemyComponentPtr->GetCollidedBurgerPartsRef())
		{
			if (burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Falling)
			{
				switch (m_EnemyComponentPtr->GetEnemyType())
				{
				case EnemyType::HotDog:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("HotDogKilled")), enemyObject);
					break;
				case EnemyType::Pickle:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("PickleKilled")), enemyObject);
					break;
				case EnemyType::Egg:
					m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EggKilled")), enemyObject);
					break;
				}

				return std::make_unique<EnemyDyingState>();
				break;
			}
		}
	}

	return nullptr;
}

std::unique_ptr<dae::EnemyState> dae::StunnedState::OnCollisionExit(GameObject*, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
	{
		const auto burgerComp = observedGameObject->GetComponent<BurgerPartComponent>();

		m_EnemyComponentPtr->GetCollidedBurgerPartsRef().erase(burgerComp);
	}

	return nullptr;
}



void dae::FallingState::OnEnter(GameObject* enemyObject)
{
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();
	m_EnemyMoveComponentPtr->SetDirection({ 0.0f, 1.0f });

	auto& sound = ServiceLocator::GetSoundSystem();
	sound.Play("EnemyFall.wav", 0.8f);
}

std::unique_ptr<dae::EnemyState> dae::FallingState::OnCollisionEnter(GameObject*, GameObject* observedGameObject)
{
	if (observedGameObject->GetTag() == make_sdbm_hash("Platform") || observedGameObject->GetTag() == make_sdbm_hash("Plate"))
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
	enemyObject->GetComponent<RectCollider2DComponent>()->SetIsActive(false);

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

	auto& sound = ServiceLocator::GetSoundSystem();
	sound.Play("EnemySquashed.wav", 0.8f);
}

std::unique_ptr<dae::EnemyState> dae::EnemyDyingState::Update(GameObject*, float elapsedSec)
{
	if (m_Timer > 0.0f)
	{
		m_Timer -= elapsedSec;
		return nullptr;
	}

	return std::make_unique<EnemyDeadState>();
}

void dae::EnemyDyingState::OnExit(GameObject* enemyObject)
{
	enemyObject->SetIsActive(false);
}



void dae::EnemyDeadState::OnEnter(GameObject* enemyObject)
{
	m_EnemyComponentPtr = enemyObject->GetComponent<EnemyComponent>();
	m_EnemyMoveComponentPtr = enemyObject->GetComponent<EnemyMoveComponent>();

	m_EnemyComponentPtr->GetEnemyDyingEvent()->NotifyObservers(Event(make_sdbm_hash("EnemyDied")), enemyObject);
}

std::unique_ptr<dae::EnemyState> dae::EnemyDeadState::Update(GameObject*, float)
{
	return nullptr;
}

void dae::EnemyDeadState::OnExit(GameObject* enemyObject)
{
	m_EnemyMoveComponentPtr->SetIsActive(true);
	enemyObject->GetComponent<RectCollider2DComponent>()->SetIsActive(true);
	enemyObject->SetIsActive(true);
}














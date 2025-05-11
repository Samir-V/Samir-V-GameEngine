#pragma once
#include "GameObject.h"
#include "SpritesheetComponent.h"
#include "EnemyMoveComponent.h"

namespace dae
{
	class EnemyState
	{
	public:
		virtual ~EnemyState() = default;
		virtual void OnEnter(GameObject*) {}
		virtual std::unique_ptr<EnemyState> Update(GameObject*, float) { return nullptr; }
		virtual void OnExit(GameObject*) {}
	};

	class WalkingState final : public EnemyState
	{
	public:
		WalkingState() = default;

		void OnEnter(GameObject* peterPepperObject) override
		{
			m_EnemyMoveComponentPtr = peterPepperObject->GetComponent<EnemyMoveComponent>();
			m_SpritesheetComponentPtr = peterPepperObject->GetComponent<SpritesheetComponent>();
		}

		std::unique_ptr<EnemyState> Update(GameObject*, float) override
		{
			auto velocity = m_EnemyMoveComponentPtr->GetVelocity();

			switch (m_EnemyMoveComponentPtr->GetEnemyType())
			{
			case EnemyMoveComponent::EnemyType::Egg:
				if (velocity.x < 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingLeft"));
				}
				else if (velocity.x > 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingRight"));
				}
				else if (velocity.y > 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingDown"));
				}
				else if (velocity.y < 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("EggWalkingUp"));
				}
				break;
			case EnemyMoveComponent::EnemyType::HotDog:
				if (velocity.x < 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingLeft"));
				}
				else if (velocity.x > 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingRight"));
				}
				else if (velocity.y > 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingDown"));
				}
				else if (velocity.y < 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("HotDogWalkingUp"));
				}
				break;
			case EnemyMoveComponent::EnemyType::Pickle:
				if (velocity.x < 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingLeft"));
				}
				else if (velocity.x > 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingRight"));
				}
				else if (velocity.y > 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingDown"));
				}
				else if (velocity.y < 0)
				{
					m_SpritesheetComponentPtr->Play(make_sdbm_hash("PickleWalkingUp"));
				}
				break;
			}

			return nullptr;
		}

		void OnExit(GameObject*) override
		{
		}

	private:

		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
	};

	class ClimbingState final : public EnemyState
	{
		
	};

	class FinishedClimbingState final : public EnemyState
	{
		
	};

	class StunnedState final : public EnemyState
	{
		// State when enemy was sprayed
	};

	class FallingState final : public EnemyState
	{
		// State when enemy was on the burger part and falls with it
	};

	class DyingState final : public EnemyState
	{
		// Dying state
	};

}

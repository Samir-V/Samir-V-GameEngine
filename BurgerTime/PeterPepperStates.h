#pragma once
#include "GameObject.h"
#include "MoveComponent.h"
#include "SpritesheetComponent.h"

namespace dae
{
	class PeterPepperState
	{
	public:
		virtual ~PeterPepperState() = default;
		virtual void OnEnter(GameObject*) {}
		virtual std::unique_ptr<PeterPepperState> Update(GameObject*, float) { return nullptr; }
		virtual void OnExit(GameObject*) {}
	};


	class WalkingState final : public PeterPepperState
	{
	public:
		WalkingState() = default;

		void OnEnter(GameObject* peterPepperObject) override
		{
			m_MoveComponentPtr = peterPepperObject->GetComponent<MoveComponent>();
			m_SpritesheetComponentPtr = peterPepperObject->GetComponent<SpritesheetComponent>();
		}

		std::unique_ptr<PeterPepperState> Update(GameObject*, float) override
		{
			auto velocity = m_MoveComponentPtr->GetVelocity();
			auto lastNonZeroDirection = m_MoveComponentPtr->GetLastDirection();

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

		void OnExit(GameObject*) override
		{
		}

	private:

		MoveComponent* m_MoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
	};


	class SprayingState final : public PeterPepperState
	{
	public:
		SprayingState() = default;

		void OnEnter(GameObject* peterPepperObject) override
		{
			auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
			m_LastDirection = moveComponent->GetLastDirection();

			// TODO: Disable movement (deactivate component)
			moveComponent->SetDirection(glm::vec2(0.0f, 0.0f));

			m_SpritesheetComponentPtr = peterPepperObject->GetComponent<SpritesheetComponent>();
		}

		std::unique_ptr<PeterPepperState> Update(GameObject*, float elapsedSec) override
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

		void OnExit(GameObject*) override
		{
			//auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();

			// TODO: Enable movement (Reactivate component)

		}

	private:

		float m_Timer{ 1.0f };

		glm::vec2 m_LastDirection{};

		SpritesheetComponent* m_SpritesheetComponentPtr{};
	};
}


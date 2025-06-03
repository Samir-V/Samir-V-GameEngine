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

			moveComponent->SetDirection(glm::vec2(0.0f, 0.0f));
			moveComponent->SetIsActive(false);

			m_SpritesheetComponentPtr = peterPepperObject->GetComponent<SpritesheetComponent>();

			// Here also needed to get the child object of PeterPepper and activate it, which is the pepper spray object
			auto it = std::ranges::find_if(peterPepperObject->GetChildrenVector(), [](GameObject* child)
				{
					return child->GetTag() == make_sdbm_hash("PepperSpray");
				});

			m_PepperSprayObjectPtr = *it;

			assert(m_PepperSprayObjectPtr != nullptr);

			m_PepperSprayObjectPtr->SetIsActive(true);

			auto spraySpriteSheetComp = m_PepperSprayObjectPtr->GetComponent<SpritesheetComponent>();
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

		void OnExit(GameObject* peterPepperObject) override
		{
			auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
			moveComponent->SetIsActive(true);

			// Here also needed to get the child object of PeterPepper and deactivate it, which is the pepper spray object
			m_PepperSprayObjectPtr->SetIsActive(false);
		}

	private:

		float m_Timer{ 1.0f };

		glm::vec2 m_LastDirection{};

		SpritesheetComponent* m_SpritesheetComponentPtr{};
		GameObject* m_PepperSprayObjectPtr{};
	};

	class WinningState final : public PeterPepperState
	{
		// To be implemented
	};

	class DyingState final : public PeterPepperState
	{
	public:
		DyingState() = default;

		void OnEnter(GameObject* peterPepperObject) override
		{
			auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();

			moveComponent->SetDirection(glm::vec2(0.0f, 0.0f));
			moveComponent->SetIsActive(false);

			auto spriteSheetComp = peterPepperObject->GetComponent<SpritesheetComponent>();
			spriteSheetComp->ResetSpriteTiming();

			spriteSheetComp->Play(make_sdbm_hash("PPDying"));
			
		}

		std::unique_ptr<PeterPepperState> Update(GameObject*, float elapsedSec) override
		{
			m_Timer -= elapsedSec;

			if (m_Timer < 0.0f)
			{
				return std::make_unique<WalkingState>();
			}

			return nullptr;
		}

		void OnExit(GameObject* peterPepperObject) override
		{
			auto moveComponent = peterPepperObject->GetComponent<MoveComponent>();
			moveComponent->SetIsActive(true);
		}

	private:

		float m_Timer{ 2.0f };
	};
}


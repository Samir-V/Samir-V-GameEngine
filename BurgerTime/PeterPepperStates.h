#pragma once
#include "GameObject.h"
#include "MoveComponent.h"
#include "ServiceLocator.h"
#include "SpritesheetComponent.h"

namespace svengine
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

		void OnEnter(GameObject* peterPepperObject) override;
		std::unique_ptr<PeterPepperState> Update(GameObject* peterPepperObject, float elapsedSec) override;
		void OnExit(GameObject* peterPepperObject) override;

	private:

		MoveComponent* m_MoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
	};

	class SprayingState final : public PeterPepperState
	{
	public:
		SprayingState() = default;

		void OnEnter(GameObject* peterPepperObject) override;
		std::unique_ptr<PeterPepperState> Update(GameObject* peterPepperObject, float elapsedSec) override;
		void OnExit(GameObject* peterPepperObject) override;

	private:

		float m_Timer{ 1.0f };

		glm::vec2 m_LastDirection{};

		SpritesheetComponent* m_SpritesheetComponentPtr{};
		GameObject* m_PepperSprayObjectPtr{};
	};

	class WinningState final : public PeterPepperState
	{
	public:
		WinningState() = default;

		void OnEnter(GameObject* peterPepperObject) override;

		std::unique_ptr<PeterPepperState> Update(GameObject* peterPepperObject, float elapsedSec) override;

		void OnExit(GameObject* peterPepperObject) override;

	private:

		float m_Timer{ 4.0f };
	};


	class DeadState final : public PeterPepperState
	{
	public:
		DeadState() = default;

		void OnEnter(GameObject* peterPepperObject) override;

		std::unique_ptr<PeterPepperState> Update(GameObject* peterPepperObject, float elapsedSec) override;

		void OnExit(GameObject* peterPepperObject) override;
	};

	class DyingState final : public PeterPepperState
	{
	public:
		DyingState() = default;

		void OnEnter(GameObject* peterPepperObject) override;

		std::unique_ptr<PeterPepperState> Update(GameObject* peterPepperObject, float elapsedSec) override;

		void OnExit(GameObject* peterPepperObject) override;

	private:

		float m_Timer{ 2.0f };
	};
}


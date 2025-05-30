#pragma once
#include <memory>
#include <set>

namespace dae
{
	class GameObject;
	class SpritesheetComponent;
	class EnemyComponent;
	class EnemyMoveComponent;
	class RectCollider2DComponent;

	class EnemyState
	{
	public:

		enum class EnemyType
		{
			HotDog,
			Egg,
			Pickle
		};

		virtual ~EnemyState() = default;
		virtual void OnEnter(GameObject*) {}
		virtual std::unique_ptr<EnemyState> Update(GameObject*, float) { return nullptr; }
		virtual void OnExit(GameObject*) {}

		virtual std::unique_ptr<EnemyState> OnCollisionEnter(GameObject*, GameObject*) { return nullptr; }
		virtual std::unique_ptr<EnemyState> OnCollisionStay(GameObject*, GameObject*) { return nullptr; }
		virtual std::unique_ptr<EnemyState> OnCollisionExit(GameObject*, GameObject*) { return nullptr; }
	};


	class EnemyWalkingState final : public EnemyState
	{
	public:
		EnemyWalkingState() = default;

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		std::unique_ptr<EnemyState> OnCollisionEnter(GameObject* enemyObject, GameObject* observedGameObject) override;
		std::unique_ptr<EnemyState> OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject) override;

	private:

		std::set<RectCollider2DComponent*> m_CurrentLadderColliders;

		GameObject* m_LadderBelowPtr;

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
		RectCollider2DComponent* m_RectCollider2DComponentPtr{};
	};

	class ClimbingState final : public EnemyState
	{
	public:
		ClimbingState() = default;

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		std::unique_ptr<EnemyState> OnCollisionStay(GameObject* enemyObject, GameObject* observedGameObject) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
		RectCollider2DComponent* m_RectCollider2DComponentPtr{};

		float m_Timer{ 1.0f };
	};


	class FinishedClimbingState final : public EnemyState
	{
	public:
		FinishedClimbingState() = default;

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};

		float m_Timer{ 1.0f };
	};

	class StunnedState final : public EnemyState
	{
	public:
		StunnedState(std::unique_ptr<EnemyState> previousState);

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		void OnExit(GameObject* enemyObject) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
		std::unique_ptr<EnemyState> m_PreviousState{};

		float m_Timer{ 4.0f };
	};

	class FallingState final : public EnemyState
	{
		// State when enemy was on the burger part and falls with it
	};

	class EnemyDyingState final : public EnemyState
	{
	public:
		EnemyDyingState() = default;

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		void OnExit(GameObject* enemyObject) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};

		float m_Timer{ 2.0f };
	};

}

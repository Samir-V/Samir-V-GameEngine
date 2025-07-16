#pragma once
#include <memory>
#include <set>

namespace svengine
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

		virtual std::unique_ptr<EnemyState> Clone() const = 0;

		virtual std::unique_ptr<EnemyState> OnCollisionEnter(GameObject*, GameObject*) { return nullptr; }
		virtual std::unique_ptr<EnemyState> OnCollisionStay(GameObject*, GameObject*) { return nullptr; }
		virtual std::unique_ptr<EnemyState> OnCollisionExit(GameObject*, GameObject*) { return nullptr; }
	};


	class EnemyWalkingState final : public EnemyState
	{
	public:
		EnemyWalkingState() = default;

		std::unique_ptr<EnemyState> Clone() const override { return std::make_unique<EnemyWalkingState>(*this); }

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		std::unique_ptr<EnemyState> OnCollisionEnter(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionStay(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionExit(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;

	private:

		std::set<RectCollider2DComponent*> m_CurrentLadderColliders{};

		GameObject* m_LadderBelowPtr{};

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
		RectCollider2DComponent* m_RectCollider2DComponentPtr{};
	};

	class ClimbingState final : public EnemyState
	{
	public:
		ClimbingState() = default;

		std::unique_ptr<EnemyState> Clone() const override { return std::make_unique<ClimbingState>(*this); }

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		std::unique_ptr<EnemyState> OnCollisionEnter(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionStay(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionExit(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
		RectCollider2DComponent* m_RectCollider2DComponentPtr{};

		float m_Timer{ 0.5f };
	};


	class FinishedClimbingState final : public EnemyState
	{
	public:
		FinishedClimbingState() = default;

		std::unique_ptr<EnemyState> Clone() const override { return std::make_unique<FinishedClimbingState>(*this); }

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		std::unique_ptr<EnemyState> OnCollisionEnter(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionStay(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionExit(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};

		float m_Timer{ 0.32f };
	};

	class EnemyStunnedState final : public EnemyState
	{
	public:
		EnemyStunnedState(std::unique_ptr<EnemyState> previousState);

		// Although exists - will not be used
		std::unique_ptr<EnemyState> Clone() const override
		{
			auto prevClone = m_PreviousState->Clone();

			auto newStun = std::make_unique<EnemyStunnedState>(std::move(prevClone));

			newStun->m_Timer = this->m_Timer;

			return newStun;
		}

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		void OnExit(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> OnCollisionEnter(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionStay(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;
		std::unique_ptr<EnemyState> OnCollisionExit(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};
		std::unique_ptr<EnemyState> m_PreviousState{};

		float m_Timer{ 4.0f };
	};

	class EnemyFallingState final : public EnemyState
	{
	public:
		EnemyFallingState() = default;

		std::unique_ptr<EnemyState> Clone() const override { return std::make_unique<EnemyFallingState>(*this); }

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> OnCollisionEnter(GameObject* enemyObject, GameObject* gameObjectCausingEvent) override;


	private:

		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
	};

	class EnemyDyingState final : public EnemyState
	{
	public:
		EnemyDyingState() = default;

		std::unique_ptr<EnemyState> Clone() const override { return std::make_unique<EnemyDyingState>(*this); }

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		void OnExit(GameObject* enemyObject) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
		SpritesheetComponent* m_SpritesheetComponentPtr{};

		float m_Timer{ 2.0f };
	};

	class EnemyDeadState final : public EnemyState
	{
	public:
		EnemyDeadState() = default;

		std::unique_ptr<EnemyState> Clone() const override { return std::make_unique<EnemyDeadState>(*this); }

		void OnEnter(GameObject* enemyObject) override;
		std::unique_ptr<EnemyState> Update(GameObject* enemyObject, float elapsedSec) override;
		void OnExit(GameObject* enemyObject) override;

	private:

		EnemyComponent* m_EnemyComponentPtr{};
		EnemyMoveComponent* m_EnemyMoveComponentPtr{};
	};

}

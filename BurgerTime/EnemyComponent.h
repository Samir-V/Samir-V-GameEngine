#pragma once
#include "ComponentBase.h"
#include <memory>
#include <typeindex>

#include "IObserver.h"
#include "Transform.h"
#include "EnemyStates.h"

using EnemyType = dae::EnemyState::EnemyType;

namespace dae
{
	class EnemyComponent final : public ComponentBase, public IObserver
	{
	public:

		EnemyComponent(GameObject* ownerPtr, EnemyType enemyType);
		~EnemyComponent() override = default;

		EnemyComponent(const EnemyComponent& other) = delete;
		EnemyComponent(EnemyComponent&& other) = delete;
		EnemyComponent& operator=(const EnemyComponent& other) = delete;
		EnemyComponent& operator=(EnemyComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;
		void Notify(const Event& event, GameObject* observedGameObject) override;

		void ChangeState(std::unique_ptr<EnemyState> newState);
		EnemyType GetEnemyType() const;
		std::type_index GetCurrentStateType() const;

	private:

		Transform m_LocalTransform{};

		std::unique_ptr<EnemyState> m_State;

		EnemyType m_EnemyType{};
	};
}

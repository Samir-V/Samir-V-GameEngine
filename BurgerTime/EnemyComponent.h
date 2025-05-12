#pragma once
#include "ComponentBase.h"
#include <memory>

#include "Transform.h"

namespace dae
{
	class EnemyState;

	class EnemyComponent final : public ComponentBase
	{
	public:

		enum class EnemyType
		{
			HotDog,
			Egg,
			Pickle
		};

		EnemyComponent(GameObject* ownerPtr);
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

		void ChangeState(std::unique_ptr<EnemyState> newState);
		EnemyType GetEnemyType() const;

	private:

		Transform m_LocalTransform{};

		std::unique_ptr<EnemyState> m_State;

		EnemyType m_EnemyType{};
	};
}

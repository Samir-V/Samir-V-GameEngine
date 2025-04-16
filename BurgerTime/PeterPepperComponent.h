#pragma once
#include <memory>

#include "ComponentBase.h"
#include "Subject.h"
#include "Transform.h"

namespace dae
{
	class SpritesheetComponent;

	class PeterPepperComponent final : public ComponentBase, public IObserver
	{
	public:

		PeterPepperComponent(GameObject* ownerPtr, float maxSpeed);
		~PeterPepperComponent() override = default;

		PeterPepperComponent(const PeterPepperComponent& other) = delete;
		PeterPepperComponent(PeterPepperComponent&& other) = delete;
		PeterPepperComponent& operator=(const PeterPepperComponent& other) = delete;
		PeterPepperComponent& operator=(PeterPepperComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;
		void Notify(const Event& event, GameObject* observedGameObject) override;

		void AddInputToDirection(const glm::vec2& direction);

		Subject* GetObjectDeathEvent() const;
		int GetRemainingHealth() const;
		void Damage(int damage = 1);

		Subject* GetEnemyKilledEvent() const;

	private:

		Transform m_LocalTransform{};
		glm::vec2 m_Velocity{};
		glm::vec2 m_Direction{};
		glm::vec2 m_LastNonZeroDirection{};

		const float m_MaxSpeed{};

		int m_Health{ 3 };

		SpritesheetComponent* m_SpritesheetComponentPtr{};

		std::unique_ptr<Subject> m_ObjectDeathEvent;
		std::unique_ptr<Subject> m_EnemyKilledEvent;
	};
}

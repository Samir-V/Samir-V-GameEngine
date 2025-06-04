#pragma once
#include <memory>

#include "ComponentBase.h"
#include "PeterPepperStates.h"
#include "Subject.h"

namespace dae
{
	class PeterPepperComponent final : public ComponentBase
	{
	public:

		PeterPepperComponent(GameObject* ownerPtr);
		~PeterPepperComponent() override = default;

		PeterPepperComponent(const PeterPepperComponent& other) = delete;
		PeterPepperComponent(PeterPepperComponent&& other) = delete;
		PeterPepperComponent& operator=(const PeterPepperComponent& other) = delete;
		PeterPepperComponent& operator=(PeterPepperComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		Subject* GetObjectDeathEvent() const;
		int GetRemainingHealth() const;
		void Damage(int damage = 1);

		Subject* GetEnemyKilledEvent() const;

		void ChangeState(std::unique_ptr<PeterPepperState> newState);

		void AssertVictory();

	private:

		int m_Health{ 3 };

		std::unique_ptr<Subject> m_ObjectDeathEvent;
		std::unique_ptr<Subject> m_EnemyKilledEvent;

		std::unique_ptr<PeterPepperState> m_State;
	};
}

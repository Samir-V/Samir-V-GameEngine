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

		Subject* GetPlayerDiedEvent() const;
		int GetRemainingHealth() const;
		void Damage(int damage = 1);

		void ChangeState(std::unique_ptr<PeterPepperState> newState);

		void AssertVictory();
		void Resurrect();

	private:

		int m_Health{ 3 };
		int m_Peppers{ 5 };

		std::unique_ptr<Subject> m_PlayerDiedEvent;

		std::unique_ptr<PeterPepperState> m_State;
	};
}

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
		Subject* GetPepperAmountChangedEvent() const;
		int GetRemainingLives() const;
		int GetRemainingPeppers() const;

		void ChangeState(std::unique_ptr<PeterPepperState> newState);

		void AssertVictory();
		void Resurrect();
		void SprayPepper();
		void DecreaseLives(int amount = 1);
		void IncreasePepper(int amount = 1);
		void FullRespawn();

	private:

		int m_Lives{ 3 };
		int m_Peppers{ 5 };

		std::unique_ptr<Subject> m_PlayerDiedEvent;
		std::unique_ptr<Subject> m_PepperAmountChangedEvent;

		std::unique_ptr<PeterPepperState> m_State;
	};
}

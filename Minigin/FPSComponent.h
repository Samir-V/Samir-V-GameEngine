#pragma once
#include "ComponentBase.h"
#include "TextComponent.h"
#include "Transform.h"

namespace dae
{
	class FPSComponent final : public ComponentBase
	{
	public:

		FPSComponent(TextComponent* textComponentPtr);
		~FPSComponent() override = default;

		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;

		void Update([[maybe_unused]] float elapsedSec) override;
		void FixedUpdate([[maybe_unused]] float fixedTimeStep) override;
		void LateUpdate([[maybe_unused]] float elapsedSec) override;
		void Render() const override;

		void SetPosition(float x, float y) override;
		void SetOwner(const GameObject* owner) override;
		void Destroy() override;
		bool GetMarkedToDestroy() const override;

	private:

		float CalculateFPS(float elapsedSec);

		bool m_MarkedToDestroy{};
		Transform m_Transform{};

		// A choice to store the text component as a raw pointer to avoid cluttering the GameObject class and for easier communication between components
		// Not const since the text will be modified constantly
		TextComponent* m_AssignedTextComponentPtr{};
		const GameObject* m_OwnerPtr{};
	};
}


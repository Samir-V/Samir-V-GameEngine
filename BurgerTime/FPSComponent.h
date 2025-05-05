#pragma once
#include "ComponentBase.h"
#include "TextComponent.h"
#include "Transform.h"

namespace dae
{
	class FPSComponent final : public ComponentBase
	{
	public:

		FPSComponent(GameObject* ownerPtr, TextComponent* textComponentPtr);
		~FPSComponent() override = default;

		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

	private:

		float CalculateFPS(float elapsedSec);

		Transform m_LocalTransform{};

		// A choice to store the text component as a raw pointer to avoid cluttering the GameObject class and for easier communication between components
		// Not const since the text will be modified constantly
		TextComponent* m_AssignedTextComponentPtr{};
	};
}


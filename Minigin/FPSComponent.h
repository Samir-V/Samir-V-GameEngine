#pragma once
#include "ComponentBase.h"
#include "TextComponent.h"
#include "Transform.h"

namespace dae
{
	class FPSComponent final : public ComponentBase
	{
	public:

		FPSComponent(std::shared_ptr<TextComponent> textComponent);
		~FPSComponent() override = default;

		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void FixedUpdate(float fixedTimeStep) override;
		void Render() const override;

		void SetPosition(float x, float y) override;
		void SetOwner(std::weak_ptr<GameObject> owner) override;

	private:

		float CalculateFPS(float elapsedSec);

		Transform m_Transform{};

		// A choice to store the text component as a shared pointer to avoid cluttering the GameObject class and for easier communication between components
		std::shared_ptr<TextComponent> m_AssignedTextComponent{};
		std::weak_ptr<GameObject> m_Owner{};
	};
}


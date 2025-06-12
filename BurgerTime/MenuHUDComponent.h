#pragma once
#include "ComponentBase.h"
#include "Transform.h"
#include "GameHandlerComponent.h"
#include <array>

namespace dae
{
	class TextComponent;

	class MenuHUDComponent final : public ComponentBase
	{
	public:

		MenuHUDComponent(GameObject* ownerPtr, TextComponent* markerTextComponent, std::vector<TextComponent*> buttons);
		~MenuHUDComponent() override = default;

		MenuHUDComponent(const MenuHUDComponent& other) = delete;
		MenuHUDComponent(MenuHUDComponent&& other) = delete;
		MenuHUDComponent& operator=(const MenuHUDComponent& other) = delete;
		MenuHUDComponent& operator=(MenuHUDComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void ChangeMarkerIndexBy(int delta);
		void UpdateMarkerPosition() const;
		void ChooseMode() const;

	private:

		int m_CurrentIndex{ 0 };

		static constexpr std::array<GameHandlerComponent::GameMode, 3> m_IndexToMode = {
			GameHandlerComponent::GameMode::Solo,
			GameHandlerComponent::GameMode::Coop,
			GameHandlerComponent::GameMode::Versus
		};

		GameHandlerComponent* m_GameHandlerComponentPtr{};
		TextComponent* m_MarkerTextComponentPtr{};
		std::vector<TextComponent*> m_Buttons{};
	};
}


#pragma once
#include <string>
#include <memory>

#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class Font;
	class Texture2D;
	class TextComponent final : public ComponentBase
	{
	public:

		TextComponent(const std::string& text, std::shared_ptr<Font> font);
		~TextComponent() override = default;

		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		void Update([[maybe_unused]] float elapsedSec) override;
		void FixedUpdate([[maybe_unused]] float fixedTimeStep) override;
		void LateUpdate([[maybe_unused]] float elapsedSec) override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetPosition(float x, float y) override;
		void SetOwner(const GameObject* owner) override;
		void Destroy() override;
		bool GetMarkedToDestroy() const override;

	private:

		bool m_NeedsUpdate;
		bool m_MarkedToDestroy{};
		std::string m_Text;
		Transform m_Transform{};
		std::shared_ptr<Font> m_Font;
		std::shared_ptr<Texture2D> m_TextTexture;
		const GameObject* m_OwnerPtr{};
	};
}

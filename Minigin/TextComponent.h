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

		TextComponent(GameObject* ownerPtr, const std::string& text, std::shared_ptr<Font> font);
		~TextComponent() override = default;

		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;
		void RenderImGui() override;

		void SetText(const std::string& text);
		void SetLocalPosition(float x, float y) override;

	private:

		bool m_NeedsUpdate;
		std::string m_Text;
		Transform m_LocalTransform{};
		std::shared_ptr<Font> m_Font;
		std::shared_ptr<Texture2D> m_TextTexture;
	};
}

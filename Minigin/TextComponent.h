#pragma once
#include <string>
#include <memory>

#include "ComponentBase.h"

namespace svengine
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

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetText(const std::string& text);

	private:

		bool m_NeedsUpdate;
		std::string m_Text;
		std::shared_ptr<Font> m_Font;
		std::shared_ptr<Texture2D> m_TextTexture;
	};
}

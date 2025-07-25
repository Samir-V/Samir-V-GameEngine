#include <stdexcept>
#include <SDL_ttf.h>
#include "TextComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "GameObject.h"
#include "Texture2D.h"

svengine::TextComponent::TextComponent(GameObject* ownerPtr, const std::string& text, std::shared_ptr<Font> font) :
	ComponentBase(ownerPtr),
	m_NeedsUpdate(true),
	m_Text(text),
	m_Font(font),
	m_TextTexture(nullptr)
{
}

void svengine::TextComponent::Start()
{
}

void svengine::TextComponent::Update(float)
{
	if (m_NeedsUpdate)
	{
		const SDL_Color color = { 255,255,255,255 }; // only white text is supported now
		const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), color);
		if (surf == nullptr) 
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr) 
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_FreeSurface(surf);
		m_TextTexture = std::make_shared<Texture2D>(texture);
		m_NeedsUpdate = false;
	}
}

void svengine::TextComponent::LateUpdate(float)
{

}

void svengine::TextComponent::Render() const
{
	if (m_TextTexture != nullptr)
	{
		const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + GetLocalTransform().GetPosition();
		Renderer::GetInstance().RenderTexture(*m_TextTexture, pos.x, pos.y);
	}
}

// This implementation uses the "dirty flag" pattern
void svengine::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}
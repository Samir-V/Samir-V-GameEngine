#include <stdexcept>
#include <SDL_ttf.h>
#include "TextComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"

dae::TextComponent::TextComponent(const std::string& text, std::shared_ptr<Font> font)
	: m_NeedsUpdate(true), m_Text(text), m_Font(std::move(font)), m_TextTexture(nullptr)
{ }

void dae::TextComponent::Update([[maybe_unused]] float elapsedSec)
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

void dae::TextComponent::FixedUpdate([[maybe_unused]] float fixedTimeStep)
{

}

void dae::TextComponent::LateUpdate([[maybe_unused]] float elapsedSec)
{

}

void dae::TextComponent::Render() const
{
	if (m_TextTexture != nullptr)
	{
		const auto& pos = m_Transform.GetPosition();
		Renderer::GetInstance().RenderTexture(*m_TextTexture, pos.x, pos.y);
	}
}

// This implementation uses the "dirty flag" pattern
void dae::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetPosition(const float x, const float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}

void dae::TextComponent::SetOwner(const GameObject* owner)
{
	m_OwnerPtr = owner;
}

void dae::TextComponent::Destroy()
{
	m_MarkedToDestroy = true;
}

bool dae::TextComponent::GetMarkedToDestroy() const
{
	return m_MarkedToDestroy;
}
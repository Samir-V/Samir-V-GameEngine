#include <stdexcept>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "ResourceManager.h"
#include "Renderer.h"


void dae::ResourceManager::Init()
{
	if (TTF_Init() != 0)
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}
}

std::shared_ptr<dae::Texture2D> dae::ResourceManager::LoadTexture(const std::string& file)
{
	if(!m_LoadedTextures.contains(file))
	{
		m_LoadedTextures.insert(std::pair(file, std::make_shared<Texture2D>(file)));
	}
	return m_LoadedTextures.at(file);
}

std::shared_ptr<dae::Font> dae::ResourceManager::LoadFont(const std::string& file, uint8_t size)
{
	const auto key = std::pair<std::string, uint8_t>(file, size);

	if(!m_LoadedFonts.contains(key))
	{
		m_LoadedFonts.insert(std::pair(key, std::make_shared<Font>(file, size)));
	}
	return m_LoadedFonts.at(key);
}

void dae::ResourceManager::UnloadUnusedResources()
{
	for (auto it = m_LoadedTextures.begin(); it != m_LoadedTextures.end();)
	{
		if (it->second.use_count() == 1)
			it = m_LoadedTextures.erase(it);
		else
			++it;
	}

	for (auto it = m_LoadedFonts.begin(); it != m_LoadedFonts.end();)
	{
		if (it->second.use_count() == 1)
			it = m_LoadedFonts.erase(it);
		else
			++it;
	}
}

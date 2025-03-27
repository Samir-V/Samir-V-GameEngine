#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <map>
#include "Singleton.h"
#include "Texture2D.h"
#include "Font.h"

namespace dae
{
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Init();
		std::shared_ptr<Texture2D> LoadTexture(const std::string& file);
		std::shared_ptr<Font> LoadFont(const std::string& file, uint8_t size);
	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;
		std::filesystem::path m_DataPath;

		void UnloadUnusedResources();

		std::map<std::string, std::shared_ptr<Texture2D>> m_LoadedTextures;
		std::map<std::pair<std::string, uint8_t>, std::shared_ptr<Font>> m_LoadedFonts;
	};
}

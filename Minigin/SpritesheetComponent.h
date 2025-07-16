#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "ComponentBase.h"
#include "Texture2D.h"

using SpriteID = unsigned int;

namespace svengine
{
	class SpritesheetComponent final : public ComponentBase
	{
	public:

		struct SpriteMetaData
		{
			int numOfCols;
			int numOfRows;
			float timeStep;
		};

		SpritesheetComponent(GameObject* ownerPtr, std::string folderPath = {});
		~SpritesheetComponent() override = default;

		SpritesheetComponent(const SpritesheetComponent& other) = delete;
		SpritesheetComponent(SpritesheetComponent&& other) = delete;
		SpritesheetComponent& operator=(const SpritesheetComponent& other) = delete;
		SpritesheetComponent& operator=(SpritesheetComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void Play(SpriteID spriteId);
		void AddSprite(const std::string& spriteName, SpriteID spriteId, SpriteMetaData spriteMetadata);
		void ResetSpriteTiming();

	private:

		struct SpriteData
		{
			std::shared_ptr<Texture2D> texture;
			SpriteMetaData metaData;
		};

		int   m_CurrentCol;
		int   m_CurrentRow;
		float m_Accumulator;

		std::string m_FolderPath;

		SpriteData* m_CurrentSprite{ nullptr };

		std::unordered_map<SpriteID, SpriteData> m_SpriteSheet;
	};
}


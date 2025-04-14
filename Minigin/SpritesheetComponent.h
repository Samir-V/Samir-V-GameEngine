#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "ComponentBase.h"
#include "Texture2D.h"
#include "Transform.h"

namespace dae
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

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void Play(const std::string& spriteName);
		void AddSprite(const std::string& spriteName, SpriteMetaData spriteMetadata);

		void SetLocalPosition(float x, float y) override;

	private:

		struct SpriteData
		{
			std::shared_ptr<Texture2D> texture;
			SpriteMetaData metaData;
		};

		Transform m_LocalTransform{};

		int   m_CurrentCol{};
		int   m_CurrentRow{};
		float m_Accumulator{};

		std::string m_FolderPath{};

		SpriteData* m_CurrentSprite{ nullptr };

		std::unordered_map<std::string, SpriteData> m_SpriteSheet{};
	};
}


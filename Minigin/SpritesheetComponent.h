#pragma once
#include <map>
#include <memory>
#include <string>

#include "ComponentBase.h"
#include "Texture2D.h"
#include "Transform.h"

namespace dae
{
	class SpritesheetComponent final : ComponentBase
	{
	public:

		struct SpriteData
		{

		};

		SpritesheetComponent(GameObject* ownerPtr, const std::string& filename);
		~SpritesheetComponent() override = default;

		SpritesheetComponent(const SpritesheetComponent& other) = delete;
		SpritesheetComponent(SpritesheetComponent&& other) = delete;
		SpritesheetComponent& operator=(const SpritesheetComponent& other) = delete;
		SpritesheetComponent& operator=(SpritesheetComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void Play(const std::string& spriteName);
		void AddSprite(const std::string& spriteName, SpriteData spriteMetadata);

		void SetLocalPosition(float x, float y) override;

	private:

		Transform m_LocalTransform{};

		std::map<std::string, SpriteData> m_SpriteSheet{};
	};
}


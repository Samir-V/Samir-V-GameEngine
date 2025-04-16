#pragma once
#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class LevelPartComponent final : public ComponentBase
	{
	public:
		enum class LevelPartType
		{
			Platform,
			Ladder,
			Plate
		};

		LevelPartComponent(GameObject* ownerPtr, LevelPartType partType);
		~LevelPartComponent() override = default;

		LevelPartComponent(const LevelPartComponent& other) = delete;
		LevelPartComponent(LevelPartComponent&& other) = delete;
		LevelPartComponent& operator=(const LevelPartComponent& other) = delete;
		LevelPartComponent& operator=(LevelPartComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

		LevelPartType GetLevelPartType() const;

	private:

		Transform m_LocalTransform{};

		const LevelPartType m_PartType;
	};
}


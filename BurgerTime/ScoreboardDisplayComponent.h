#pragma once
#include <string>
#include <vector>

#include "ComponentBase.h"

namespace dae
{
	class TextComponent;

	class ScoreboardDisplayComponent final : public ComponentBase
	{
	public:

		using HighScoreEntry = std::pair<std::string, int>;

		ScoreboardDisplayComponent(GameObject* ownerPtr, std::vector<TextComponent*> displays);
		~ScoreboardDisplayComponent() override = default;

		ScoreboardDisplayComponent(const ScoreboardDisplayComponent& other) = delete;
		ScoreboardDisplayComponent(ScoreboardDisplayComponent&& other) = delete;
		ScoreboardDisplayComponent& operator=(const ScoreboardDisplayComponent& other) = delete;
		ScoreboardDisplayComponent& operator=(ScoreboardDisplayComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

	private:	

		const std::string m_Filename = "highscores.txt";
		std::vector<HighScoreEntry> m_Entries;
		std::vector<TextComponent*> m_Displays;
	};
}


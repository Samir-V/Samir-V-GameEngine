#pragma once
#include <string>
#include <vector>

#include "ComponentBase.h"

namespace svengine
{
	class TextComponent;

	class HighScoreInputComponent final : public ComponentBase
	{
	public:

		using HighScoreEntry = std::pair<std::string, int>;

		HighScoreInputComponent(GameObject* ownerPtr, TextComponent* scoreDisplay, TextComponent* nameDisplay, std::vector<TextComponent*> letterDisplays);
		~HighScoreInputComponent() override = default;

		HighScoreInputComponent(const HighScoreInputComponent& other) = delete;
		HighScoreInputComponent(HighScoreInputComponent&& other) = delete;
		HighScoreInputComponent& operator=(const HighScoreInputComponent& other) = delete;
		HighScoreInputComponent& operator=(HighScoreInputComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetScore(int score);
		void AlterLetterIndex(int delta);
		void ConfirmLetter();
		void ConfirmName();
		void RemoveLetter();

	private:

		int m_CurrentLetterIndex;
		const std::string m_Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::string m_Name;

		int m_FinalScore;
		TextComponent* m_ScoreDisplay;
		TextComponent* m_NameDisplay;
		std::vector<TextComponent*> m_LetterDisplays;

		// Information from file
		std::vector<HighScoreEntry> m_Entries;
		const std::string m_Filename = "highscores.txt";
		int m_MaxEntries;
	};
}


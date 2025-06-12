#include "HighScoreInputComponent.h"

#include <algorithm>
#include <fstream>
#include "TextComponent.h"

dae::HighScoreInputComponent::HighScoreInputComponent(GameObject* ownerPtr, TextComponent* scoreDisplay, TextComponent* nameDisplay, const std::vector<TextComponent*>& letterDisplays):
	ComponentBase(ownerPtr)
	, m_CurrentLetterIndex{0}
	, m_Name{}
	, m_FinalScore{0}
	, m_ScoreDisplay{scoreDisplay}
	, m_NameDisplay{nameDisplay}
	, m_LetterDisplays{letterDisplays}
	, m_MaxEntries{ 5 }
{
	m_ScoreDisplay->SetText("Score: " + std::to_string(m_FinalScore));
	m_NameDisplay->SetText("Your name");
	AlterLetterIndex(0);
}

void dae::HighScoreInputComponent::Start()
{
	m_Entries.clear();
	std::ifstream file{ m_Filename };

	assert(file.is_open());

	HighScoreEntry entry;
	while (file >> entry.first >> entry.second)
	{
		m_Entries.push_back(entry);
	}
	file.close();

	std::ranges::sort(m_Entries, [](auto& a, auto& b) { return a.second > b.second; });

	if (static_cast<int>(m_Entries.size()) > m_MaxEntries)
	{
		m_Entries.resize(m_MaxEntries);
	}
}

void dae::HighScoreInputComponent::Update(float)
{
}

void dae::HighScoreInputComponent::LateUpdate(float)
{
}

void dae::HighScoreInputComponent::Render() const
{
}

void dae::HighScoreInputComponent::SetScore(int score)
{
	m_FinalScore = score;
	m_ScoreDisplay->SetText("Score: " + std::to_string(score));
}

void dae::HighScoreInputComponent::AlterLetterIndex(int delta)
{
	const int alphabetSize = static_cast<int>(m_Alphabet.size());
	m_CurrentLetterIndex = (m_CurrentLetterIndex + delta + alphabetSize) % alphabetSize;

	const int prevIndex = (m_CurrentLetterIndex - 1 + alphabetSize) % alphabetSize;
	const int nextIndex = (m_CurrentLetterIndex + 1) % alphabetSize;

	m_LetterDisplays[0]->SetText(std::string(1, m_Alphabet[prevIndex]));
	m_LetterDisplays[1]->SetText(std::string(1, m_Alphabet[m_CurrentLetterIndex]));
	m_LetterDisplays[2]->SetText(std::string(1, m_Alphabet[nextIndex]));
}

void dae::HighScoreInputComponent::ConfirmLetter()
{
	m_Name += m_Alphabet[m_CurrentLetterIndex];
	m_NameDisplay->SetText(m_Name);
}

void dae::HighScoreInputComponent::ConfirmName()
{
	m_Entries.emplace_back(m_Name, m_FinalScore);

	std::ranges::sort(m_Entries, [](auto& a, auto& b) { return a.second > b.second; });

	if (static_cast<int>(m_Entries.size()) > m_MaxEntries)
	{
		m_Entries.resize(m_MaxEntries);
	}

	std::ofstream file{ m_Filename, std::ios::trunc };
	assert(file.is_open());

	for (auto& [name, score] : m_Entries)
	{
		file << name << ' ' << score << '\n';
	}
}



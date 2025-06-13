#include "ScoreboardDisplayComponent.h"
#include <fstream>
#include <utility>

#include "TextComponent.h"

dae::ScoreboardDisplayComponent::ScoreboardDisplayComponent(GameObject* ownerPtr, std::vector<TextComponent*> displays) :
	ComponentBase(ownerPtr)
	, m_Entries{}
	, m_Displays{ std::move(displays)}
{
}

void dae::ScoreboardDisplayComponent::Start()
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

	assert(m_Entries.size() <= m_Displays.size());

	for (int index{0}; index < static_cast<int>(m_Entries.size()); ++index)
	{
		auto [name, score] = m_Entries.at(index);
		m_Displays[index]->SetText(name + " - " + std::to_string(score));
	}
}

void dae::ScoreboardDisplayComponent::Update(float)
{
}

void dae::ScoreboardDisplayComponent::LateUpdate(float)
{
}

void dae::ScoreboardDisplayComponent::Render() const
{
}




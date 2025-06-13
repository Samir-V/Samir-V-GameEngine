#pragma once
#include <vector>
#include "IObserver.h"

namespace dae
{
	class Subject
	{
	public:

		void AddObserver(IObserver* observer)
		{
			// Preventing adding the same observer to the same subject

			if (m_Observers.empty())
			{
				m_Observers.push_back(observer);
			}
			else
			{
				auto it = std::ranges::find(m_Observers, observer);

				if (it == m_Observers.end())
				{
					m_Observers.push_back(observer);
				}
			}
		}

		void RemoveObserver(IObserver* observer)
		{
			if (m_Observers.empty())
			{
				return;
			}

			auto it = std::ranges::find(m_Observers, observer);

			if (it != m_Observers.end())
			{
				m_Observers.erase(it);
			}
		}

		void NotifyObservers(const Event& event, GameObject* gameObjectCausingEvent) const
		{
			for (const auto observer : m_Observers)
			{
				observer->Notify(event, gameObjectCausingEvent);
			}
		}

	private:

		std::vector<IObserver*> m_Observers{};
	};

}


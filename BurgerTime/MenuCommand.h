#pragma once
#include "Command.h"
#include "MenuHUDComponent.h"

namespace svengine
{
	class MenuCommand : public Command
	{
	public:

		MenuCommand(GameObject* menuHudObject) : m_MenuHudObjectPtr(menuHudObject)
		{
			m_MenuHudComponentPtr = m_MenuHudObjectPtr->GetComponent<MenuHUDComponent>();
		}
		virtual ~MenuCommand() override = default;

	protected:

		MenuHUDComponent* GetMenuHudComponent() const { return m_MenuHudComponentPtr; }

	private:

		GameObject* m_MenuHudObjectPtr;
		MenuHUDComponent* m_MenuHudComponentPtr;
	};


	class AlterHUDCounter : public MenuCommand
	{
	public:
		AlterHUDCounter(GameObject* menuHudObject, int deltaIndex) : MenuCommand(menuHudObject), m_IndexDelta{ deltaIndex }
		{
			m_IndexDelta = std::clamp(m_IndexDelta, -1, 1);
		}

		void Execute() override
		{
			GetMenuHudComponent()->ChangeMarkerIndexBy(m_IndexDelta);
		}

	private:

		int m_IndexDelta;
	};

	class ConfirmModeChoice : public MenuCommand
	{
	public:
		ConfirmModeChoice(GameObject* menuHudObject) : MenuCommand(menuHudObject)
		{
		}

		void Execute() override
		{
			GetMenuHudComponent()->ChooseMode();
		}

	};
}

#include <SDL.h>
#include "InputManager.h"

bool dae::InputManager::ProcessInput()
{
	FlushPendingControllers();
	FlushPendingInputMaps();
	HandleControllerInput();
	return HandleKeyboardInput();
}

bool dae::InputManager::HandleKeyboardInput() const
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:

			return false;

			break;
		case SDL_KEYDOWN:

			if (e.key.repeat == 0)
			{
				for (const auto& keyboardCommandsPair : m_KeyboardCommands)
				{
					if (keyboardCommandsPair.first != m_CurrentInputMap)
					{
						continue;
					}

					for (const auto & keyboardCommand : keyboardCommandsPair.second)
					{
						if (keyboardCommand.key == e.key.keysym.scancode && keyboardCommand.activationType == ActivationType::Pressing)
						{
							keyboardCommand.command->Execute();
						}
					}
				}
			}

			break;
		case SDL_KEYUP:

			for (const auto& keyboardCommandsPair : m_KeyboardCommands)
			{
				if (keyboardCommandsPair.first != m_CurrentInputMap)
				{
					continue;
				}

				for (const auto& keyboardCommand : keyboardCommandsPair.second)
				{
					if (keyboardCommand.key == e.key.keysym.scancode && keyboardCommand.activationType == ActivationType::Releasing)
					{
						keyboardCommand.command->Execute();
					}
				}
			}

			break;
		}
	}

	const Uint8* keyStates = SDL_GetKeyboardState(nullptr);

	for (const auto& keyboardCommandsPair : m_KeyboardCommands)
	{
		if (keyboardCommandsPair.first != m_CurrentInputMap)
		{
			continue;
		}

		for (const auto & keyboardCommand : keyboardCommandsPair.second)
		{
			if (keyboardCommand.activationType == ActivationType::Holding)
			{
				if (keyStates[keyboardCommand.key])
				{
					keyboardCommand.command->Execute();
				}
			}
		}
	}

	return true;
}

void dae::InputManager::HandleControllerInput() const
{
	for (const auto& controller : m_Controllers)
	{
		controller->ProcessInput();

		for (const auto& controllerCommands : m_ControllerCommands)
		{
			// Checks for the input map
			if (controllerCommands.first != m_CurrentInputMap)
			{
				continue;
			}

			for (const auto& controllerCommand : controllerCommands.second)
			{
				if (controllerCommand.controllerIndex != controller->GetControllerIndex())
				{
					continue;
				}

				bool conditionMet = false;

				switch (controllerCommand.activationType)
				{
				case ActivationType::Holding:
					conditionMet = controller->IsHeld(controllerCommand.controllerKey);
					break;
				case ActivationType::Pressing:
					conditionMet = controller->IsPressedThisFrame(controllerCommand.controllerKey);
					break;
				case ActivationType::Releasing:
					conditionMet = controller->IsReleasedThisFrame(controllerCommand.controllerKey);
					break;
				}

				if (conditionMet)
				{
					controllerCommand.command->Execute();
				}
			}
		}
	}
}



void dae::InputManager::RegisterKeyboardCommand(InputMap inputMap, std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType)
{
	m_KeyboardCommands.at(inputMap).emplace_back(std::move(command), key, activationType);
}

void dae::InputManager::RegisterControllerCommand(InputMap inputMap, std::unique_ptr<Command> command, unsigned int controllerKey, ActivationType activationType, int controllerIndex)
{
	m_ControllerCommands.at(inputMap).emplace_back(std::move(command), controllerKey, activationType, controllerIndex);
}

void dae::InputManager::AddController(int index)
{
	m_PendingControllersToAdd.push_back(index);
}

void dae::InputManager::RemoveController(int index)
{
	if (index >= 0 && index < static_cast<int>(m_Controllers.size()))
	{
		m_PendingControllersToRemove.push_back(index);
	}
}

void dae::InputManager::FlushPendingControllers()
{
	for (int index : m_PendingControllersToAdd)
	{
		m_Controllers.emplace_back(std::make_unique<Controller>(index));
	}
	m_PendingControllersToAdd.clear();

	for (int index : m_PendingControllersToRemove)
	{
		m_Controllers.erase(m_Controllers.begin() + index);
	}
	m_PendingControllersToRemove.clear();
}


void dae::InputManager::AddInputMap(InputMap inputMap)
{
	m_KeyboardCommands.emplace(inputMap, std::vector<KeyboardCommand>{});
	m_ControllerCommands.emplace(inputMap, std::vector<ControllerCommand>{});
}

void dae::InputManager::SetActiveInputMap(InputMap inputMap)
{
	m_CurrentInputMap = inputMap;
}

void dae::InputManager::ClearInputMap(InputMap inputMap)
{
	m_PendingInputMapsToClear.push_back(inputMap);
}

void dae::InputManager::FlushPendingInputMaps()
{
	for (InputMap pendingInputMapToClear : m_PendingInputMapsToClear)
	{
		m_KeyboardCommands.at(pendingInputMapToClear).clear();
		m_ControllerCommands.at(pendingInputMapToClear).clear();
	}

	m_PendingInputMapsToClear.clear();
}



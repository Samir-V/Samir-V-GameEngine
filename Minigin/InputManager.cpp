#include <SDL.h>
#include "InputManager.h"

bool dae::InputManager::ProcessInput() const
{
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
				for (const auto& keyboardCommand : m_KeyboardCommands)
				{
					if (keyboardCommand.key == e.key.keysym.scancode && keyboardCommand.activationType == ActivationType::Pressing)
					{
						keyboardCommand.command->Execute();
					}
				}
			}

			break;
		case SDL_KEYUP:

			for (const auto& keyboardCommand : m_KeyboardCommands)
			{
				if (keyboardCommand.key == e.key.keysym.scancode && keyboardCommand.activationType == ActivationType::Releasing)
				{
					keyboardCommand.command->Execute();
				}
			}

			break;
		}
	}


	const Uint8* keyStates = SDL_GetKeyboardState(nullptr);

	for (const auto& keyboardCommand : m_KeyboardCommands)
	{
		if (keyboardCommand.activationType == ActivationType::Holding)
		{
			if (keyStates[keyboardCommand.key])
			{
				keyboardCommand.command->Execute();
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

		for (const auto& controllerCommand : m_ControllerCommands)
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



void dae::InputManager::RegisterKeyboardCommand(std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType)
{
	m_KeyboardCommands.emplace_back(std::move(command), key, activationType);
}

void dae::InputManager::RegisterControllerCommand(std::unique_ptr<Command> command, unsigned int controllerKey, ActivationType activationType, int controllerIndex)
{
	m_ControllerCommands.emplace_back(std::move(command), controllerKey, activationType, controllerIndex);
}

void dae::InputManager::AddController(int index)
{
	m_Controllers.emplace_back(std::make_unique<Controller>(index));
}


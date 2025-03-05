#include <SDL.h>
#include "InputManager.h"

#include "imgui_impl_sdl2.h"

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) 
	{
		if (e.type == SDL_QUIT) 
		{
			return false;
		}

		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			for (const auto& command : m_KeyboardCommands)
			{
				if (std::get<1>(command) == e.key.keysym.scancode && std::get<2>(command) == ActivationType::Pressing)
				{
					std::get<0>(command)->Execute();
				}
			}
		}

		if (e.type == SDL_KEYUP)
		{
			for (const auto& command : m_KeyboardCommands)
			{
				if (std::get<1>(command) == e.key.keysym.scancode && std::get<2>(command) == ActivationType::Releasing)
				{
					std::get<0>(command)->Execute();
				}
			}
		}

		if (e.type == SDL_MOUSEBUTTONDOWN) 
		{
			
		}
		// etc...

		ImGui_ImplSDL2_ProcessEvent(&e);
	}


	const Uint8* keyStates = SDL_GetKeyboardState(nullptr);

	for (const auto& command : m_KeyboardCommands)
	{
		if (std::get<2>(command) == ActivationType::Holding)
		{
			if (keyStates[std::get<1>(command)])
			{
				std::get<0>(command)->Execute();
			}
		}
	}

	return true;
}

void dae::InputManager::RegisterCommand(std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType)
{
	m_KeyboardCommands.emplace_back(std::move(command), key, activationType);
}

#pragma once
#include <memory>
#include <vector>

#include "Command.h"
#include "Singleton.h"

namespace dae
{
	class InputManager final : public Singleton<InputManager>
	{
	public:

		enum class ActivationType
		{
			Pressing,
			Holding,
			Releasing
		};

		bool ProcessInput();

		void RegisterCommand(std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType);


	private:
		// Keyboard commands
		std::vector<std::tuple<std::unique_ptr<Command>, SDL_Scancode, ActivationType>> m_KeyboardCommands{};
	};

}

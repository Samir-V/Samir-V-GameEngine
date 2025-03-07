#pragma once
#include <memory>
#include <vector>

#include "Command.h"
#include "Controller.h"
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

		struct KeyboardCommand
		{
			std::unique_ptr<Command> command;
			SDL_Scancode key;
			ActivationType activationType;
		};

		struct ControllerCommand
		{
			std::unique_ptr<Command> command;
			unsigned int controllerKey;
			ActivationType activationType;
			int controllerIndex;
		};

		bool ProcessInput();

		bool HandleKeyboardInput() const;
		void HandleControllerInput() const;

		void RegisterKeyboardCommand(std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType);
		void RegisterControllerCommand(std::unique_ptr<Command> command, unsigned int controllerKey, ActivationType activationType, int controllerIndex);

		void AddController(int index);

	private:
		std::vector<KeyboardCommand> m_KeyboardCommands{};
		std::vector<ControllerCommand> m_ControllerCommands{};

		std::vector<std::unique_ptr<Controller>> m_Controllers{};
	};

}

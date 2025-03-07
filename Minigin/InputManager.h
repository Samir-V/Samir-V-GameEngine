#pragma once
#include <memory>
#include <vector>
#include <algorithm>

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

		bool ProcessInput() const;

		bool HandleKeyboardInput() const;
		void HandleControllerInput() const;

		void RegisterKeyboardCommand(std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType);
		void RegisterControllerCommand(std::unique_ptr<Command> command, unsigned int controllerKey, ActivationType activationType, int controllerIndex);

		template <typename CommandType>
			requires std::derived_from<CommandType, Command>
		void UnregisterKeyboardCommand()
		{
			auto commandsToUnregister = std::ranges::remove_if(m_KeyboardCommands, [](const auto& keyboardCommand)
				{
					return dynamic_cast<CommandType*>(keyboardCommand.command.get()) != nullptr;
				});

			m_KeyboardCommands.erase(commandsToUnregister.begin(), m_KeyboardCommands.end());
		}

		template <typename CommandType>
			requires std::derived_from<CommandType, Command>
		void UnregisterControllerCommand(int controllerIndex)
		{
			auto commandsToUnregister = std::ranges::remove_if(m_ControllerCommands, [controllerIndex](const auto& controllerCommand)
				{
					return (controllerCommand.controllerIndex == controllerIndex && dynamic_cast<CommandType*>(controllerCommand.command.get()) != nullptr);
				});

			m_ControllerCommands.erase(commandsToUnregister.begin(), m_ControllerCommands.end());
		}

		void AddController(int index);

	private:
		std::vector<KeyboardCommand> m_KeyboardCommands{};
		std::vector<ControllerCommand> m_ControllerCommands{};

		std::vector<std::unique_ptr<Controller>> m_Controllers{};
	};

}

#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "Command.h"
#include "Controller.h"
#include "Singleton.h"

using InputMap = unsigned int;

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

		void AddInputMap(InputMap inputMap);
		void SetActiveInputMap(InputMap inputMap);

		void RegisterKeyboardCommand(InputMap inputMap, std::unique_ptr<Command> command, SDL_Scancode key, ActivationType activationType);
		void RegisterControllerCommand(InputMap inputMap, std::unique_ptr<Command> command, unsigned int controllerKey, ActivationType activationType, int controllerIndex);

		template <typename CommandType>
			requires std::derived_from<CommandType, Command>
		void UnregisterKeyboardCommand(InputMap inputMap, SDL_Scancode key, ActivationType  activationType)
		{
			auto& keyboardCommands = m_KeyboardCommands.at(inputMap);

			auto commandsToUnregister = std::ranges::remove_if(keyboardCommands, [key, activationType](const auto& keyboardCommand)
				{
					return dynamic_cast<CommandType*>(keyboardCommand.command.get()) != nullptr
						&& keyboardCommand.key == key
						&& keyboardCommand.activationType == activationType;
				});

			keyboardCommands.erase(commandsToUnregister.begin(), keyboardCommands.end());
		}

		template <typename CommandType>
			requires std::derived_from<CommandType, Command>
		void UnregisterControllerCommand(InputMap inputMap, unsigned int controllerKey, ActivationType activationType, int controllerIndex)
		{
			auto& controllerCommands = m_ControllerCommands.at(inputMap);

			auto commandsToUnregister = std::ranges::remove_if(controllerCommands, [controllerKey, activationType, controllerIndex](const auto& controllerCommand)
				{
					return controllerCommand.controllerIndex == controllerIndex
						&& dynamic_cast<CommandType*>(controllerCommand.command.get()) != nullptr
						&& controllerCommand.controllerKey == controllerKey
						&& controllerCommand.activationType == activationType;
				});

			controllerCommands.erase(commandsToUnregister.begin(), controllerCommands.end());
		}

		void AddController(int index);
		void RemoveController(int index);

	private:

		void FlushPendingControllers();

		std::unordered_map<InputMap, std::vector<KeyboardCommand>> m_KeyboardCommands{};
		std::unordered_map<InputMap, std::vector<ControllerCommand>> m_ControllerCommands{};

		std::vector<std::unique_ptr<Controller>> m_Controllers{};
		std::vector<int> m_PendingControllersToAdd{};
		std::vector<int> m_PendingControllersToRemove{};

		InputMap m_CurrentInputMap{};
	};

}

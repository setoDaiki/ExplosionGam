#pragma once
#include "KeyInput.h"
#include "Command.h"
#include <functional>
#include <memory>


struct GamepadButtonInput {
	int buttonType;
	KeyState buttonState;
};

class GamepadButtonCommand : public Command {
public:
	GamepadButtonCommand(std::function<void()> action, std::vector<GamepadButtonInput> inputs);
	GamepadButtonCommand(std::function<void()> action, GamepadButtonInput input);
	void Excute()override;

private:
	std::function<void()> mAction;
	std::vector<GamepadButtonInput> mInputs;
};

#include "GamepadButtonCommand.h"

GamepadButtonCommand::GamepadButtonCommand(std::function<void()> action,std::vector<GamepadButtonInput> inputs) 
	:mAction(action)
	,mInputs(inputs)
{

}

GamepadButtonCommand::GamepadButtonCommand(std::function<void()> action,GamepadButtonInput input)
	:mAction(action)
{
	mInputs.push_back(input);
}


void GamepadButtonCommand::Excute() {
	for (auto input : mInputs) {
		if (KeyInput::GetButtonState(input.buttonType) == input.buttonState) {
			mAction();
		}
	}
}
#include "KeyBoardCommand.h"


KeyBoardCommand::KeyBoardCommand(std::function<void()> action, std::vector<KeyBoardInput>&& keyInputs)
	:mKeyInputs(std::move(keyInputs))
	,mAction(action)
{

}

KeyBoardCommand::KeyBoardCommand(std::function<void()> action,KeyBoardInput keyInput)
	:mAction(action)
{
	mKeyInputs.push_back(keyInput);
}


void KeyBoardCommand::Excute() {
	for (auto key : mKeyInputs) {
		if (KeyInput::GetKeyState(key.keyType) == key.keyState) {
			mAction();
		}
	}
}
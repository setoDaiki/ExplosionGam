#pragma once
#include "Command.h"
#include "KeyInput.h"
#include <functional>
#include <windows.h>
#include <memory>

struct KeyBoardInput {
	int keyType;
	KeyState keyState;
};

class KeyBoardCommand :public Command {
public:
	KeyBoardCommand(std::function<void()>,std::vector<KeyBoardInput>&& keyInputs);
	KeyBoardCommand(std::function<void()>,KeyBoardInput keyInput);
	void Excute()override;

private:
	std::vector<KeyBoardInput> mKeyInputs;
	std::function<void()> mAction;

};

using KeyBoardInputs = std::vector<KeyBoardInput>;
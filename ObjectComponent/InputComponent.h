#pragma once
#include "Input/KeyInput.h"
#include <vector>
#include <functional>
#include <memory>




class InputComponent {
public:
	InputComponent();
	InputComponent(std::vector<std::unique_ptr<class Command>> commands);
	~InputComponent();
	//InputComponent(std::vector<class Command>&& commands);

	void Update();
	void AddCommand(std::unique_ptr<class Command> command);

private:
	std::vector<std::unique_ptr<class Command>> mCommands;

};

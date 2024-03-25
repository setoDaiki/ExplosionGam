#include "InputComponent.h"
#include "Input/Command.h"
#include "Input/Command.h"

InputComponent::InputComponent(){

}

InputComponent::InputComponent(std::vector<std::unique_ptr<Command>> commands)
	:mCommands(std::move(commands))
{

}


InputComponent::~InputComponent() {

}



void InputComponent::Update() {
	for (auto& c : mCommands) {
		c->Excute();
	}
}

void InputComponent::AddCommand(std::unique_ptr<Command> command) {
	mCommands.push_back(std::move(command));
}
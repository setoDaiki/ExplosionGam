#include "GamepadStickCommand.h"
#include "KeyInput.h"

GamepadStickCommand::GamepadStickCommand(std::function<void(DirectX::XMFLOAT2)> action,StickType stickType) 
	:mAction(action)
	,mStickType(stickType)
{

}


void GamepadStickCommand::Excute() {
	DirectX::XMFLOAT2 stickAxis = KeyInput::GetStickAxis();
	mAction(stickAxis);
}
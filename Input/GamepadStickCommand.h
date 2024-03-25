#pragma once
#include "Command.h"
#include <functional>
#include <DirectXMath.h>

enum StickType {
	ERStick,
	ELStick
};


class GamepadStickCommand :public Command{
public:
	GamepadStickCommand(std::function<void(DirectX::XMFLOAT2)>,StickType stickType);
	void Excute()override;

private:
	std::function<void(DirectX::XMFLOAT2)> mAction;
	StickType mStickType;

};

#include "Number.h"
#include "ObjectComponent/UI.h"
#include "Common/FileUtil.h"
#include "Common/MyMath.h"
#include "Camera/Camera.h"
#include "ObjectComponent/WorldTransform.h"
#include <iostream>

std::vector<UI> NumberUI::mNumberUIs;


NumberUI::NumberUI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale) 
	:mPos(pos)
	,mScale(scale)
{

	static bool isFirst = true;
	if (isFirst) {
		mNumberUIs.reserve(10);
		for (int i = 0; i < 10; i++) {
			std::string numberString = std::to_string(i);
			std::string textureName =  "Asset/" + numberString + ".png";
			std::wstring textureNameWide = ToWideString(textureName);
			UI numberUI(textureNameWide);
			mNumberUIs.push_back(std::move(numberUI));
		}
		isFirst = false;
	}
}

//----------------------------
//  ”Žš•`‰æ
//----------------------------
void NumberUI::Render(const Camera& camera, int num) {
	static DirectX::XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f,180.0f,0.0f);
	WorldTransform world(mPos,mScale,quaternion);
	int numDigits = Math::NumDigits(num);
	for (int digit = 1; digit <= numDigits; digit++) {
		int renderNumber = Math::GetNumOfDigit(num,digit);
		mNumberUIs[renderNumber].Render(world.GetWorldMat(), camera);
		world.AddPos(DirectX::XMFLOAT3(-mScale.x,0.0f,0.0f));
		world.UpdateWorldMat();
	}
}
#include "HitBox.h"
#include "GameObject/GameObject.h"
#include "ObjectComponent/WorldTransform.h"
#include "Common/Printer.h"

HitBox::HitBox(const DirectX::XMFLOAT3& ownerPos) 
	:mOwnerPos(ownerPos)
{
	
}


const DirectX::XMFLOAT3& HitBox::GetPos()const {
	return mOwnerPos;
}

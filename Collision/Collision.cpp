#include "Collision.h"
#include "SphereHitBox.h"
#include "GameObject/NullObject.h"


std::vector<SphereHitBoxPtr> Collision::mHitBoxs;  // static�ϐ��̒�`

Collision::Collision(SphereHitBoxPtr sphereHitBox) {
	mHitBoxs.push_back(sphereHitBox);
	mHitBox = sphereHitBox;
}

Collision::~Collision() {
	RemoveMyHitBox();
}



//------------------------------------------------------------
//   ���������S�ẴI�u�W�F�N�g�̏����擾
//------------------------------------------------------------
std::vector<GameObject*> Collision::GetAllHitObjectsInfo() {
	std::vector<GameObject*> hitObjects;
	for (auto hit : mHitBoxs) {
		if (mHitBox->IsHitWithSphere(*hit)) {
			GameObject* obj = hit->GetOwnerPtr();
			hitObjects.emplace_back(obj);
		}
	}
	return hitObjects;
}

//---------------------------------------------------------------------
//   ����������̃I�u�W�F�N�g�̏����擾
//---------------------------------------------------------------------
GameObject* Collision::GetHitObjectInfo() {
	
	for (auto hit : mHitBoxs) {
		
		if (mHitBox->IsHitWithSphere(*hit)) {
			GameObject* obj = hit->GetOwnerPtr();
			return obj;
		}
	}
	static GameObject* nullObject = new NullObject();
	
	return nullObject;
}


//----------------------------------------
//   HitBox�̍폜
//----------------------------------------
void Collision::RemoveMyHitBox() {
	auto iter = std::find(mHitBoxs.begin(), mHitBoxs.end(), mHitBox);
	mHitBoxs.erase(iter);
	mHitBoxs.shrink_to_fit();
}


//------------------------------------------
//  �ǂɂԂ���������Ԃ�
//------------------------------------------
bool IsHitWall(const DirectX::XMFLOAT3& pos) {
	if (IsHitFrontOrBackWall(pos)) return true;
	if (IsHitSideWall(pos)) return true;
	return false;
}


//-------------------------------------------
//   �O��̕ǂɂԂ������̂���Ԃ�
//-------------------------------------------
bool IsHitFrontOrBackWall(const DirectX::XMFLOAT3& pos) {
	if (pos.x > 48.0f || pos.x < -48.0f) {   // x����48�ȏォ-48�ȏ�Ȃ�
		return true;                         // �Ԃ����Ă�
	}
	return false;                            // �����łȂ��Ȃ�Ԃ����ĂȂ�
}

//-------------------------------------------
//   ���E�̕ǂɂԂ������̂���Ԃ�
//-------------------------------------------
bool  IsHitSideWall(const DirectX::XMFLOAT3& pos) {
	if (pos.z > 48.0f || pos.z < -48.0f) {   // z����48�ȏォ-48�ȉ��Ȃ�
		return true;                         // �Ԃ����Ă�
	}
	return false;                            // �����łȂ��Ȃ�Ԃ����ĂȂ�
}
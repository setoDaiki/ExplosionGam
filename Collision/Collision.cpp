#include "Collision.h"
#include "SphereHitBox.h"
#include "GameObject/NullObject.h"


std::vector<SphereHitBoxPtr> Collision::mHitBoxs;  // static変数の定義

Collision::Collision(SphereHitBoxPtr sphereHitBox) {
	mHitBoxs.push_back(sphereHitBox);
	mHitBox = sphereHitBox;
}

Collision::~Collision() {
	RemoveMyHitBox();
}



//------------------------------------------------------------
//   当たった全てのオブジェクトの情報を取得
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
//   当たった一つのオブジェクトの情報を取得
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
//   HitBoxの削除
//----------------------------------------
void Collision::RemoveMyHitBox() {
	auto iter = std::find(mHitBoxs.begin(), mHitBoxs.end(), mHitBox);
	mHitBoxs.erase(iter);
	mHitBoxs.shrink_to_fit();
}


//------------------------------------------
//  壁にぶつかったかを返す
//------------------------------------------
bool IsHitWall(const DirectX::XMFLOAT3& pos) {
	if (IsHitFrontOrBackWall(pos)) return true;
	if (IsHitSideWall(pos)) return true;
	return false;
}


//-------------------------------------------
//   前後の壁にぶつかったのかを返す
//-------------------------------------------
bool IsHitFrontOrBackWall(const DirectX::XMFLOAT3& pos) {
	if (pos.x > 48.0f || pos.x < -48.0f) {   // x軸が48以上か-48以上なら
		return true;                         // ぶつかってる
	}
	return false;                            // そうでないならぶつかってない
}

//-------------------------------------------
//   左右の壁にぶつかったのかを返す
//-------------------------------------------
bool  IsHitSideWall(const DirectX::XMFLOAT3& pos) {
	if (pos.z > 48.0f || pos.z < -48.0f) {   // z軸が48以上か-48以下なら
		return true;                         // ぶつかってる
	}
	return false;                            // そうでないならぶつかってない
}
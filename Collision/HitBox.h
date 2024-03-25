#pragma once
#include <DirectXmath.h>
#include <string>
#include <memory>

using namespace DirectX;


class HitBox{
public:
	HitBox(const DirectX::XMFLOAT3& gameObject);
	virtual ~HitBox() {};

	//------------------------------------------------------------
	/// [機能] 球との衝突判定
	/// [引数] このインスタンスと衝突を判別する球
	/// [戻り値] 衝突していたらtrue、衝突していないならfalse
	//------------------------------------------------------------
	virtual bool IsHitWithSphere(const class SphereHitBox& sphere)const = 0;


	//----------------------------------------------------------------------------------
	/// [機能] 当たり判定を所有しているオブジェクトを取得
	/// [戻り値] 当たり判定を所有しているオブジェクト
	//-----------------------------------------------------------------------------------
	const DirectX::XMFLOAT3& GetPos()const;

	
private:
	const DirectX::XMFLOAT3& mOwnerPos;
};


using HitBoxPtrU = std::unique_ptr<HitBox>;
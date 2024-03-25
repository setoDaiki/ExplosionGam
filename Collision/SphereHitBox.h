#pragma once
#include "HitBox.h"
#include <iostream>
#include <memory>

class SphereHitBox :public HitBox {
public:
	SphereHitBox(const DirectX::XMFLOAT3& ownerPos,float radius);
	~SphereHitBox()override;

	//------------------------------------------------------------
	/// [機能] 球との衝突判定
	/// [引数] このインスタンスと衝突を判別する球
	/// [戻り値] 衝突していたらtrue、衝突していないならfalse
	//------------------------------------------------------------
	bool IsHitWithSphere(const SphereHitBox& sphere)const override;


	//-------------------------------------------------------------
	/// [機能] 当たり判定の半径の取得
	/// [戻り値] 当たり判定の半径
	//-------------------------------------------------------------
	float GetRadius()const { return mRadius; }

	

private:
	float mRadius;               // 当たり判定の半径
};

using SpherePtrU = std::unique_ptr<SphereHitBox>;
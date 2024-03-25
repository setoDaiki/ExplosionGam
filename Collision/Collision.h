#pragma once
#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>


class Collision {
public:
	Collision(std::shared_ptr<class SphereHitBox> hitBox);
	~Collision();

	
	//----------------------------------------------------------------------------------
	/// [機能] オブジェクトが衝突したか調べ衝突した全てのオブジェクトのタグ名を返す
	/// [引数] 衝突を検知するオブジェクト(引数のオブジェクトが他のと当たったか調べる)
	/// [戻り値] 衝突した全てのオブジェクトのタグ名
	/// [オバーライドで変化している点] 引数のhitBoxの継承先
	//----------------------------------------------------------------------------------
	std::vector<class GameObject*> GetAllHitObjectsInfo();

	//----------------------------------------------------------------------------------
	/// [機能] オブジェクトが衝突したか調べ最初に衝突したオブジェクトのタグ名を返す
	/// [引数] 衝突を検知するオブジェクト(引数のオブジェクトが他のと当たったか調べる)
	/// [戻り値] 最初に衝突したオブジェクトのタグ名
	/// [オーバーライドで変化している点] 引数のhitBoxの継承先
	//----------------------------------------------------------------------------------
	class GameObject* GetHitObjectInfo();
	/*後で引数がSphere以外の衝突検知関数を作るべき*/


private:
	void RemoveMyHitBox();
	std::shared_ptr<class HitBox> mHitBox;
	static std::vector<std::shared_ptr<class SphereHitBox>> mHitBoxs;  // ゲームにある全ての当たり判定
};

bool IsHitWall(const DirectX::XMFLOAT3& pos);                 // 壁にぶつかったのかを返す
bool IsHitFrontOrBackWall(const DirectX::XMFLOAT3& pos);      // 前後の壁にぶつかったのかを返す
bool IsHitSideWall(const DirectX::XMFLOAT3& pos);             // 左右の壁にぶつかったのかを返す

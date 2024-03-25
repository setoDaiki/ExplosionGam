#pragma once
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>


// 前方宣言
namespace Object {
	enum Type;
};


//=====================================================
/// [機能] オブジェクトのクラスです。
/// 3Dオブジェクトをつくるときはこのクラスを使用します
//=====================================================
class GameObject
{
public:
	enum State {
		EActive,
		EDead,
		EStop,
		EUnDraw,
		EStopUndraw,
		EOther
	};

	
	//---------------------------------------------------------------------------------------------------
	/// [機能] コンストラクタ。引数を用いてobjectの初期化をします。位置、回転、拡大率を決定できます
	/// [引数１] オブジェクトの見た目(頂点やルートシグネチャなど)のソースです
	/// [引数２] ワールド座標
	//----------------------------------------------------------------------------------------------------
	GameObject(
		std::shared_ptr<class ObjectResource> model,
		std::unique_ptr<class WorldTransform> world
	);
	virtual ~GameObject();

	//---------------------------------------------------
	/// [機能] このオブジェクトの更新をする
	//---------------------------------------------------
	void UpdateGameObject();

	//---------------------------------------------------
	///[機能] オブジェクトを描画します
	///[引数] カメラ
	//---------------------------------------------------
	virtual void Render(const class Camera& camera); 


	//---------------------------------------------------
	/// [機能] 影をテクスチャにマップする
	/// [引数] カメラ
	//---------------------------------------------------
	void ShadowMap(const class Camera& camera);

	//---------------------------------------------------
	/// [機能] イベントを送信する
	/// [引数] イベント
	//---------------------------------------------------
	virtual void ReceiveEvent(const class Event& event);


	//---------------------------------------------------
	///[機能] オブジェクトを消してしてよいのか取得
	///[戻り値] オブジェクトを消してよいのか
	//---------------------------------------------------
	bool NeedErase();

	//--------------------------------------------------
	/// [機能] 当たり判定を返す
	//--------------------------------------------------
	const class HitBox& GetHitBox();

	//--------------------------------------------------
	/// ワールドトランスフォームのゲッターセッター
	//--------------------------------------------------
	void SetWorld(const class WorldTransform& world);
	const class WorldTransform& GetWorld();
	void SetPos(const DirectX::XMFLOAT3& pos);
	DirectX::XMFLOAT3 GetPos();


protected:
	
	State mState;                                                  // オブジェクトの状態
	std::shared_ptr<class ObjectResource> mResource;               // 見た目などのリソース
	std::unique_ptr<class WorldTransform> mWorld;                  // ワールドトランスフォーム
	std::unique_ptr<class HitBox> mHitBox;
	
private:

	virtual void Update();
	
	
};

typedef std::shared_ptr<GameObject> GameObjectPtrS;



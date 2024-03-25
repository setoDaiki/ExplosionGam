#pragma once
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "ObjectResource.h"

//--------------------------------------------------------------------------
/// [機能]　第一引数の位置にいるオブジェクトが画面内にいるのかを判定します
//--------------------------------------------------------------------------
bool VFCulling(const DirectX::XMFLOAT3& pos, const DirectX::XMMATRIX& pmView, float Angle, float NearClip, float FarClip, float Aspect); // 視錐台


class Model : public ObjectResource{
public:
	Model(const wchar_t* FileName, bool a = false);
	~Model();
	bool mA;
	
	
	void Render(const DirectX::XMMATRIX& worldMat,const class Camera& camera)override;
	void ShadowMap(const DirectX::XMMATRIX& worldMat,const class Camera& camera);

	

private:
	std::vector<class MyMesh> mMeshes;  // モデルのメッシュ
	std::unique_ptr<class Renderer> mRenderer;

};



typedef std::shared_ptr<Model> ModelPtrS;


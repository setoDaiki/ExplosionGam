#include "Model.h"
#include "Rendering/Mesh.h"
#include "WorldTransform.h"
#include "App.h"
#include "Common/FileUtil.h"
#include "Camera/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/HeapOffset.h"


using namespace DirectX;


Model::Model(const wchar_t* fileName,bool a) 
	:mA(a)
	,mRenderer(new Renderer())
{
	
	LoadMeshesFromFile(mRenderer->GetDevice(), fileName, mMeshes);

	/*for (MyMesh& mesh : mMeshes) {
		const MyMaterial& material = mesh.GetMaterial();
		if (mesh.GetMaterial().hasTexture) {
			mRenderer->CreateTexture(material.texture.Get(), material.textureName, material.textureFormat, mesh.GetSrvHeapOffset());
		}
	}*/
}

Model::~Model() {
	
}


//----------------------------------
//  描画する
//----------------------------------
void Model::Render(const DirectX::XMMATRIX& worldMat, const Camera& camera) {
	for (MyMesh& mesh : mMeshes) {
		mRenderer->RenderMesh(worldMat,camera,mesh);
	}
}

//----------------------------------------------------------------
//  影情報をマップする(影を画面に描画するのはRender関数)
//----------------------------------------------------------------
void Model::ShadowMap(const DirectX::XMMATRIX& worldMat, const Camera& camera) {
	unsigned meshCount = 0;
	for (MyMesh& mesh : mMeshes) {
		mRenderer->ShadowMap(worldMat, camera, mesh);
		
		meshCount++;
	}

}








// 視錐台カリング　コピペしただけだからアルゴリズム分からない
// アルゴリズム考察
// 平面式　= ax + by + cz + d  ですね
// 平面式は平面上の3点があれば求められますね
// XMPlaneFromPoint(float3,float3,float3) 引数に3点いれて、戻り値は平面式の係数
// うーんわからん
bool VFCulling(const XMFLOAT3& pos, const XMMATRIX& pmView, float Angle, float NearClip, float FarClip, float Aspect)
{
	XMFLOAT4 leftPlane, rightPlane, topPlane, bottomPlane;
	float radius = 1.0f;
	XMVECTOR vPos = XMLoadFloat3(&pos);
	//まず、ジオメトリの位置ベクトルをワールドからビュー空間に変換
	vPos = XMVector3TransformCoord(vPos, pmView);

	//左右、上下の平面を計算
	{
		XMFLOAT3 p1(0.0f, 0.0f, 0.0f);
		XMFLOAT3 p2(0.0f, 0.0f, 0.0f);
		XMFLOAT3 p3(0.0f, 0.0f, 0.0f);
		XMVECTOR pln;
		//左面
		p1 = XMFLOAT3(0, 0, 0);
		p2.x = -FarClip * ((FLOAT)tan(Angle / 2) * Aspect);
		p2.y = -FarClip * (FLOAT)tan(Angle / 2);
		p2.z = FarClip;
		p3.x = p2.x;
		p3.y = -p2.y;
		p3.z = p2.z;
		XMVECTOR pv1 = XMLoadFloat3(&p1);
		XMVECTOR pv2 = XMLoadFloat3(&p2);
		XMVECTOR pv3 = XMLoadFloat3(&p3);
		pln = XMPlaneFromPoints(pv1, pv2, pv3);  // plnには平面方程式の係数が入る
		XMStoreFloat4(&leftPlane, pln);
		//右面
		p1 = XMFLOAT3(0, 0, 0);
		p2.x = FarClip * ((FLOAT)tan(Angle / 2) * Aspect);
		p2.y = FarClip * (FLOAT)tan(Angle / 2);
		p2.z = FarClip;
		p3.x = p2.x;
		p3.y = -p2.y;
		p3.z = p2.z;
		pv1 = XMLoadFloat3(&p1);
		pv2 = XMLoadFloat3(&p2);
		pv3 = XMLoadFloat3(&p3);
		pln = XMPlaneFromPoints(pv1, pv2, pv3);
		XMStoreFloat4(&rightPlane, pln);
		//上面
		p1 = XMFLOAT3(0, 0, 0);
		p2.x = -FarClip * ((FLOAT)tan(Angle / 2) * Aspect);
		p2.y = FarClip * (FLOAT)tan(Angle / 2);
		p2.z = FarClip;
		p3.x = -p2.x;
		p3.y = p2.y;
		p3.z = p2.z;
		pv1 = XMLoadFloat3(&p1);
		pv2 = XMLoadFloat3(&p2);
		pv3 = XMLoadFloat3(&p3);
		pln = XMPlaneFromPoints(pv1, pv2, pv3);
		XMStoreFloat4(&topPlane, pln);
		//下面
		p1 = XMFLOAT3(0, 0, 0);
		p2.x = FarClip * ((FLOAT)tan(Angle / 2) * Aspect);
		p2.y = -FarClip * (FLOAT)tan(Angle / 2);
		p2.z = FarClip;
		p3.x = -p2.x;
		p3.y = p2.y;
		p3.z = p2.z;
		pv1 = XMLoadFloat3(&p1);
		pv2 = XMLoadFloat3(&p2);
		pv3 = XMLoadFloat3(&p3);
		pln = XMPlaneFromPoints(pv1, pv2, pv3);
		XMStoreFloat4(&bottomPlane, pln);
	}

	//6つの平面とジオメトリ境界球をチェック
	{
		//ニアクリップ面について
		if ((XMVectorGetZ(vPos) + radius) < NearClip)
		{
			return false;
		}
		//ファークリップ面について
		if ((XMVectorGetZ(vPos) - radius) > FarClip)
		{
			return false;
		}
		//左クリップ面について
		float distance = XMVectorGetX(vPos) * leftPlane.x + XMVectorGetZ(vPos) * leftPlane.z;
		if (distance > radius)
		{
			return false;
		}
		//右クリップ面について
		distance = XMVectorGetX(vPos) * rightPlane.x + XMVectorGetZ(vPos) * rightPlane.z;
		if (distance > radius)
		{
			return false;
		}
		//上クリップ面について
		distance = XMVectorGetY(vPos) * topPlane.y + XMVectorGetZ(vPos) * topPlane.z;
		if (distance > radius)
		{
			return false;
		}
		//下クリップ面について
		distance = XMVectorGetY(vPos) * bottomPlane.y + XMVectorGetZ(vPos) * bottomPlane.z;
		if (distance > radius)
		{
			return false;
		}
	}
	return true;
}





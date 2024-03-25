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
//  �`�悷��
//----------------------------------
void Model::Render(const DirectX::XMMATRIX& worldMat, const Camera& camera) {
	for (MyMesh& mesh : mMeshes) {
		mRenderer->RenderMesh(worldMat,camera,mesh);
	}
}

//----------------------------------------------------------------
//  �e�����}�b�v����(�e����ʂɕ`�悷��̂�Render�֐�)
//----------------------------------------------------------------
void Model::ShadowMap(const DirectX::XMMATRIX& worldMat, const Camera& camera) {
	unsigned meshCount = 0;
	for (MyMesh& mesh : mMeshes) {
		mRenderer->ShadowMap(worldMat, camera, mesh);
		
		meshCount++;
	}

}








// ������J�����O�@�R�s�y��������������A���S���Y��������Ȃ�
// �A���S���Y���l�@
// ���ʎ��@= ax + by + cz + d  �ł���
// ���ʎ��͕��ʏ��3�_������΋��߂��܂���
// XMPlaneFromPoint(float3,float3,float3) ������3�_����āA�߂�l�͕��ʎ��̌W��
// ���[��킩���
bool VFCulling(const XMFLOAT3& pos, const XMMATRIX& pmView, float Angle, float NearClip, float FarClip, float Aspect)
{
	XMFLOAT4 leftPlane, rightPlane, topPlane, bottomPlane;
	float radius = 1.0f;
	XMVECTOR vPos = XMLoadFloat3(&pos);
	//�܂��A�W�I���g���̈ʒu�x�N�g�������[���h����r���[��Ԃɕϊ�
	vPos = XMVector3TransformCoord(vPos, pmView);

	//���E�A�㉺�̕��ʂ��v�Z
	{
		XMFLOAT3 p1(0.0f, 0.0f, 0.0f);
		XMFLOAT3 p2(0.0f, 0.0f, 0.0f);
		XMFLOAT3 p3(0.0f, 0.0f, 0.0f);
		XMVECTOR pln;
		//����
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
		pln = XMPlaneFromPoints(pv1, pv2, pv3);  // pln�ɂ͕��ʕ������̌W��������
		XMStoreFloat4(&leftPlane, pln);
		//�E��
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
		//���
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
		//����
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

	//6�̕��ʂƃW�I���g�����E�����`�F�b�N
	{
		//�j�A�N���b�v�ʂɂ���
		if ((XMVectorGetZ(vPos) + radius) < NearClip)
		{
			return false;
		}
		//�t�@�[�N���b�v�ʂɂ���
		if ((XMVectorGetZ(vPos) - radius) > FarClip)
		{
			return false;
		}
		//���N���b�v�ʂɂ���
		float distance = XMVectorGetX(vPos) * leftPlane.x + XMVectorGetZ(vPos) * leftPlane.z;
		if (distance > radius)
		{
			return false;
		}
		//�E�N���b�v�ʂɂ���
		distance = XMVectorGetX(vPos) * rightPlane.x + XMVectorGetZ(vPos) * rightPlane.z;
		if (distance > radius)
		{
			return false;
		}
		//��N���b�v�ʂɂ���
		distance = XMVectorGetY(vPos) * topPlane.y + XMVectorGetZ(vPos) * topPlane.z;
		if (distance > radius)
		{
			return false;
		}
		//���N���b�v�ʂɂ���
		distance = XMVectorGetY(vPos) * bottomPlane.y + XMVectorGetZ(vPos) * bottomPlane.z;
		if (distance > radius)
		{
			return false;
		}
	}
	return true;
}





#pragma once
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "ObjectResource.h"

//--------------------------------------------------------------------------
/// [�@�\]�@�������̈ʒu�ɂ���I�u�W�F�N�g����ʓ��ɂ���̂��𔻒肵�܂�
//--------------------------------------------------------------------------
bool VFCulling(const DirectX::XMFLOAT3& pos, const DirectX::XMMATRIX& pmView, float Angle, float NearClip, float FarClip, float Aspect); // ������


class Model : public ObjectResource{
public:
	Model(const wchar_t* FileName, bool a = false);
	~Model();
	bool mA;
	
	
	void Render(const DirectX::XMMATRIX& worldMat,const class Camera& camera)override;
	void ShadowMap(const DirectX::XMMATRIX& worldMat,const class Camera& camera);

	

private:
	std::vector<class MyMesh> mMeshes;  // ���f���̃��b�V��
	std::unique_ptr<class Renderer> mRenderer;

};



typedef std::shared_ptr<Model> ModelPtrS;


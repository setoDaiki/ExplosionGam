#include "Camera.h"
#include "App.h"
#include "Common/MyMath.h"
#include "CameraMovement.h"

Camera::Camera(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& lookPos,std::unique_ptr<CameraMovement> cameraMovement)
	:mMovement(std::move(cameraMovement))
	,mPosition(cameraPos)
	, mLookPos(lookPos)
	, mViewMat()
	, mProjMat()
	, mFov(DirectX::XM_PI / 4)
	, mNearClip(0.3f)
	, mFarClip(1000.0f)
	, mAspect(static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight))
{
	UpdateViewMat();
	UpdateProjMat();
}

//--------------------------------------
//  �J�����̍X�V
//--------------------------------------
void Camera::Update() {
	mMovement->Excute(mPosition,mLookPos);
	UpdateViewMat();
}


//--------------------------------------
//  View�s��̍X�V
//--------------------------------------
void Camera::UpdateViewMat() {
	DirectX::XMFLOAT3 pos = mPosition;   // �J�����ʒu�̎擾
	DirectX::XMFLOAT3 look = mLookPos;   // �����ʒu�̎擾
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0.0f);    // XMFLOAT3�^��XMVECTOR�ɕϊ�
	DirectX::XMVECTOR lookPos = DirectX::XMVectorSet(look.x, look.y, look.z, 0.0f);   // XMFLOAT3�^��XMVECTOR�ɕϊ�
	DirectX::XMVECTOR upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);       //�J�����̏���x�N�g��
	mViewMat = DirectX::XMMatrixLookAtLH(eyePos, lookPos, upward);   // �r���[�s��̐ݒ�
}

//--------------------------------------
//  Projection�s��̍X�V
//--------------------------------------
void Camera::UpdateProjMat() {
	
	mProjMat = DirectX::XMMatrixIdentity();
	mProjMat = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspect, mNearClip, mFarClip);

}


//------------------------------------------
//  Projection�s��̍\���v�f�̍\���̂��擾
//------------------------------------------
ProjectionComponents Camera::GetProjectionComponents()const {
	ProjectionComponents projComp;
	projComp.farClip = mFarClip;
	projComp.nearClip = mNearClip;
	projComp.fov = mFov;
	projComp.aspect = mAspect;
	return projComp;
}
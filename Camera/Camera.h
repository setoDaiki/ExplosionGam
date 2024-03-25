#pragma once
#include <DirectXmath.h>
#include <memory>


struct ProjectionComponents {
	float nearClip;
	float farClip;
	float fov;
	float aspect;
};

//======================================================
/// [�@�\] �Q�[�����[���h�̃J�����̖���������N���X�ł�
//======================================================
class Camera {
public:
	

	Camera(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& lookPos,std::unique_ptr<class CameraMovement> cameraMovement);
	

	//-------------------------------------
	/// [�@�\]�J�����̏�Ԃ̍X�V
	//-------------------------------------
	void Update();
	

	//------------------------------------------
	/// [�@�\] �r���[�s��̎擾
	/// [�Ԃ�l] �r���[�s��
	//------------------------------------------
	const DirectX::XMMATRIX& GetViewMat()const { return mViewMat; }

	//------------------------------------------
	/// [�@�\] �v���W�F�N�V�����s��̎擾
	/// [�Ԃ�l] �v���W�F�N�V�����s��
	//------------------------------------------
	const DirectX::XMMATRIX& GetProjMat()const { return mProjMat; }

	//-----------------------------------------
	/// [�@�\] �J�����̈ʒu���擾���܂�
	/// [�Ԃ�l] �J�����̈ʒu�ł�
	//-----------------------------------------
	DirectX::XMFLOAT3 GetPos()const { return mPosition; }

	//----------------------------------------------
	/// [�@�\] �v���W�F�N�V�����s��̍\���v�f�̎擾
	/// [�Ԃ�l] �v���W�F�N�V�����s��̍\���v�f
	//-----------------------------------------------
	ProjectionComponents GetProjectionComponents()const;


private:
	//-----------------------------------------
	/// [�@�\] �r���[�s��̍X�V
	//-----------------------------------------
	void UpdateViewMat();

	//-----------------------------------------
	/// [�@�\] �v���W�F�N�V�����s��̍X�V
	//-----------------------------------------
	void UpdateProjMat();


	std::unique_ptr<class CameraMovement> mMovement;
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mLookPos;
	DirectX::XMMATRIX mViewMat;
	DirectX::XMMATRIX mProjMat;
	float mFov;
	float mNearClip;
	float mFarClip;
	float mAspect;               
};

typedef std::unique_ptr<Camera> CameraPtrU;

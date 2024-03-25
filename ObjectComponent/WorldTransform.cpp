#include "WorldTransform.h"
#include "Common/MyMath.h"


using namespace DirectX;

WorldTransform::WorldTransform(
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& scale,
	const DirectX::XMVECTOR& quaternion
	)
	:mPos(pos)
	,mQuaternion(quaternion)
	,mScale(scale)
	,mWasChanged(true)
	,mWorldMat()
	
{

	UpdateWorldMat();
	
}

WorldTransform::~WorldTransform() {

}

//-------------------------------------------------
// �ʒu�̃Z�b�^�[�A�Q�b�^�[
//-------------------------------------------------
void WorldTransform::SetPos(const XMFLOAT3& pos) {
	mPos = pos;
	mWasChanged = true;
}

const XMFLOAT3& WorldTransform::GetPos()const {
	return mPos;
}

//------------------------------------------------
//   �p�x�̃Z�b�^�[�A�Q�b�^�[
//------------------------------------------------
void WorldTransform::SetRot(const XMVECTOR& rot) {
	mQuaternion = rot;
	mWasChanged = true;
}

const XMVECTOR& WorldTransform::GetRot()const {
	return mQuaternion;
}

//------------------------------------------------
//   �g�嗦�̃Z�b�^�[�A�Q�b�^�[
//------------------------------------------------
void WorldTransform::SetScale(const XMFLOAT3& scale) {
	mScale = scale;
	mWasChanged = true;
}

const XMFLOAT3& WorldTransform::GetScale()const {
	return mScale;
}

//------------------------------------------------
//   �ʒu�x�N�g���Ƀx�N�g���𑫂�
//------------------------------------------------
void WorldTransform::AddPos(const XMFLOAT3& pos) {
	mPos = mPos + pos;
	mWasChanged = true;
}


//------------------------------------------------
//   ��]�x�N�g���Ƀx�N�g���𑫂�
//------------------------------------------------
void WorldTransform::AddRot(const XMVECTOR& quaternion) {
	mQuaternion = XMQuaternionMultiply(quaternion,mQuaternion);
	mWasChanged = true;
}

//------------------------------------------------
//   �g�嗦�x�N�g���Ƀx�N�g���𑫂�
//------------------------------------------------
void WorldTransform::AddScale(const XMFLOAT3& scale) {
	mScale = mScale + scale;
	mWasChanged = true;
}



//----------------------------------------------
//   ���[���h�s��̎擾
//----------------------------------------------
const DirectX::XMMATRIX& WorldTransform::GetWorldMat() {
	return mWorldMat;
}


//-----------------------------------------------
//   ���[���h�s��̍X�V
//-----------------------------------------------
void WorldTransform::UpdateWorldMat() {
	if (!mWasChanged) return;

	// ���[���h���W�̍X�V�p�̍s��̏���
	XMMATRIX scaleMat;
	scaleMat = XMMatrixScaling(mScale.x, mScale.y, mScale.z);   // �X�P�[���s��
	XMMATRIX quaternionMat;
	quaternionMat = XMMatrixRotationQuaternion(mQuaternion);    // ��]�s��
	XMMATRIX translation;
	translation = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);  // �ʒu�s��

		
	// ���[���h���W�X�V
	mWorldMat = XMMatrixIdentity();
	mWorldMat = scaleMat * quaternionMat * translation;
	mWasChanged = false;
}

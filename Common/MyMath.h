#pragma once
#include "d3dx12.h"
#include <directXmath.h>
#include <vector>


//==============================================================================================
/// [�o���邱�ƃ��X�g]
/// 3�����x�N�g���̉��Z�A���Z�A�X�J���[��Z�A�X�J���[���Z�A�x�N�g���̒����擾�A�x�N�g���̐��K��
/// �O���x�N�g���v�Z�A����x�N�g���v�Z�A�E�����x�N�g���̌v�Z�A�������x�N�g���̌v�Z
/// float�̐�Βl�擾�Afloat�̂قڈ�v�̔��f
//==============================================================================================


//----------------------------------------------------------------------------
/// �x�N�g���̌v�Z
//----------------------------------------------------------------------------
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator*(float scaler, const DirectX::XMFLOAT3& vector);
const DirectX::XMFLOAT3 operator/(float scaler, const DirectX::XMFLOAT3& vector);


namespace Math {

	//---------------------------------------------
	/// [�@�\] �x�N�g���̒����̎擾
	/// [����] 3�����x�N�g��
	/// [�Ԃ�l] �x�N�g���̒���
	//---------------------------------------------
	float GetLenght(const DirectX::XMFLOAT3& vector);

	//---------------------------------------------
	/// [�@�\] �x�N�g���̐��K��
	/// [����] 3�����x�N�g��
	/// [�Ԃ�l] ���K�����ꂽ�x�N�g��
	//---------------------------------------------
	DirectX::XMFLOAT3 GetNormalize(const DirectX::XMFLOAT3& vector);


	//---------------------------------------------
	/// [�@�\] �x�N�g���̓��ς̎擾
	/// [����1] 3�����x�N�g��1��
	/// [����2] 3�����x�N�g��2��
	/// [�߂�l] �x�N�g���̓���
	//---------------------------------------------
	constexpr float GetDot(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);

	//----------------------------------------------
	/// [�@�\] ���g����ڕW���ւ̊p�x�����߂�
	/// [����1] �����̈ʒu�x�N�g��
	/// [����2] �ڕW���̈ʒu�x�N�g��
	/// [�߂�l] �ڕW���̂���p�x
	//----------------------------------------------
	DirectX::XMFLOAT3 GetAngleToTarget(const DirectX::XMFLOAT3& ownVector, const DirectX::XMFLOAT3& targetVector);


	//---------------------------------------------
	/// [�@�\] XMVECTOR�^��XMFLOAT3�^�ɕϊ����܂�
	/// [����] �ϊ��������x�N�g���ł�
	/// [�Ԃ�l] �ϊ����ꂽXMFLOAT3�ł�
	//---------------------------------------------
	constexpr DirectX::XMFLOAT3 ConvertVectorToFloat3(const DirectX::XMVECTOR& vector);

	//---------------------------------------------
	/// [�@�\] �O���x�N�g�����N�H�[�^�j�I���Ŏ擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] �O���x�N�g���ł�
	//---------------------------------------------
	DirectX::XMFLOAT3 GetFront(const DirectX::XMVECTOR& quaternion);


	//----------------------------------------
	/// [�@�\] ����x�N�g�����擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ����x�N�g���ł�
	//----------------------------------------
	DirectX::XMFLOAT3 GetBack(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------
	/// [�@�\] �E�x�N�g�����擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] �E�x�N�g���ł�
	//----------------------------------------
	DirectX::XMFLOAT3 GetRight(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------
	/// [�@�\] ���x�N�g�����擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ���x�N�g���ł�
	//----------------------------------------
	DirectX::XMFLOAT3 GetLeft(const DirectX::XMVECTOR& quaternion);


	//-----------------------------------------------
	/// [�@�\] �x���@�ŉ�]�l�������擾���܂�
	//-----------------------------------------------
	DirectX::XMVECTOR QuaternionRotateEuler(float pitch, float yaw, float roll);
	DirectX::XMVECTOR QuaternionRotateAxis(const DirectX::XMVECTOR& axis, float degreeAngle);


	//-------------------------------------------------------------------
	/// [�@�\] �I�C���[�p�ŉ�]�����A�\�[�X�̃N�H�[�^�j�I���Ə�Z���܂�
	/// [����1,2,3] ���[���A�s�b�`�A���[������̉�]�ʂł�(�x���@) 
	/// [����4] �Z�o���ꂽ�N�H�[�^�j�I���Ə�Z�������N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ��Z���ʂł�
	//--------------------------------------------------------------------
	DirectX::XMVECTOR QuaternionRotateEulerAndMultiply(float pitch, float yaw, float roll, const DirectX::XMVECTOR& srcQuaternion);

	//-----------------------------------------------------------------
	/// [�@�\] �C�ӎ��ŉ�]�����A�\�[�X�̃N�H�[�^�j�I���Ə�Z���܂�
	/// [����1] ��]���ł�
	/// [����2] ��]��(�x���@)
	/// [����2] �Z�o���ꂽ�N�H�[�^�j�I���Ə�Z�������N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ��Z���ʂł�
	//-----------------------------------------------------------------
	DirectX::XMVECTOR QuaternionRotateAxisAndMultiply(const DirectX::XMVECTOR& axis, float degreeAngle, const DirectX::XMVECTOR& srcQuaternion);


	//---------------------------------------------------------------
	/// [�@�\] ��̈������قړ����Ȃ�true��Ԃ��܂�(foat�p)
	/// [����1] ��ׂ������l�ł�
	/// [����2] ����1�Ɣ�ׂ鐔�l�ł�
	/// [�Ԃ�l] ����1�ƈ���2���قړ����Ȃ�true��Ԃ��܂�
	//---------------------------------------------------------------
	constexpr bool NearEqualF(float num1, float num2);

	//----------------------------------------------------------------
	/// [�@�\] �������擾����
	/// [����] �������擾���������l
	/// [�߂�l] ����
	//----------------------------------------------------------------
	int NumDigits(int num);

	//----------------------------------------------------------------
	/// [�@�\] �w�肵�����̐��l���擾����
	/// [����1] ���o���̐��l
	/// [����2] ���l���擾����������
	/// [�߂�l] �w�肳�ꂽ���̐��l
	//----------------------------------------------------------------
	int GetNumOfDigit(int num, int digit);
 
}

#include "MyMath.h"
#include <cmath>


using namespace DirectX;

//------------------------------
//   �x�N�g���̉��Z
//------------------------------
const XMFLOAT3 operator+(const XMFLOAT3& vector1,const XMFLOAT3& vector2) {
	XMFLOAT3 returnVec;
	returnVec.x = vector1.x + vector2.x;
	returnVec.y = vector1.y + vector2.y;
	returnVec.z = vector1.z + vector2.z;
	return returnVec;
}

//-------------------------------
//  �x�N�g���̌��Z
//-------------------------------
const XMFLOAT3 operator-(const XMFLOAT3& vector1,const XMFLOAT3& vector2) {
	XMFLOAT3 returnVec;
	returnVec.x = vector1.x - vector2.x;
	returnVec.y = vector1.y - vector2.y;
	returnVec.z = vector1.z - vector2.z;
	return returnVec;
}

//--------------------------------
//   �x�N�g���ƃX�J���[�̏�Z
//--------------------------------
const DirectX::XMFLOAT3 operator*(float scaler, const DirectX::XMFLOAT3& vector) {
	XMFLOAT3 returnVec;
	returnVec.x = vector.x * scaler;
	returnVec.y = vector.y * scaler;
	returnVec.z = vector.z * scaler;
	return returnVec;
}

//---------------------------------
//   �x�N�g���ƃX�J���[�̏��Z
//---------------------------------
const XMFLOAT3 operator/(float scaler,const XMFLOAT3& vector) {
	assert(scaler == 0 || !("0���Z�_���BMath::DivScaler"));
	XMFLOAT3 returnVec;
	returnVec.x = vector.x / scaler;
	returnVec.y = vector.y / scaler;
	returnVec.z = vector.z / scaler;
	return returnVec;
}


//----------------------------------------
//  �x�N�g���̒����̎擾
//----------------------------------------
float Math::GetLenght(const XMFLOAT3& vector) {
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

//--------------------------------------
//   �x�N�g���̐��K��
//--------------------------------------
XMFLOAT3 Math::GetNormalize(const XMFLOAT3& vector) {
	float lenght = Math::GetLenght(vector);
	XMFLOAT3 returnVec;
	returnVec.x = vector.x / lenght;
	returnVec.y = vector.y / lenght;
	returnVec.z = vector.z / lenght;
	return returnVec;
}

//-----------------------------------
//  �x�N�g���̓��ς̎擾
//-----------------------------------
constexpr float Math::GetDot(const XMFLOAT3& vector1,const XMFLOAT3& vector2) {
	return (vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z);
}

//------------------------------------
//  ���g����ڕW���ւ̊p�x�����߂�
//------------------------------------
DirectX::XMFLOAT3 Math::GetAngleToTarget(const DirectX::XMFLOAT3& ownVector, const DirectX::XMFLOAT3& targetVector) {
	DirectX::XMFLOAT3 ownToTargetVec = targetVector - ownVector;
	DirectX::XMFLOAT3 xVec(1.0f, 0.0f, 0.0f);
	ownToTargetVec.y = 0.0f;
	DirectX::XMFLOAT3 normalizeOwnToTargetVec = Math::GetNormalize(ownToTargetVec);
	float ownToTargetRot = std::acos(Math::GetDot(normalizeOwnToTargetVec, xVec));
	ownToTargetRot = DirectX::XMConvertToDegrees(ownToTargetRot);
	if (ownVector.z < targetVector.z) ownToTargetRot *= -1.0f;
	return DirectX::XMFLOAT3(0.0f, ownToTargetRot, 0.0f);
}

//--------------------------------------
//  XMVECTOR�^��XMFLOAT�^�ɕϊ�
//--------------------------------------
constexpr DirectX::XMFLOAT3 Math::ConvertVectorToFloat3(const XMVECTOR& vector) {
	return XMFLOAT3 (vector.m128_f32[0],vector.m128_f32[1],vector.m128_f32[2]);
}



//-------------------------------------
//   �O���x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Math::GetFront(const XMVECTOR& quaternion) {
	XMVECTOR xAxis = XMVectorSet(-1.0f,0.0f,0.0f,1.0f);
	XMVECTOR frontVector = XMVector3Rotate(xAxis, quaternion);
	return XMFLOAT3(frontVector.m128_f32[0], frontVector.m128_f32[1], frontVector.m128_f32[2]);
}

//-------------------------------------
//   ����x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Math::GetBack(const XMVECTOR& quaternion) {
	XMFLOAT3 front = Math::GetFront(quaternion);
	return XMFLOAT3(- 1 * front);
}

//-------------------------------------
//   �E�x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Math::GetRight(const DirectX::XMVECTOR& quaternion) {
	XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR rightVector = XMVector3Rotate(zAxis, quaternion);
	return XMFLOAT3(rightVector.m128_f32[0], rightVector.m128_f32[1], rightVector.m128_f32[2]);
}

//-------------------------------------
//   ���x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Math::GetLeft(const XMVECTOR& quaternion) {
	XMFLOAT3 right = Math::GetRight(quaternion);
	return XMFLOAT3(- 1 * right);
}


//--------------------------------------------
//  ��]�l������x���@�Ŏ擾
//--------------------------------------------
XMVECTOR Math::QuaternionRotateEuler(float pitch,float yaw ,float roll) {
	pitch = XMConvertToRadians(pitch);
	yaw = XMConvertToRadians(yaw);
	roll = XMConvertToRadians(roll);
	return XMQuaternionRotationRollPitchYaw(pitch,yaw ,roll);
}


XMVECTOR Math::QuaternionRotateAxis(const DirectX::XMVECTOR& axis, float angleDegree) {
	float angleRadian = XMConvertToRadians(angleDegree);
	return XMQuaternionRotationAxis(axis,angleDegree);
}


//--------------------------------------------
//  �N�H�[�^�j�I���̉�]�Ə�Z
//--------------------------------------------
XMVECTOR Math::QuaternionRotateEulerAndMultiply(float pitch, float yaw, float roll, const XMVECTOR& srcQuaternion) {
	XMVECTOR quaternion = QuaternionRotateEuler(pitch,yaw,roll);
	XMVECTOR destQuaternion = XMQuaternionMultiply(srcQuaternion, quaternion);
	return destQuaternion;
}

XMVECTOR Math::QuaternionRotateAxisAndMultiply(const XMVECTOR& axis, float degreeAngle, const XMVECTOR& srcQuaternion) {
	XMVECTOR quaternion = QuaternionRotateAxis(axis,degreeAngle);
	XMVECTOR destQuaternion = XMQuaternionMultiply(srcQuaternion, quaternion);
	return destQuaternion;
}



//----------------------------------------
//   ��̐��l���قړ��������ׂ�
//----------------------------------------
constexpr bool Math::NearEqualF(float num1,float num2) {
	float error = num1 - num2;  // �덷�̎擾
	if (error < 0.01f && error > -0.01f) {  // �덷0.01�����Ȃ�
		return true;  // true��Ԃ�
	}
	return false; // �덷��0.01���傫���Ȃ�false��Ԃ�
}


//-----------------------------------------
//  �������擾����
//-----------------------------------------
int Math::NumDigits(int num) {
	if (num == 0) return 1;
	return static_cast<int>(std::log10(num) + 1);
}

//-----------------------------------------
// �w�肵�����̐��l���擾����
//-----------------------------------------
int Math::GetNumOfDigit(int num, int digit) {
	assert(num >= 0);
	if(NumDigits(num) < digit) return 0;  // num�̌������w�肳�ꂽ���������Ȃ��Ȃ�0���҂�
	int divisor = static_cast<int>(std::pow(10, digit));
	int numUpToThisDigit = (num % divisor);
	int numOfThisDigit = numUpToThisDigit / static_cast<int>((std::pow(10, digit - 1)));
	return numOfThisDigit;
}

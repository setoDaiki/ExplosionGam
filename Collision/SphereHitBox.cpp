#include "SphereHitBox.h"
#include "Common/MyMath.h"
#include "Collision.h"
#include "GameObject/GameObject.h"
#include "Common/Printer.h"

SphereHitBox::SphereHitBox(const DirectX::XMFLOAT3& ownerPos,float radius)
	:HitBox(ownerPos)
	,mRadius(radius)
{
	
}

SphereHitBox::~SphereHitBox() {
}


//---------------------------------
//   ���Ƌ��̏Փ˔���
//---------------------------------
bool SphereHitBox::IsHitWithSphere(const SphereHitBox& sphere)const {
	if (&sphere == this) {  // �����Ǝ����̏Փ˂ł�
		return false;       // �Փ˂��ĂȂ����Ƃɂ���
	}
	const XMFLOAT3& pos = sphere.GetPos();
	float radius = sphere.GetRadius();
	const XMFLOAT3 subVec = pos - GetPos();   // ���S�ԋ���
	float lenght2 = subVec.x * subVec.x + subVec.y * subVec.y + subVec.z * subVec.z;  // ���S�ԋ����̓��
	// ���a�̘a�̓��ƒ��S�ԋ���2��ŏՓˌ��m
	if (lenght2 < ((radius + mRadius) * (radius * mRadius))) {
		return true;  // �Փ˂��Ă���
		
	}
	return false;  
}
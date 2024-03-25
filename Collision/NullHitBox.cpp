#include "NullHitBox.h"
#include "SphereHitBox.h"

NullHitBox::NullHitBox() 
	:HitBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f))
{

}

bool NullHitBox::IsHitWithSphere(const SphereHitBox& hitBox)const {
	return false;
}
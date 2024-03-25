#pragma once
#include <vector>
#include <DirectXMath.h>
#include <memory>

class NumberUI {
public:
	NumberUI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale);
	NumberUI(const NumberUI&) = default;

	//--------------------------------------------------------------------
	/// [�@�\] �������Ŏw�肵���ʒu�ɑ�O�����Ŏw�肵�����l��`�悷��
	//--------------------------------------------------------------------
	void Render(const class Camera& camera,int num);

private:
	DirectX::XMFLOAT3 mPos;
	DirectX::XMFLOAT3 mScale;
	static std::vector<class UI> mNumberUIs;
};

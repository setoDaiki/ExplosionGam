#pragma once
#include <vector>
#include <DirectXMath.h>
#include <memory>

class NumberUI {
public:
	NumberUI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale);
	NumberUI(const NumberUI&) = default;

	//--------------------------------------------------------------------
	/// [機能] 第二引数で指定した位置に第三引数で指定した数値を描画する
	//--------------------------------------------------------------------
	void Render(const class Camera& camera,int num);

private:
	DirectX::XMFLOAT3 mPos;
	DirectX::XMFLOAT3 mScale;
	static std::vector<class UI> mNumberUIs;
};

#include "UI.h"
#include "Rendering/Mesh.h"
#include "Rendering/Renderer.h"
#include "Camera/Camera.h"

UI::UI(const std::wstring& textureName) 
	:mMesh(new MyMesh())
	,mRenderer(new Renderer())
{
	mMesh->CreateSquareMesh(mRenderer->GetDevice(), textureName);
}


UI::~UI(){

}


UI::UI(UI&& ui) 
	:mMesh(std::move(ui.mMesh))
	,mRenderer(std::move(ui.mRenderer))
{

}


void UI::Render(const DirectX::XMMATRIX& worldMat,const Camera& camera) {
	mRenderer->RenderUI(worldMat,camera,*mMesh.get());
}
#include "Sprite.h"
#include "Rendering/Renderer.h"
#include "Rendering/Mesh.h"
#include "Rendering/HeapOffset.h"
#include <iostream>


Sprite::Sprite(const std::wstring& textureName) 
	:mMesh(new MyMesh())
{
	CreateSprite(textureName);
}

Sprite::~Sprite() {
	
}

void Sprite::CreateSprite(const std::wstring& textureName) {
	mMesh->CreateSquareMesh(mRenderer->GetDevice(),textureName);
}


void Sprite::Render(const DirectX::XMMATRIX& worldMat, const class Camera& camera) {
	mRenderer->RenderSprite(worldMat,camera,*mMesh.get());
}
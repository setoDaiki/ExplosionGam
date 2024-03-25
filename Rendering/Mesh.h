#pragma once

#include <dxgi1_4.h>
#include "ComPtr.h"
#include <windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <memory>



#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"DirectXTK12.lib") 

using namespace std;


// �}�e���A���\����
struct MyMaterial
{
	DirectX::XMFLOAT4 ambient;        // �A���r�G���g
	DirectX::XMFLOAT4 diffuse;        // �f�B�t���[�Y
	DirectX::XMFLOAT4 specular;       // �X�y�L�����[
	std::wstring textureName;         // �e�N�X�`���[�t�@�C����
	DXGI_FORMAT textureFormat;        // �e�N�X�`���t�H�[�}�b�g
	ComPtr<ID3D12Resource> texture;   // �e�N�X�`�����\�[�X
	bool hasTexture;                  // ���̃}�e���A�����e�N�X�`���������Ă���̂�
	MyMaterial()
		:hasTexture(false)
		,ambient(DirectX::XMFLOAT4(0.0f,0.0f,0.0f,1.0f))
		,diffuse(DirectX::XMFLOAT4(0.0f,0.0f,0.0f,1.0f))
		,specular(DirectX::XMFLOAT4(0.0f,0.0f,0.0f,1.0f))
		,textureName()
		,textureFormat(DXGI_FORMAT_UNKNOWN)
	{
		
	}
	~MyMaterial() {

	}
};

// mesh�v�f�\����
class  MyMesh {

public:
	//----------------------------------------------------------------------------------
	/// [�@�\] ��������mesh�̃��[�h�A1���f���ɕ�����mesh���܂މ\��������̂ŃR���e�i
	/// [����1] �f�o�C�X�A�h���X
	/// [����2] ���[�h����t�@�C����
	/// [����3] mesh�R���e�i 
	//-----------------------------------------------------------------------------------
	friend void LoadMeshesFromFile(ID3D12Device* pDevice, const wchar_t* FileName, std::vector<class MyMesh>& mesh);

private:
	friend HRESULT LoadMesh(ID3D12Device* device, class MyMesh& dst, const struct aiMesh* src);
	friend HRESULT LoadPmdMeshes(ID3D12Device* device, const wchar_t* filename, std::vector<class MyMesh>& meshes);


public:
	MyMesh();
	~MyMesh();

	MyMesh(MyMesh&&) = default;

	//----------------------------------------------------------------
	/// [�@�\] �l�p�`���b�V�������
	/// [����1] �f�o�C�X�A�h���X
	/// [����2] �l�p�`�ɒ���t����e�N�X�`����
	//----------------------------------------------------------------
	void CreateSquareMesh(ID3D12Device* device, const std::wstring& textureName);	


	const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView()const;
	const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView()const;
	MyMaterial GetMaterial()const;
	DWORD GetNumTriangles()const;
	unsigned GetSrvHeapOffset()const;

private:
	


	DWORD mNumTriangles;                              // �O�p�`��
	std::unique_ptr<class HeapOffset> mSrvHeapOffset; // �V�F�_�[���\�[�X�̃q�[�v�I�t�Z�b�g
	ComPtr<ID3D12Resource> mVertexBuffer;             // ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;       // ���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> mIndexBuffer;              // �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;         // �C���f�b�N�X�o�b�t�@�r���[
	MyMaterial mMaterial;                             // �}�e���A��
	static ComPtr<ID3D12Resource> mBlackTexture;
	static class HeapOffset* mBlackTextureHeapOffset;
};


//���_�̍\����
struct MyVertex
{
	DirectX::XMFLOAT3 pos;     // �ʒu
	DirectX::XMFLOAT3 normal;  // �@��
	DirectX::XMFLOAT2 uv;      // uv

};




//----------------------------------------------------------------------
/// [�@�\] �e�N�X�`���t�@�C���̊g���q���擾����(png,jpeg,bmp)
/// [�@�\] �p�X���������ĂĂ����Ȃ�
/// [����] �e�N�X�`���t�@�C����
//----------------------------------------------------------------------
DXGI_FORMAT GetTextureFileFormat(const std::wstring& textureFileName);
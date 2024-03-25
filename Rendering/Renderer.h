#pragma once
#include <windows.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "ComPtr.h"
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"Gdi32.lib") 
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"DirectXTK12.lib") 

//�R���X�^���g�o�b�t�@
struct Cbuffer
{
	DirectX::XMMATRIX w;          // ���[���h�s��
	DirectX::XMMATRIX wvp;        // ���[���h����ˉe�܂ł̕ϊ��s��
	DirectX::XMMATRIX wp;         // ���[���h * �v���W�F�N�V���� (UI�p)
	DirectX::XMMATRIX wlp;        // ���[���h * ���C�g�r���[ * �v���W�F�N�V����
	DirectX::XMMATRIX wlpt;       // ���[���h * ���C�g�r���[ * �v���W�F�N�V���� * UV
	DirectX::XMFLOAT4 lightPos;   // ���C�g�ʒu
	DirectX::XMFLOAT4 eye;        // ���_�ʒu
	DirectX::XMFLOAT4 ambient;    // ����
	DirectX::XMFLOAT4 diffuse;    // �g�U���ˌ�
	DirectX::XMFLOAT4 specular;   // ���ʔ��ˌ�
	Cbuffer()
	{
		ZeroMemory(this, sizeof(Cbuffer));
	}
};


class Renderer {
public:
	static HRESULT CreateRenderObject(HWND* hWnd);
	static void BeginRender();
	static void EndRender();

	Renderer();
	void CreateTexture(ID3D12Resource* resource, std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset);   // �e�N�X�`�������
	
	void ShadowMap(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);
	void RenderMesh(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);
	void RenderSprite(const DirectX::XMMATRIX& worldMat, const class Camera& camera,const class MyMesh& mesh);
	void RenderUI(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);

	// ���̊֐��͕s�׍H������̂Ō�ŕς�����
	static ID3D12Device* GetDevice() {
		return mDevice.Get();
	}

private:
	static void WaitGpu();                                    // GPU�Ƃ̓���

	void Render(const DirectX::XMMATRIX& worldMat, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat, const DirectX::XMFLOAT3& eyeVec, const class MyMesh& mesh);

	// �`��C���^�[�t�F�[�X
	static UINT64 mFenceValue;                                // �t�F���X�l
	static UINT mRTVStride;                                   // �����_�\�^�[�Q�b�g�r���[�������̑傫��
	static UINT mCBV_SRV_Stride;                              // �����_�\�^�[�Q�b�g�r���[�ƃR���X�^���g�o�b�t�@�\�r���[�������̑傫��
	static UINT mCBSize;                                      // �R���X�^���g�o�b�t�@�\�������̑傫��
	static const int FrameCount = 2;                          // �t���C���̐�(�L�����o�X�̐�?)
	static UINT* mCBVDataBegin;
	static ComPtr<IDXGISwapChain3> mSwapChain;                // �X���b�v�`�F�[��
	static ComPtr<ID3D12Device> mDevice;                      // �f�o�C�X
	static ComPtr<ID3D12CommandAllocator> mCommandAllocator;  // �R�}���h�A���P�[�^
	static ComPtr<ID3D12CommandQueue> mCommandQueue;          // �R�}���h�L���[
	static ComPtr<ID3D12GraphicsCommandList> mCommandList;    // �R�}���h���X�g
	static ComPtr<ID3D12Fence> mFence;                        // �t�F���X
	static unsigned mShadowTextureHeapOffset;                 // �V���h�E�}�b�v�p�e�N�X�`���̃q�[�v�ʒu
	static ComPtr<ID3D12RootSignature> mRootSignature;        // ���[�g�V�O�l�`��

	// �p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> mPipelineState;        // �p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> mPipelineUI;           // UI�p�p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> mPipelineSprite;       // �p�C�v���C���X�e�[�g�A�X�v���C�g�p
	static ComPtr<ID3D12PipelineState> mPipelineWriteShadow;  // �V���h�E�}�b�v�������ݗp�p�C�v���C��
	static ComPtr<ID3D12PipelineState> mPipelineReadShadow;   // �V���h�E�}�b�v�ǂݍ��ݗp


	// CBV SRV�n
	static ComPtr<ID3D12Resource> mConstantBuffer;            // �R���X�^���g�o�b�t�@
	static ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;      // �R���X�^���g�A�V�F�_�[���\�[�X�p�̃q�[�v

	// RTV DSV�n
	static ComPtr<ID3D12Resource> mRenderTarget[FrameCount];  // �����_�\�^�[�Q�b�g�̃��\�[�X
	static ComPtr<ID3D12DescriptorHeap> mRTVHeap;             // �����_�\�^�[�Q�b�g�̃q�[�v
	static ComPtr<ID3D12Resource> mDepthBuffer;               // �[�x�o�b�t�@
	static ComPtr<ID3D12DescriptorHeap> mDSPHeap;             // �[�x�o�b�t�@�̃q�[�v
	// �V���h�E�}�b�v�̂��߂�RTV DSP
	static ComPtr<ID3D12Resource> mShadowRTVTexture;          // �V���h�E�}�b�v�̃e�N�X�`�������_�\�^�[�Q�b�g
	static ComPtr<ID3D12DescriptorHeap> mShadowRTVHeap;       // �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g�̃q�[�v
	static ComPtr<ID3D12Resource> mShadowDepthBuffer;         // �V���h�E�}�b�v�̐[�x�o�b�t�@
	static ComPtr<ID3D12DescriptorHeap> mShadowDSPHeap;       // �V���h�E�}�b�v�̐[�x�o�b�t�@�q�[�v

	static std::vector<class HeapOffset> mCbvHeapOffsets;     // �R���X�^���g�o�b�t�@�r���[�̃q�[�v�I�t�Z�b�g
	static bool mIsFullScreenMode;                            // �X�N���[�����[�h
	static bool mHasInit;                                     // �������ς�?
	static bool mIsFirstRenderingOnFrame;                     // ���̃t���[���ōŏ��̃����_�����O?
	static const unsigned mNumMaxModels = 100;                // ���f���̍ő吔
	static const unsigned mNumMaxMaterials = 10;              // ���f���������̍ő�}�e���A����
	static const unsigned mNumMaxModelTypes = 20;             // ���f���̎�ނ̍ő吔
	static bool mBeganRender;
	static bool mEndedRender;
};


typedef std::unique_ptr<Renderer> RendererPtrU;
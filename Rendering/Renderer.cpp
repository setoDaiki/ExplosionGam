#include "Renderer.h"
#include "App.h"
#include "Common/FileUtil.h"
#include "Mesh.h"
#include "Camera/Camera.h"
#include "HeapOffset.h"
#include <cassert>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace DirectX;

ComPtr<IDXGISwapChain3> Renderer::mSwapChain;
ComPtr<ID3D12Device> Renderer::mDevice;
ComPtr<ID3D12CommandAllocator> Renderer::mCommandAllocator;
ComPtr<ID3D12CommandQueue> Renderer::mCommandQueue;
ComPtr<ID3D12GraphicsCommandList> Renderer::mCommandList;
ComPtr<ID3D12Fence> Renderer::mFence;
UINT* Renderer::mCBVDataBegin;
UINT64 Renderer::mFenceValue;
UINT Renderer::mRTVStride;
UINT Renderer::mCBV_SRV_Stride;
UINT Renderer::mCBSize;
bool Renderer::mHasInit = false;
bool Renderer::mIsFullScreenMode = false;
ComPtr<ID3D12RootSignature> Renderer::mRootSignature;
ComPtr<ID3D12PipelineState> Renderer::mPipelineState;
ComPtr<ID3D12PipelineState> Renderer::mPipelineSprite;
ComPtr<ID3D12PipelineState> Renderer::mPipelineWriteShadow;
ComPtr<ID3D12PipelineState> Renderer::mPipelineReadShadow;
ComPtr<ID3D12PipelineState> Renderer::mPipelineUI;
ComPtr<ID3D12Resource> Renderer::mConstantBuffer;
ComPtr<ID3D12DescriptorHeap> Renderer::mDescriptorHeap;
ComPtr<ID3D12Resource> Renderer::mRenderTarget[FrameCount];
ComPtr<ID3D12DescriptorHeap> Renderer::mRTVHeap;
ComPtr<ID3D12Resource> Renderer::mDepthBuffer;
ComPtr<ID3D12DescriptorHeap> Renderer::mDSPHeap;
ComPtr<ID3D12Resource> Renderer::mShadowRTVTexture;
ComPtr<ID3D12DescriptorHeap> Renderer::mShadowRTVHeap;
ComPtr<ID3D12Resource> Renderer::mShadowDepthBuffer;
ComPtr<ID3D12DescriptorHeap> Renderer::mShadowDSPHeap;
unsigned Renderer::mShadowTextureHeapOffset;
std::vector<HeapOffset> Renderer::mCbvHeapOffsets;
bool Renderer::mBeganRender = false;


HRESULT Renderer::CreateRenderObject(HWND* hWnd) {
	assert(hWnd != nullptr || !"������HWND��null�|�C���^");
	assert(mHasInit == false || !"���̊֐��͓��Ăяo���Ă͂����Ȃ�");

	UINT dxgiFactoryFlags = 0;

	// DirectX�̃f�o�b�O�֗��c�[��
#if defined(_DEBUG)	
	//ID3D12Debug* debugController;
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	//{
	//	debugController->EnableDebugLayer();
	//}
#endif
	// �f�o�C�X�̐���
	if (!SUCCEEDED(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(mDevice.GetAddressOf())))) {
		assert(!"�f�o�C�X�̐����Ɏ��s");
		return S_FALSE;
	}

	// �e���\�[�X�r���[�������̑傫�������߂�
	mRTVStride = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mCBV_SRV_Stride = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �R�}���h�L���[�쐬(�R�}���h���X�g�����s����̂͂�����)
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf()));

	// �R�}���h���X�g�쐬(�`��R�}���h�����߂Ĉ�C�Ɏ��s���č�������}��炵��)
	mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()));
	mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), NULL, IID_PPV_ARGS(mCommandList.GetAddressOf()));
	mCommandList->Close();

	// �X���b�v�`�F�[���쐬(�t���[���̊Ǘ�������)
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = WindowWidth;
	swapChainDesc.Height = WindowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	IDXGIFactory4* factory;
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	factory->CreateSwapChainForHwnd(mCommandQueue.Get(), *hWnd, &swapChainDesc, NULL, NULL, (IDXGISwapChain1**)mSwapChain.GetAddressOf());   // �����œ����Ń����_�\�^�[�Q�b�g�̃o�b�t�@�������
	/*if (mIsFullScreenMode) {
		mSwapChain->SetFullscreenState(true, NULL);
		mSwapChain->ResizeBuffers(FrameCount, WindowWidth, WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	}*/
	// �����_�\�^�[�Q�b�g�̍쐬(�`���)
	// �����_�\�^�[�Q�b�g�̃q�[�v���܂����
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRTVHeap.GetAddressOf()));

	// ���Ƀ����_�\�^�[�Q�b�g�r���[�����
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());             // �q�[�v�̃A�h���X�����蓖�Ăăq�[�v�̒��Ƀr���[�����������悤�ɂ��Ă���
	for (UINT n = 0; n < FrameCount; n++) {
		mSwapChain->GetBuffer(n, IID_PPV_ARGS(mRenderTarget[n].GetAddressOf()));                         // �X���b�v�`�F�[���̒��Ƀo�b�t�@������̂ł��炤
		mDevice->CreateRenderTargetView(mRenderTarget[n].Get(), NULL, rtvHandle);                        // ��������NULL�����K�v�Ȃ�\���̂�����ċ@�\��ς���
		rtvHandle.Offset(1, mRTVStride);   // ��̃����_�\�^�[�Q�b�g�r���[�̑傫������������i�߂�
	}

	//�[�x�o�b�t�@�̍쐬
	D3D12_HEAP_PROPERTIES dsHeapProp = {};
	ZeroMemory(&dsHeapProp, sizeof(&dsHeapProp));
	dsHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	dsHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	dsHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	dsHeapProp.CreationNodeMask = NULL;
	dsHeapProp.VisibleNodeMask = NULL;

	D3D12_RESOURCE_DESC dsResDesc = {};
	ZeroMemory(&dsResDesc, sizeof(D3D12_RESOURCE_DESC));
	dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsResDesc.Alignment = 0;
	dsResDesc.Width = WindowWidth;
	dsResDesc.Height = WindowHeight;
	dsResDesc.DepthOrArraySize = 1;
	dsResDesc.MipLevels = 1;
	dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsResDesc.SampleDesc.Count = 1;
	dsResDesc.SampleDesc.Quality = 0;
	dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue;
	ZeroMemory(&clearValue, sizeof(D3D12_CLEAR_VALUE));
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	mDevice->CreateCommittedResource(
		&dsHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&dsResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(mDepthBuffer.GetAddressOf())
	);

	// �[�x�o�b�t�@�̃r���[�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	ZeroMemory(&dsvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = NULL;
	mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDSPHeap.GetAddressOf()));

	// �[�x�o�b�t�@�r���[
	D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	ZeroMemory(&dsViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsViewDesc.Texture2D.MipSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsViewDesc, dsvHandle);


	// �t�F���X���쐬(GPU��CPU�̓������Ƃ邽�߂̂��)
	mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));
	mFenceValue = 1;

	// �R���X�^���g�o�b�t�@�r���[�ƃV�F�_�[���\�[�X�r���[�p�̃q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC csHeapDesc = {};
	unsigned numConstantBuffers = mNumMaxMaterials * mNumMaxModels;
	unsigned numTextures = mNumMaxMaterials * mNumMaxModelTypes;
	unsigned numDescriptors = numConstantBuffers + numTextures;
	csHeapDesc.NumDescriptors = numDescriptors;
	csHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	csHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	mDevice->CreateDescriptorHeap(&csHeapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));

	// �R���X�^���g�o�b�t�@�̍쐬(��{�I�Ƀ��[���h�}�b�g�Ƃ��̏��)
	// 256�̔{���ɃT�C�Y���A���C�����g����
	mCBSize = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT * ((sizeof(Cbuffer) / D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) + 1);
	D3D12_HEAP_PROPERTIES cHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC cRDesc = CD3DX12_RESOURCE_DESC::Buffer(mCBSize * numConstantBuffers);
	HRESULT hr = mDevice->CreateCommittedResource(
		&cHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cRDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mConstantBuffer.GetAddressOf())
	);
	if (!SUCCEEDED(hr)) {
		assert(!"�R���X�^���g�o�b�t�@�������s");
	}

	for (DWORD i = 0; i < numConstantBuffers; i++) {
		// �r���[�����
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		// �o�b�t�@�̈ʒu��CB�T�C�Y�ł��炷
		cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + (i * mCBSize);
		cbvDesc.SizeInBytes = mCBSize;
		D3D12_CPU_DESCRIPTOR_HANDLE cHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cHandle.ptr += i * mCBV_SRV_Stride; // �n���h���̈ʒu�̓r���[�̃X�g���C�h�ł��炷
		mDevice->CreateConstantBufferView(&cbvDesc, cHandle);
	}

	// �R���X�^���g�o�b�t�@�̃}�b�v
	CD3DX12_RANGE readRange(0, 0);
	readRange = CD3DX12_RANGE(0, 0);
	mCBVDataBegin;  // �������܂����ꕨ
	mConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mCBVDataBegin));

	// �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g
	D3D12_RESOURCE_DESC shadowResDesc = {};
	ZeroMemory(&shadowResDesc, sizeof(shadowResDesc));
	shadowResDesc.MipLevels = 1;
	shadowResDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shadowResDesc.Width = WindowWidth;
	shadowResDesc.Height = WindowHeight;
	shadowResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	shadowResDesc.DepthOrArraySize = 1;
	shadowResDesc.SampleDesc.Count = 1;
	shadowResDesc.SampleDesc.Quality = 0;
	shadowResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE clearValueShadowRTV = {};
	ZeroMemory(&clearValueShadowRTV, sizeof(D3D12_CLEAR_VALUE));
	clearValueShadowRTV.Format = DXGI_FORMAT_R32_FLOAT;
	clearValueShadowRTV.Color[0] = 0.0f;
	clearValueShadowRTV.Color[1] = 0.0f;
	clearValueShadowRTV.Color[2] = 0.5f;
	clearValueShadowRTV.Color[3] = 1.0f;
	clearValueShadowRTV.DepthStencil.Depth = 1.0f;
	clearValueShadowRTV.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES shadowRTVHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	mDevice->CreateCommittedResource(
		&shadowRTVHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&shadowResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&clearValueShadowRTV,
		IID_PPV_ARGS(mShadowRTVTexture.GetAddressOf())
	);

	// �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC shadowResHeapDesc = {};
	shadowResHeapDesc.NumDescriptors = FrameCount;
	shadowResHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	shadowResHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDevice->CreateDescriptorHeap(&shadowResHeapDesc, IID_PPV_ARGS(mShadowRTVHeap.GetAddressOf()));
	// �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g�r���[
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart());
	mDevice->CreateRenderTargetView(mShadowRTVTexture.Get(), NULL, shadowRTVHandle);

	// �V���h�E�}�b�v�p�̃V�F�_�[���\�[�X�r���[�����
	D3D12_SHADER_RESOURCE_VIEW_DESC shadowSRDesc = {};
	shadowSRDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shadowSRDesc.Format = shadowResDesc.Format;
	shadowSRDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	shadowSRDesc.Texture2D.MipLevels = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	mShadowTextureHeapOffset = numDescriptors - 1;
	handle.ptr += mShadowTextureHeapOffset * mCBV_SRV_Stride;
	mDevice->CreateShaderResourceView(mShadowRTVTexture.Get(), &shadowSRDesc, handle);

	// �V���h�E�}�b�v�p�̐[�x�o�b�t�@
	D3D12_HEAP_PROPERTIES shadowDepthHeapProp = {};
	ZeroMemory(&shadowDepthHeapProp, sizeof(&shadowDepthHeapProp));
	shadowDepthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	shadowDepthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	shadowDepthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	shadowDepthHeapProp.CreationNodeMask = NULL;
	shadowDepthHeapProp.VisibleNodeMask = NULL;

	D3D12_RESOURCE_DESC shadowDepthResDesc = {};
	ZeroMemory(&shadowDepthResDesc, sizeof(D3D12_RESOURCE_DESC));
	shadowDepthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	shadowDepthResDesc.Alignment = 0;
	shadowDepthResDesc.Width = WindowWidth;
	shadowDepthResDesc.Height = WindowHeight;
	shadowDepthResDesc.DepthOrArraySize = 1;
	shadowDepthResDesc.MipLevels = 1;
	shadowDepthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDepthResDesc.SampleDesc.Count = 1;
	shadowDepthResDesc.SampleDesc.Quality = 0;
	shadowDepthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	shadowDepthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValueShadowDSP = {};
	ZeroMemory(&clearValueShadowDSP, sizeof(D3D12_CLEAR_VALUE));
	clearValueShadowDSP.Format = DXGI_FORMAT_D32_FLOAT;
	clearValueShadowDSP.DepthStencil.Depth = 1.0f;
	clearValueShadowDSP.DepthStencil.Stencil = 0;
	mDevice->CreateCommittedResource(
		&shadowDepthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&shadowDepthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValueShadowDSP,
		IID_PPV_ARGS(mShadowDepthBuffer.GetAddressOf())
	);

	// �V���h�E�}�b�v�[�x�o�b�t�@�r���[�̃q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC shadowDSVHeapDesc = {};
	ZeroMemory(&shadowDSVHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	shadowDSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	shadowDSVHeapDesc.NumDescriptors = 1;
	shadowDSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	shadowDSVHeapDesc.NodeMask = NULL;
	mDevice->CreateDescriptorHeap(&shadowDSVHeapDesc, IID_PPV_ARGS(mShadowDSPHeap.GetAddressOf()));

	// �V���h�E�}�b�v�[�x�o�b�t�@�r���[
	D3D12_DEPTH_STENCIL_VIEW_DESC shadowDSViewDesc = {};
	ZeroMemory(&shadowDSViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	shadowDSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	shadowDSViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	shadowDSViewDesc.Texture2D.MipSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateDepthStencilView(mShadowDepthBuffer.Get(), &shadowDSViewDesc, shadowDSVHandle);

	// �V�F�_�[�쐬
	ID3DBlob* spriteVertexShader;
	ID3DBlob* spritePixelShader;
	ID3DBlob* uiVertexShader;
	ID3DBlob* uiPixelShader;
	ID3DBlob* onlyPhongVertexShader;
	ID3DBlob* onlyPhongPixelShader;
	ID3DBlob* writeShadowVertexShader;
	ID3DBlob* writeShadowPixelShader;
	ID3DBlob* readShadowVertexShader;
	ID3DBlob* readShadowPixelShader;


	D3DCompileFromFile(L"Shader/ForSprite.hlsl", nullptr, nullptr, "VS", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &spriteVertexShader, nullptr);
	D3DCompileFromFile(L"Shader/ForSprite.hlsl", nullptr, nullptr, "PS", "ps_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &spritePixelShader, nullptr);
	D3DCompileFromFile(L"Shader/ForUI.hlsl",     nullptr, nullptr, "VS", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &uiVertexShader, nullptr);
	D3DCompileFromFile(L"Shader/ForUI.hlsl",     nullptr, nullptr, "PS", "ps_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &uiPixelShader, nullptr);
	D3DCompileFromFile(L"Shader/Phong.hlsl",	 nullptr, nullptr, "VS", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &onlyPhongVertexShader, nullptr);
	D3DCompileFromFile(L"Shader/Phong.hlsl",     nullptr, nullptr, "PS", "ps_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &onlyPhongPixelShader, nullptr);
	D3DCompileFromFile(L"Shader/ShadowMap.hlsl", nullptr, nullptr, "VS_Write", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &writeShadowVertexShader, nullptr);
	D3DCompileFromFile(L"Shader/ShadowMap.hlsl", nullptr, nullptr, "PS_Write", "ps_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &writeShadowPixelShader, nullptr);
	D3DCompileFromFile(L"Shader/ShadowMap.hlsl", nullptr, nullptr, "VS", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &readShadowVertexShader, nullptr);
	D3DCompileFromFile(L"Shader/ShadowMap.hlsl", nullptr, nullptr, "PS", "ps_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &readShadowPixelShader, nullptr);


	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputElement[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	// ���[�g�V�O�l�`���̍쐬(�V�F�[�_�Ƀ��\�[�X�̈ʒu��������)
	// �T���v���[���쐬
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	// �Ȃ񂩃����W�̎w��̎��̂�����邩��V�O�l�`���ɒ��ړ������̂���?
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// �����W������Ă���Ń��[�g�p�����[�^������
	// �֘A�t����K�v�̂��郊�\�[�X�̃^�C�v����Ȃ̂Ń��[�g�p�����[�^�ƃ����W�͓��
	// �R���X�^���g�o�b�t�@�r���[�ƃV�F�_�[���\�[�X�r���[
	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	// �֘A�t���郊�\�[�X���R���X�^���g�o�b�t�@�r���[�Ȃ̂ő�������TYPE_CBV
	// ��3�����͈�ԏ��������W�X�^�[�ԍ�
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	// �֘A�t�����ڂ̓V�F�_�[���\�[�X�r���[
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	// �O�߂��V�F�_�[���\�[�X�r���[
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_ALL);


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	ID3DBlob* error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf()));
	//�u�����h�p�̐ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC bdesc = {};
	bdesc.BlendEnable = true;
	bdesc.LogicOpEnable = false;
	bdesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	bdesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	bdesc.BlendOp = D3D12_BLEND_OP_ADD;
	bdesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	bdesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	bdesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	bdesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	bdesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// �p�C�v���C���X�e�[�g�쐬
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};
	psDesc.InputLayout = { inputElement,_countof(inputElement) };
	psDesc.pRootSignature = mRootSignature.Get();
	psDesc.VS = CD3DX12_SHADER_BYTECODE(onlyPhongVertexShader);
	psDesc.PS = CD3DX12_SHADER_BYTECODE(onlyPhongPixelShader);
	psDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psDesc.RasterizerState.FrontCounterClockwise = false;
	psDesc.BlendState.RenderTarget[0] = bdesc;
	psDesc.DepthStencilState.DepthEnable = true;
	psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	psDesc.DepthStencilState.FrontFace = defaultStencilOp;
	psDesc.DepthStencilState.BackFace = defaultStencilOp;
	psDesc.DepthStencilState.StencilEnable = false;
	psDesc.SampleMask = UINT_MAX;
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psDesc.SampleDesc.Count = 1;
	mDevice->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(mPipelineState.GetAddressOf()));
	
	// �X�v���C�g�p
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescSprite = psDesc;
	psDescSprite.VS = CD3DX12_SHADER_BYTECODE(spriteVertexShader);
	psDescSprite.PS = CD3DX12_SHADER_BYTECODE(spritePixelShader);
	mDevice->CreateGraphicsPipelineState(&psDescSprite, IID_PPV_ARGS(mPipelineSprite.GetAddressOf()));

	// UI�p
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescUI = psDesc;
	psDescUI.VS = CD3DX12_SHADER_BYTECODE(uiVertexShader);
	psDescUI.PS = CD3DX12_SHADER_BYTECODE(uiPixelShader);
	mDevice->CreateGraphicsPipelineState(&psDescUI, IID_PPV_ARGS(mPipelineUI.GetAddressOf()));

	// �V���h�E�}�b�v�p
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescWriteShadow = psDesc;
	psDescWriteShadow.DepthStencilState.DepthEnable = true;
	psDescWriteShadow.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDescWriteShadow.VS = CD3DX12_SHADER_BYTECODE(writeShadowVertexShader);
	psDescWriteShadow.PS = CD3DX12_SHADER_BYTECODE(writeShadowPixelShader);
	psDescWriteShadow.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
	mDevice->CreateGraphicsPipelineState(&psDescWriteShadow, IID_PPV_ARGS(mPipelineWriteShadow.GetAddressOf()));
	// �V���h�E�}�b�v�ǂݍ��ݗp
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescReadShadow = psDescWriteShadow;
	psDescReadShadow.DepthStencilState.DepthEnable = true;
	psDescReadShadow.VS = CD3DX12_SHADER_BYTECODE(readShadowVertexShader);
	psDescReadShadow.PS = CD3DX12_SHADER_BYTECODE(readShadowPixelShader);
	psDescReadShadow.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	mDevice->CreateGraphicsPipelineState(&psDescReadShadow, IID_PPV_ARGS(mPipelineReadShadow.GetAddressOf()));

	mHasInit = true;
	return S_OK;
}


void Renderer::BeginRender() {
	assert(!mBeganRender);
	mBeganRender = true;

	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	//�R�}���h���X�g�ƃA���P�[�^�����Z�b�g���Ƃ�
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), mPipelineWriteShadow.Get());

	// �R�}���h���X�g�ɕ`�施�߂����߂Ă���


	//���\�[�X�o���A�̏�Ԃ������_�\�^�[�Q�b�g���[�h�ɕύX
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mRenderTarget[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier(1, &barrier);

	//�r���[�|�[�g���Z�b�g(�`��̈�)
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight));
	CD3DX12_RECT scissorRect = CD3DX12_RECT(0, 0, WindowWidth, WindowHeight);
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);


	// �V���h�E�p���[�_�[�^�[�Q�b�g�n���h���̎擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	// �V���h�E�p�����_�\�^�[�Q�b�g�̃Z�b�g
	// mCommandList->OMSetRenderTargets(1, &shadowRTVHandle, false, &shadowDSVHandle); �����ŃZ�b�g����K�v�͂Ȃ�

	// �V���h�E�}�b�v�p�����_�\�^�[�Q�b�g�̃N���A
	const float clearColor[] = { 0.0f,0.0f,0.5f,1.0f };
	mCommandList->ClearRenderTargetView(shadowRTVHandle, clearColor, 0, NULL);
	mCommandList->ClearDepthStencilView(shadowDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	mCommandList->SetPipelineState(mPipelineState.Get());


	// �J�����g�o�b�t�@�������_�\�^�[�Q�b�g�ɂ��� 
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();

	// ��ʃN���A
	mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
	mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// ���[�g�V�O�l�`�����Z�b�g
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// �q�[�v���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { mDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);


	// �|���S���̌`����w��
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // �O�p�`���X�g

}

void Renderer::EndRender() {

	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = {};
	// ���\�[�X�o���A��Present���[�h�ɕύX
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mRenderTarget[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier(1, &barrier);

	// ���ߏI�����̂ŕ���
	mCommandList->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̐؂�ւ�
	mSwapChain->Present(1, 0);

	// GPU�Ɠ���
	WaitGpu();
	for (auto& heapOffset : mCbvHeapOffsets) {
		heapOffset.Release();
	}
	mCbvHeapOffsets.clear();

	mBeganRender = false;
}

Renderer::Renderer() {
	assert(mHasInit == true);
}


void Renderer::ShadowMap(const DirectX::XMMATRIX& worldMat, const Camera& camera,const MyMesh& mesh) {

	assert(mBeganRender);
	//World View Projection �ϊ�
	XMMATRIX viewMat = camera.GetViewMat();
	XMMATRIX projMat = camera.GetProjMat();
	XMFLOAT3 eyeVec = camera.GetPos();


	mCommandList->SetPipelineState(mPipelineWriteShadow.Get());
	// �V���h�E�p���[�_�[�^�[�Q�b�g�n���h���̎擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	// �V���h�E�p�����_�\�^�[�Q�b�g�̃Z�b�g
	mCommandList->OMSetRenderTargets(1, &shadowRTVHandle, false, &shadowDSVHandle);
	Render(worldMat, viewMat, projMat, eyeVec, mesh);
}



void Renderer::RenderMesh(const XMMATRIX& worldMat, const Camera& camera,const MyMesh& mesh) {

	assert(mBeganRender);
	//World View Projection �ϊ�
	XMMATRIX viewMat = camera.GetViewMat();
	XMMATRIX projMat = camera.GetProjMat();
	XMFLOAT3 eyeVec = camera.GetPos();

	
	mCommandList->SetPipelineState(mPipelineState.Get());

	D3D12_GPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += mShadowTextureHeapOffset * mCBV_SRV_Stride;
	mCommandList->SetGraphicsRootDescriptorTable(2, handle);

	// �J�����g�o�b�t�@�������_�\�^�[�Q�b�g�ɂ��� 
	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Render(worldMat, viewMat, projMat, eyeVec, mesh);

}


void Renderer::RenderSprite(const XMMATRIX& worldMat, const Camera& camera,const MyMesh& mesh) {
	assert(mBeganRender);
	XMMATRIX viewMat = camera.GetViewMat();  
	XMFLOAT3 eyeVec = camera.GetPos();
	XMMATRIX projMat = camera.GetProjMat();

	mCommandList->SetPipelineState(mPipelineSprite.Get());
	// �J�����g�o�b�t�@�������_�\�^�[�Q�b�g�ɂ��� 
	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Render(worldMat, viewMat, projMat, eyeVec, mesh);

}

void Renderer::RenderUI(const XMMATRIX& worldMat, const Camera& camera, const MyMesh& mesh) {
	assert(mBeganRender);
	XMMATRIX viewMat;      // UI�ł̓r���[�ϊ�������Ȃ��̂ŏ������Ȃ�
	XMFLOAT3 eyeVec;       // UI�Ɏ��_�ʒu�͊֌W�Ȃ��̂ŏ������Ȃ�
	XMMATRIX projMat = camera.GetProjMat();

	mCommandList->SetPipelineState(mPipelineUI.Get());
	// �J�����g�o�b�t�@�������_�\�^�[�Q�b�g�ɂ��� 
	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Render(worldMat, viewMat, projMat, eyeVec, mesh);
}



//------------------------------------
//  �e�N�X�`���o�b�t�@����
//------------------------------------
void Renderer::CreateTexture(ID3D12Resource* resource,std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset) {
	// �e�N�X�`���֘A����
	// �e�N�X�`�����������Ȃ��΂�
	if (textureName.size() <= 2) {
		assert(!"�e�N�X�`�����������B���̃��b�V���ɂ̓e�N�X�`�����܂܂�Ȃ��Ȃǂ̌������l�����܂�");
	}

	// �t�@�C������e�N�Z����T�C�Y����ǂݏo��
	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subRes;
	//�e�N�Z����T�C�Y�����[�h
#ifdef _DEBUG
	char str[59];
	sprintf(str, ToUTF8(textureName).c_str(), "", MB_OK);
	//std::cout << str << "\n";
#endif
	if (!SUCCEEDED(LoadWICTextureFromFile(mDevice.Get(), textureName.c_str(), &resource, decodedData, subRes))) {
		assert(!str);
	}

	// �󔒂̃e�N�X�`��
	D3D12_RESOURCE_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(tDesc));
	tDesc.MipLevels = 1;
	tDesc.Format = fileFormat;
	tDesc.Width = subRes.RowPitch / 4;
	tDesc.Height = subRes.SlicePitch / subRes.RowPitch;
	tDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	tDesc.DepthOrArraySize = 1;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//�q�[�v�͓ǂݏ����ł��Ȃ�������DEFAULT
	D3D12_HEAP_PROPERTIES tHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	mDevice->CreateCommittedResource(
		&tHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&tDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	// CPU����GPU�Ƀe�N�X�`���f�[�^��n���Ƃ��̒��p�p�e�N�X�`�������
	ID3D12Resource* textureUploadHeap;
	// �q�[�v��CPU����ǂݏ����ł���UPLOAD
	D3D12_HEAP_PROPERTIES stHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// �f�[�^���A�b�v���[�h���邽�߂ɕK�v�ȑ傫�������߂�
	UINT64 bufferSize = GetRequiredIntermediateSize(resource, 0, 1);
	D3D12_RESOURCE_DESC stResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	mDevice->CreateCommittedResource(
		&stHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&stResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureUploadHeap)
	);

	// �R�}���h���X�g���g�����烊�Z�b�g���Ƃ�
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), 0);

	// �ǂ݂������e�N�Z���f�[�^���󔒂̃e�N�X�`���[�ɗ������݁A����������
	// �R�}���h���X�g�Ƀ��\�[�X���M���߂�����
	UpdateSubresources(mCommandList.Get(), resource, textureUploadHeap, 0, 0, 1, &subRes);
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	mCommandList->ResourceBarrier(1, &barrier);

	// �e�N�X�`���̑��M�����邽�߂ɃR�}���X�����
	mCommandList->Close();

	// �R�}���h���X�g���s
	ID3D12CommandList* commandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	WaitGpu();

	// �V�F�_�[���\�[�X�r���[����
	D3D12_SHADER_RESOURCE_VIEW_DESC sDesc = {};
	sDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	sDesc.Format = fileFormat;
	sDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	sDesc.Texture2D.MipLevels = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += heapOffset * mCBV_SRV_Stride;
	mDevice->CreateShaderResourceView(resource, &sDesc, handle);
	
}

void Renderer::Render(const XMMATRIX& worldMat, const XMMATRIX& viewMat, const XMMATRIX& projMat, const XMFLOAT3& eyeVec,const MyMesh& mesh ) {
	
	const MyMaterial& material = mesh.GetMaterial();
	// ���_�o�b�t�@���Z�b�g
	mCommandList->IASetVertexBuffers(0, 1, mesh.GetVertexBufferView());

	// �R���X�^���g�o�b�t�@�̓��e�̍X�V
	Cbuffer cb;   // �������ޓ��e
	char* ptr = reinterpret_cast<char*>(mCBVDataBegin); // �������ݐ�A�h���X�A
	HeapOffset cbvHeapOffset;
	// �o�b�t�@�̂ǂ��ɏ������ނ��𒲐�����
	ptr += mCBSize * cbvHeapOffset.GetHeapOffset();
	// �R���X�^���g�o�b�t�@�ݒ�
	// ���C�g�r���[
	XMMATRIX lightViewMat = XMMatrixIdentity();
	XMMATRIX lightTranslation = XMMatrixTranslation(0.0f, 60.0f, 0.0f);
	XMMATRIX lightRotation = XMMatrixRotationX(XMConvertToRadians(90.0f));

	lightViewMat = lightRotation * lightTranslation;
	lightViewMat = XMMatrixInverse(nullptr, lightViewMat);

	//�V�F�[�_���͗�D��Ȃ̂œ]�u�������̂�n��
	cb.wvp = XMMatrixTranspose(worldMat * viewMat * projMat);
	cb.w = XMMatrixTranspose(worldMat);
	cb.wlp = XMMatrixTranspose(worldMat * lightViewMat * projMat);
	cb.wp = XMMatrixTranspose(worldMat * projMat);
	cb.lightPos = XMFLOAT4(0.0f, 60.0f, 0.0f, 1.0f);
	cb.eye = XMFLOAT4(eyeVec.x, eyeVec.y, eyeVec.z, 1.0f);
	cb.ambient = XMFLOAT4(material.ambient.x, material.ambient.y,
		material.ambient.z, material.ambient.w);
	cb.diffuse = XMFLOAT4(material.diffuse.x, material.diffuse.y,
		material.diffuse.z, material.diffuse.w);
	cb.specular = XMFLOAT4(material.specular.x, material.specular.y,
		material.specular.z, material.specular.w);
	
	// UV�ϊ��s��
	XMFLOAT4 f1(0.5f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vec1 = XMLoadFloat4(&f1);
	XMFLOAT4 f2(0.0f, -0.5f, 0.0f, 0.0f);
	XMVECTOR vec2 = XMLoadFloat4(&f2);
	XMFLOAT4 f3(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR vec3 = XMLoadFloat4(&f3);
	XMFLOAT4 f4(0.5f, 0.5f, 0.0f, 1.0f);
	XMVECTOR vec4 = XMLoadFloat4(&f4);
	XMMATRIX clipMat(vec1, vec2, vec3, vec4);
	cb.wlpt = XMMatrixTranspose(worldMat * lightViewMat * projMat * clipMat);

	std::memcpy(ptr, &cb, sizeof(Cbuffer));

	// CBV�Z�b�g
	// CBV�쐬���ɏ������悤�Ƀq�[�v�̍ŏ���CBV�������Ă�
	D3D12_GPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();


	// �f�B�X�N���v�^�q�[�v�̂ǂ�������̂����q�[�v�I�t�Z�b�g�Œ��߂���
	handle.ptr += cbvHeapOffset.GetHeapOffset() * mCBV_SRV_Stride; // �f�B�X�N���v�^�q�[�v�̈ʒu�Ȃ̂ŃX�g���C�h
	mCommandList->SetGraphicsRootDescriptorTable(0, handle);

	// ����������������ƃq�[�v�I�t�Z�b�g���������Ă��܂��̂ŕ`�悪�I���܂ŕۑ����Ă���
	mCbvHeapOffsets.push_back(cbvHeapOffset);


	// SRV�Z�b�g
	// CBV�̎���SRV�������Ă�
	//if (material.textureName.size() > 2) {
	handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += mesh.GetSrvHeapOffset() * mCBV_SRV_Stride;
	mCommandList->SetGraphicsRootDescriptorTable(1, handle);
	//}

	// �C���f�b�N�X�o�b�t�@���Z�b�g
	mCommandList->IASetIndexBuffer(mesh.GetIndexBufferView());

	// �`��
	// �������͒��_���A�t�F�C�X�̓|���S�����Ȃ̂�3�{
	mCommandList->DrawIndexedInstanced(mesh.GetNumTriangles() * 3, 1, 0, 0, 0);
}



//------------------------------------------
//  CPU��GPU�̓���������
//------------------------------------------
void Renderer::WaitGpu() {
	// GPU�̎d�����I������Ƃ���GPU����A���Ă���l���Z�b�g����
	mCommandQueue->Signal(mFence.Get(), mFenceValue);

	do {
		// �ҋ@
		// �Ȃ񂩂ł���悤�ɂ��Ȃ���?
	} while (mFence->GetCompletedValue() < mFenceValue); // �������I����Ă�Ɗ֐��̖߂�l�������ă��[�v�E�o����
	mFenceValue++;
}





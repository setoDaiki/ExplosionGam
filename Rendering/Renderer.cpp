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
	assert(hWnd != nullptr || !"引数のHWNDがnullポインタ");
	assert(mHasInit == false || !"この関数は二回呼び出してはいけない");

	UINT dxgiFactoryFlags = 0;

	// DirectXのデバッグ便利ツール
#if defined(_DEBUG)	
	//ID3D12Debug* debugController;
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	//{
	//	debugController->EnableDebugLayer();
	//}
#endif
	// デバイスの生成
	if (!SUCCEEDED(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(mDevice.GetAddressOf())))) {
		assert(!"デバイスの生成に失敗");
		return S_FALSE;
	}

	// 各リソースビュー一つ当たりの大きさを求める
	mRTVStride = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mCBV_SRV_Stride = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// コマンドキュー作成(コマンドリストを実行するのはこいつ)
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf()));

	// コマンドリスト作成(描画コマンドをためて一気に実行して高速化を図るらしい)
	mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()));
	mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), NULL, IID_PPV_ARGS(mCommandList.GetAddressOf()));
	mCommandList->Close();

	// スワップチェーン作成(フレームの管理をする)
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
	factory->CreateSwapChainForHwnd(mCommandQueue.Get(), *hWnd, &swapChainDesc, NULL, NULL, (IDXGISwapChain1**)mSwapChain.GetAddressOf());   // ここで内部でレンダ―ターゲットのバッファが作られる
	/*if (mIsFullScreenMode) {
		mSwapChain->SetFullscreenState(true, NULL);
		mSwapChain->ResizeBuffers(FrameCount, WindowWidth, WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	}*/
	// レンダ―ターゲットの作成(描画先)
	// レンダ―ターゲットのヒープをまず作る
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRTVHeap.GetAddressOf()));

	// 次にレンダ―ターゲットビューを作る
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());             // ヒープのアドレスを割り当ててヒープの中にビューが生成されるようにしている
	for (UINT n = 0; n < FrameCount; n++) {
		mSwapChain->GetBuffer(n, IID_PPV_ARGS(mRenderTarget[n].GetAddressOf()));                         // スワップチェーンの中にバッファがあるのでもらう
		mDevice->CreateRenderTargetView(mRenderTarget[n].Get(), NULL, rtvHandle);                        // 第二引数がNULLだが必要なら構造体を作って機能を変える
		rtvHandle.Offset(1, mRTVStride);   // 一つのレンダ―ターゲットビューの大きさ分メモリを進める
	}

	//深度バッファの作成
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

	// 深度バッファのビューヒープ
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	ZeroMemory(&dsvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = NULL;
	mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDSPHeap.GetAddressOf()));

	// 深度バッファビュー
	D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	ZeroMemory(&dsViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsViewDesc.Texture2D.MipSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsViewDesc, dsvHandle);


	// フェンスを作成(GPUとCPUの同期をとるためのやつ)
	mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));
	mFenceValue = 1;

	// コンスタントバッファビューとシェダーリソースビュー用のヒープ
	D3D12_DESCRIPTOR_HEAP_DESC csHeapDesc = {};
	unsigned numConstantBuffers = mNumMaxMaterials * mNumMaxModels;
	unsigned numTextures = mNumMaxMaterials * mNumMaxModelTypes;
	unsigned numDescriptors = numConstantBuffers + numTextures;
	csHeapDesc.NumDescriptors = numDescriptors;
	csHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	csHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	mDevice->CreateDescriptorHeap(&csHeapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));

	// コンスタントバッファの作成(基本的にワールドマットとかの情報)
	// 256の倍数にサイズをアライメントする
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
		assert(!"コンスタントバッファ生成失敗");
	}

	for (DWORD i = 0; i < numConstantBuffers; i++) {
		// ビューを作る
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		// バッファの位置はCBサイズでずらす
		cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + (i * mCBSize);
		cbvDesc.SizeInBytes = mCBSize;
		D3D12_CPU_DESCRIPTOR_HANDLE cHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cHandle.ptr += i * mCBV_SRV_Stride; // ハンドルの位置はビューのストライドでずらす
		mDevice->CreateConstantBufferView(&cbvDesc, cHandle);
	}

	// コンスタントバッファのマップ
	CD3DX12_RANGE readRange(0, 0);
	readRange = CD3DX12_RANGE(0, 0);
	mCBVDataBegin;  // 書き込まれる入れ物
	mConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mCBVDataBegin));

	// シャドウマップのレンダ―ターゲット
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

	// シャドウマップのレンダ―ターゲットヒープ
	D3D12_DESCRIPTOR_HEAP_DESC shadowResHeapDesc = {};
	shadowResHeapDesc.NumDescriptors = FrameCount;
	shadowResHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	shadowResHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDevice->CreateDescriptorHeap(&shadowResHeapDesc, IID_PPV_ARGS(mShadowRTVHeap.GetAddressOf()));
	// シャドウマップのレンダ―ターゲットビュー
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart());
	mDevice->CreateRenderTargetView(mShadowRTVTexture.Get(), NULL, shadowRTVHandle);

	// シャドウマップ用のシェダーリソースビューを作る
	D3D12_SHADER_RESOURCE_VIEW_DESC shadowSRDesc = {};
	shadowSRDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shadowSRDesc.Format = shadowResDesc.Format;
	shadowSRDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	shadowSRDesc.Texture2D.MipLevels = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	mShadowTextureHeapOffset = numDescriptors - 1;
	handle.ptr += mShadowTextureHeapOffset * mCBV_SRV_Stride;
	mDevice->CreateShaderResourceView(mShadowRTVTexture.Get(), &shadowSRDesc, handle);

	// シャドウマップ用の深度バッファ
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

	// シャドウマップ深度バッファビューのヒープ
	D3D12_DESCRIPTOR_HEAP_DESC shadowDSVHeapDesc = {};
	ZeroMemory(&shadowDSVHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	shadowDSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	shadowDSVHeapDesc.NumDescriptors = 1;
	shadowDSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	shadowDSVHeapDesc.NodeMask = NULL;
	mDevice->CreateDescriptorHeap(&shadowDSVHeapDesc, IID_PPV_ARGS(mShadowDSPHeap.GetAddressOf()));

	// シャドウマップ深度バッファビュー
	D3D12_DEPTH_STENCIL_VIEW_DESC shadowDSViewDesc = {};
	ZeroMemory(&shadowDSViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	shadowDSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	shadowDSViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	shadowDSViewDesc.Texture2D.MipSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateDepthStencilView(mShadowDepthBuffer.Get(), &shadowDSViewDesc, shadowDSVHandle);

	// シェダー作成
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


	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElement[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	// ルートシグネチャの作成(シェーダにリソースの位置を教える)
	// サンプラーを作成
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
	// なんかレンジの指定の時のやつがあるからシグネチャに直接入れられるのかな?
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// レンジを作ってそれでルートパラメータをつくる
	// 関連付ける必要のあるリソースのタイプが二つなのでルートパラメータとレンジは二つ
	// コンスタントバッファビューとシェダーリソースビュー
	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	// 関連付けるリソースがコンスタントバッファビューなので第一引数はTYPE_CBV
	// 第3引数は一番小さいレジスター番号
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	// 関連付ける二つ目はシェダーリソースビュー
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	// 三つめもシェダーリソースビュー
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
	//ブレンド用の設定
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

	// パイプラインステート作成
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
	
	// スプライト用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescSprite = psDesc;
	psDescSprite.VS = CD3DX12_SHADER_BYTECODE(spriteVertexShader);
	psDescSprite.PS = CD3DX12_SHADER_BYTECODE(spritePixelShader);
	mDevice->CreateGraphicsPipelineState(&psDescSprite, IID_PPV_ARGS(mPipelineSprite.GetAddressOf()));

	// UI用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescUI = psDesc;
	psDescUI.VS = CD3DX12_SHADER_BYTECODE(uiVertexShader);
	psDescUI.PS = CD3DX12_SHADER_BYTECODE(uiPixelShader);
	mDevice->CreateGraphicsPipelineState(&psDescUI, IID_PPV_ARGS(mPipelineUI.GetAddressOf()));

	// シャドウマップ用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescWriteShadow = psDesc;
	psDescWriteShadow.DepthStencilState.DepthEnable = true;
	psDescWriteShadow.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDescWriteShadow.VS = CD3DX12_SHADER_BYTECODE(writeShadowVertexShader);
	psDescWriteShadow.PS = CD3DX12_SHADER_BYTECODE(writeShadowPixelShader);
	psDescWriteShadow.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
	mDevice->CreateGraphicsPipelineState(&psDescWriteShadow, IID_PPV_ARGS(mPipelineWriteShadow.GetAddressOf()));
	// シャドウマップ読み込み用
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

	// 現在のバッファ(描画先)を取得
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	//コマンドリストとアロケータをリセットしとく
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), mPipelineWriteShadow.Get());

	// コマンドリストに描画命令をためていく


	//リソースバリアの状態をレンダ―ターゲットモードに変更
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mRenderTarget[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier(1, &barrier);

	//ビューポートをセット(描画領域)
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight));
	CD3DX12_RECT scissorRect = CD3DX12_RECT(0, 0, WindowWidth, WindowHeight);
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);


	// シャドウ用レーダーターゲットハンドルの取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	// シャドウ用レンダ―ターゲットのセット
	// mCommandList->OMSetRenderTargets(1, &shadowRTVHandle, false, &shadowDSVHandle); ここでセットする必要はない

	// シャドウマップ用レンダ―ターゲットのクリア
	const float clearColor[] = { 0.0f,0.0f,0.5f,1.0f };
	mCommandList->ClearRenderTargetView(shadowRTVHandle, clearColor, 0, NULL);
	mCommandList->ClearDepthStencilView(shadowDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	mCommandList->SetPipelineState(mPipelineState.Get());


	// カレントバッファをレンダ―ターゲットにする 
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();

	// 画面クリア
	mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
	mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// ルートシグネチャをセット
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// ヒープをセット
	ID3D12DescriptorHeap* heaps[] = { mDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);


	// ポリゴンの形状を指定
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // 三角形リスト

}

void Renderer::EndRender() {

	// 現在のバッファ(描画先)を取得
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = {};
	// リソースバリアをPresentモードに変更
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mRenderTarget[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier(1, &barrier);

	// 溜め終ったので閉じる
	mCommandList->Close();

	// コマンドリストの実行
	ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// バックバッファとフロントバッファの切り替え
	mSwapChain->Present(1, 0);

	// GPUと同期
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
	//World View Projection 変換
	XMMATRIX viewMat = camera.GetViewMat();
	XMMATRIX projMat = camera.GetProjMat();
	XMFLOAT3 eyeVec = camera.GetPos();


	mCommandList->SetPipelineState(mPipelineWriteShadow.Get());
	// シャドウ用レーダーターゲットハンドルの取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	// シャドウ用レンダ―ターゲットのセット
	mCommandList->OMSetRenderTargets(1, &shadowRTVHandle, false, &shadowDSVHandle);
	Render(worldMat, viewMat, projMat, eyeVec, mesh);
}



void Renderer::RenderMesh(const XMMATRIX& worldMat, const Camera& camera,const MyMesh& mesh) {

	assert(mBeganRender);
	//World View Projection 変換
	XMMATRIX viewMat = camera.GetViewMat();
	XMMATRIX projMat = camera.GetProjMat();
	XMFLOAT3 eyeVec = camera.GetPos();

	
	mCommandList->SetPipelineState(mPipelineState.Get());

	D3D12_GPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += mShadowTextureHeapOffset * mCBV_SRV_Stride;
	mCommandList->SetGraphicsRootDescriptorTable(2, handle);

	// カレントバッファをレンダ―ターゲットにする 
	// 現在のバッファ(描画先)を取得
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
	// カレントバッファをレンダ―ターゲットにする 
	// 現在のバッファ(描画先)を取得
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Render(worldMat, viewMat, projMat, eyeVec, mesh);

}

void Renderer::RenderUI(const XMMATRIX& worldMat, const Camera& camera, const MyMesh& mesh) {
	assert(mBeganRender);
	XMMATRIX viewMat;      // UIではビュー変換がいらないので初期化なし
	XMFLOAT3 eyeVec;       // UIに視点位置は関係ないので初期化なし
	XMMATRIX projMat = camera.GetProjMat();

	mCommandList->SetPipelineState(mPipelineUI.Get());
	// カレントバッファをレンダ―ターゲットにする 
	// 現在のバッファ(描画先)を取得
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Render(worldMat, viewMat, projMat, eyeVec, mesh);
}



//------------------------------------
//  テクスチャバッファ生成
//------------------------------------
void Renderer::CreateTexture(ID3D12Resource* resource,std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset) {
	// テクスチャ関連作るよ
	// テクスチャ名が無いなら飛ばす
	if (textureName.size() <= 2) {
		assert(!"テクスチャ名が無い。そのメッシュにはテクスチャが含まれないなどの原因が考えられます");
	}

	// ファイルからテクセルやサイズ情報を読み出す
	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subRes;
	//テクセルやサイズをロード
#ifdef _DEBUG
	char str[59];
	sprintf(str, ToUTF8(textureName).c_str(), "", MB_OK);
	//std::cout << str << "\n";
#endif
	if (!SUCCEEDED(LoadWICTextureFromFile(mDevice.Get(), textureName.c_str(), &resource, decodedData, subRes))) {
		assert(!str);
	}

	// 空白のテクスチャ
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
	//ヒープは読み書きできない高速なDEFAULT
	D3D12_HEAP_PROPERTIES tHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	mDevice->CreateCommittedResource(
		&tHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&tDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	// CPUからGPUにテクスチャデータを渡すときの中継用テクスチャを作る
	ID3D12Resource* textureUploadHeap;
	// ヒープはCPUから読み書きできるUPLOAD
	D3D12_HEAP_PROPERTIES stHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// データをアップロードするために必要な大きさを求める
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

	// コマンドリストを使うからリセットしとく
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), 0);

	// 読みだしたテクセルデータを空白のテクスチャーに流し込み、完成させる
	// コマンドリストにリソース送信命令を入れる
	UpdateSubresources(mCommandList.Get(), resource, textureUploadHeap, 0, 0, 1, &subRes);
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	mCommandList->ResourceBarrier(1, &barrier);

	// テクスチャの送信をするためにコマリスを閉じる
	mCommandList->Close();

	// コマンドリスト実行
	ID3D12CommandList* commandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	WaitGpu();

	// シェダーリソースビューつくる
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
	// 頂点バッファをセット
	mCommandList->IASetVertexBuffers(0, 1, mesh.GetVertexBufferView());

	// コンスタントバッファの内容の更新
	Cbuffer cb;   // 書き込む内容
	char* ptr = reinterpret_cast<char*>(mCBVDataBegin); // 書き込み先アドレス、
	HeapOffset cbvHeapOffset;
	// バッファのどこに書き込むかを調整する
	ptr += mCBSize * cbvHeapOffset.GetHeapOffset();
	// コンスタントバッファ設定
	// ライトビュー
	XMMATRIX lightViewMat = XMMatrixIdentity();
	XMMATRIX lightTranslation = XMMatrixTranslation(0.0f, 60.0f, 0.0f);
	XMMATRIX lightRotation = XMMatrixRotationX(XMConvertToRadians(90.0f));

	lightViewMat = lightRotation * lightTranslation;
	lightViewMat = XMMatrixInverse(nullptr, lightViewMat);

	//シェーダ側は列優先なので転置したものを渡す
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
	
	// UV変換行列
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

	// CBVセット
	// CBV作成時に書いたようにヒープの最初にCBVが入ってる
	D3D12_GPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();


	// ディスクリプタヒープのどこを見るのかをヒープオフセットで調節する
	handle.ptr += cbvHeapOffset.GetHeapOffset() * mCBV_SRV_Stride; // ディスクリプタヒープの位置なのでストライド
	mCommandList->SetGraphicsRootDescriptorTable(0, handle);

	// メモリが解放されるとヒープオフセットが解放されてしまうので描画が終わるまで保存しておく
	mCbvHeapOffsets.push_back(cbvHeapOffset);


	// SRVセット
	// CBVの次にSRVが入ってる
	//if (material.textureName.size() > 2) {
	handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += mesh.GetSrvHeapOffset() * mCBV_SRV_Stride;
	mCommandList->SetGraphicsRootDescriptorTable(1, handle);
	//}

	// インデックスバッファをセット
	mCommandList->IASetIndexBuffer(mesh.GetIndexBufferView());

	// 描画
	// 第一引数は頂点数、フェイスはポリゴン数なので3倍
	mCommandList->DrawIndexedInstanced(mesh.GetNumTriangles() * 3, 1, 0, 0, 0);
}



//------------------------------------------
//  CPUとGPUの同期をする
//------------------------------------------
void Renderer::WaitGpu() {
	// GPUの仕事が終わったときにGPUから帰ってくる値をセットする
	mCommandQueue->Signal(mFence.Get(), mFenceValue);

	do {
		// 待機
		// なんかできるようにしないか?
	} while (mFence->GetCompletedValue() < mFenceValue); // 処理が終わってると関数の戻り値が増えてループ脱出する
	mFenceValue++;
}





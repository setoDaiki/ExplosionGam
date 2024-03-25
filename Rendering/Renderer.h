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

//コンスタントバッファ
struct Cbuffer
{
	DirectX::XMMATRIX w;          // ワールド行列
	DirectX::XMMATRIX wvp;        // ワールドから射影までの変換行列
	DirectX::XMMATRIX wp;         // ワールド * プロジェクション (UI用)
	DirectX::XMMATRIX wlp;        // ワールド * ライトビュー * プロジェクション
	DirectX::XMMATRIX wlpt;       // ワールド * ライトビュー * プロジェクション * UV
	DirectX::XMFLOAT4 lightPos;   // ライト位置
	DirectX::XMFLOAT4 eye;        // 視点位置
	DirectX::XMFLOAT4 ambient;    // 環境光
	DirectX::XMFLOAT4 diffuse;    // 拡散反射光
	DirectX::XMFLOAT4 specular;   // 鏡面反射光
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
	void CreateTexture(ID3D12Resource* resource, std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset);   // テクスチャを作る
	
	void ShadowMap(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);
	void RenderMesh(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);
	void RenderSprite(const DirectX::XMMATRIX& worldMat, const class Camera& camera,const class MyMesh& mesh);
	void RenderUI(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);

	// この関数は不細工すぎるので後で変えたい
	static ID3D12Device* GetDevice() {
		return mDevice.Get();
	}

private:
	static void WaitGpu();                                    // GPUとの同期

	void Render(const DirectX::XMMATRIX& worldMat, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat, const DirectX::XMFLOAT3& eyeVec, const class MyMesh& mesh);

	// 描画インターフェース
	static UINT64 mFenceValue;                                // フェンス値
	static UINT mRTVStride;                                   // レンダ―ターゲットビュー一つ当たりの大きさ
	static UINT mCBV_SRV_Stride;                              // レンダ―ターゲットビューとコンスタントバッファ―ビュー一つ当たりの大きさ
	static UINT mCBSize;                                      // コンスタントバッファ―一つ当たりの大きさ
	static const int FrameCount = 2;                          // フレイムの数(キャンバスの数?)
	static UINT* mCBVDataBegin;
	static ComPtr<IDXGISwapChain3> mSwapChain;                // スワップチェーン
	static ComPtr<ID3D12Device> mDevice;                      // デバイス
	static ComPtr<ID3D12CommandAllocator> mCommandAllocator;  // コマンドアロケータ
	static ComPtr<ID3D12CommandQueue> mCommandQueue;          // コマンドキュー
	static ComPtr<ID3D12GraphicsCommandList> mCommandList;    // コマンドリスト
	static ComPtr<ID3D12Fence> mFence;                        // フェンス
	static unsigned mShadowTextureHeapOffset;                 // シャドウマップ用テクスチャのヒープ位置
	static ComPtr<ID3D12RootSignature> mRootSignature;        // ルートシグネチャ

	// パイプラインステート
	static ComPtr<ID3D12PipelineState> mPipelineState;        // パイプラインステート
	static ComPtr<ID3D12PipelineState> mPipelineUI;           // UI用パイプラインステート
	static ComPtr<ID3D12PipelineState> mPipelineSprite;       // パイプラインステート、スプライト用
	static ComPtr<ID3D12PipelineState> mPipelineWriteShadow;  // シャドウマップ書き込み用パイプライン
	static ComPtr<ID3D12PipelineState> mPipelineReadShadow;   // シャドウマップ読み込み用


	// CBV SRV系
	static ComPtr<ID3D12Resource> mConstantBuffer;            // コンスタントバッファ
	static ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;      // コンスタント、シェダーリソース用のヒープ

	// RTV DSV系
	static ComPtr<ID3D12Resource> mRenderTarget[FrameCount];  // レンダ―ターゲットのリソース
	static ComPtr<ID3D12DescriptorHeap> mRTVHeap;             // レンダ―ターゲットのヒープ
	static ComPtr<ID3D12Resource> mDepthBuffer;               // 深度バッファ
	static ComPtr<ID3D12DescriptorHeap> mDSPHeap;             // 深度バッファのヒープ
	// シャドウマップのためのRTV DSP
	static ComPtr<ID3D12Resource> mShadowRTVTexture;          // シャドウマップのテクスチャレンダ―ターゲット
	static ComPtr<ID3D12DescriptorHeap> mShadowRTVHeap;       // シャドウマップのレンダ―ターゲットのヒープ
	static ComPtr<ID3D12Resource> mShadowDepthBuffer;         // シャドウマップの深度バッファ
	static ComPtr<ID3D12DescriptorHeap> mShadowDSPHeap;       // シャドウマップの深度バッファヒープ

	static std::vector<class HeapOffset> mCbvHeapOffsets;     // コンスタントバッファビューのヒープオフセット
	static bool mIsFullScreenMode;                            // スクリーンモード
	static bool mHasInit;                                     // 初期化済み?
	static bool mIsFirstRenderingOnFrame;                     // このフレームで最初のレンダリング?
	static const unsigned mNumMaxModels = 100;                // モデルの最大数
	static const unsigned mNumMaxMaterials = 10;              // モデル一つあたりの最大マテリアル数
	static const unsigned mNumMaxModelTypes = 20;             // モデルの種類の最大数
	static bool mBeganRender;
	static bool mEndedRender;
};


typedef std::unique_ptr<Renderer> RendererPtrU;
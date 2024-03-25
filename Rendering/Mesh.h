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


// マテリアル構造体
struct MyMaterial
{
	DirectX::XMFLOAT4 ambient;        // アンビエント
	DirectX::XMFLOAT4 diffuse;        // ディフューズ
	DirectX::XMFLOAT4 specular;       // スペキュラー
	std::wstring textureName;         // テクスチャーファイル名
	DXGI_FORMAT textureFormat;        // テクスチャフォーマット
	ComPtr<ID3D12Resource> texture;   // テクスチャリソース
	bool hasTexture;                  // このマテリアルがテクスチャを持っているのか
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

// mesh要素構造体
class  MyMesh {

public:
	//----------------------------------------------------------------------------------
	/// [機能] いくつかのmeshのロード、1モデルに複数のmeshを含む可能性があるのでコンテナ
	/// [引数1] デバイスアドレス
	/// [引数2] ロードするファイル名
	/// [引数3] meshコンテナ 
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
	/// [機能] 四角形メッシュを作る
	/// [引数1] デバイスアドレス
	/// [引数2] 四角形に張り付けるテクスチャ名
	//----------------------------------------------------------------
	void CreateSquareMesh(ID3D12Device* device, const std::wstring& textureName);	


	const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView()const;
	const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView()const;
	MyMaterial GetMaterial()const;
	DWORD GetNumTriangles()const;
	unsigned GetSrvHeapOffset()const;

private:
	


	DWORD mNumTriangles;                              // 三角形数
	std::unique_ptr<class HeapOffset> mSrvHeapOffset; // シェダーリソースのヒープオフセット
	ComPtr<ID3D12Resource> mVertexBuffer;             // 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;       // 頂点バッファビュー
	ComPtr<ID3D12Resource> mIndexBuffer;              // インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;         // インデックスバッファビュー
	MyMaterial mMaterial;                             // マテリアル
	static ComPtr<ID3D12Resource> mBlackTexture;
	static class HeapOffset* mBlackTextureHeapOffset;
};


//頂点の構造体
struct MyVertex
{
	DirectX::XMFLOAT3 pos;     // 位置
	DirectX::XMFLOAT3 normal;  // 法線
	DirectX::XMFLOAT2 uv;      // uv

};




//----------------------------------------------------------------------
/// [機能] テクスチャファイルの拡張子を取得する(png,jpeg,bmp)
/// [機能] パスがくっついてても問題ない
/// [引数] テクスチャファイル名
//----------------------------------------------------------------------
DXGI_FORMAT GetTextureFileFormat(const std::wstring& textureFileName);
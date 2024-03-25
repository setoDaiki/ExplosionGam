#include "Mesh.h"
#include "HeapOffset.h"
#include "Common/FileUtil.h"
#include "HeapOffset.h"
#include "Renderer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3dx12.h>
#include <iostream>

//----------------------------------------------------
//   マテリアルのロード
//----------------------------------------------------
HRESULT LoadMaterial(MyMaterial& dst, const struct aiMaterial* src, const wchar_t* modelFilePath);

MyMesh::MyMesh()
    :mNumTriangles(0)
    , mMaterial()
    , mIndexBuffer(nullptr)
    , mVertexBuffer(nullptr)
    , mVertexBufferView()
    , mIndexBufferView()
    , mSrvHeapOffset(new HeapOffset())
{

}

MyMesh::~MyMesh() {

}


//--------------------------------------------
//   頂点バッファビューの取得
//--------------------------------------------
const D3D12_VERTEX_BUFFER_VIEW* MyMesh::GetVertexBufferView()const {
    return &mVertexBufferView;
}


//---------------------------------------------
//   インデックスバッファビューの取得
//---------------------------------------------
const D3D12_INDEX_BUFFER_VIEW* MyMesh::GetIndexBufferView()const {
    return &mIndexBufferView;
}

//--------------------------------------------
//   マテリアルの取得
//--------------------------------------------
MyMaterial MyMesh::GetMaterial()const {
    return mMaterial;
}

//---------------------------------------------
//    ポリゴン数の取得
//---------------------------------------------
DWORD MyMesh::GetNumTriangles()const {
    return mNumTriangles;
}

//----------------------------------------------------
//    シェダーリソースビューのヒープオフセットを取得
//----------------------------------------------------
unsigned MyMesh::GetSrvHeapOffset()const {
    return mSrvHeapOffset->GetHeapOffset();
}


//----------------------------------------------
//   四角形のメッシュを作る
//----------------------------------------------
void MyMesh::CreateSquareMesh(ID3D12Device* device ,const std::wstring& textureName) {
    //バーテックス　データ作成
    MyVertex triangleVertices[] =
    {
        { { -1.0f, 1.0f , 0.0f}, {0.0f,0.0f,0.0f}, {1.0f,0.0f} },
        { { -1.0f, -1.0f, 0.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f} },
        { {1.0f, 1.0f , 0.0f }, {0.0f,0.0f,0.0f} ,{0.0f,0.0f} },
        { { 1.0f, -1.0f , 0.0f}, {0.0f,0.0f,0.0f} ,{0.0f,1.0f} }
    };
  

    //バーテックスバッファ作成
    const UINT vertexBufferSize = sizeof(triangleVertices);
    D3D12_HEAP_PROPERTIES vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    device->CreateCommittedResource(
        &vertexHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vertexResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mVertexBuffer));

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    mVertexBuffer->Unmap(0, NULL);

    //バーテックスバッファビュー作成
    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    mVertexBufferView.SizeInBytes = vertexBufferSize;

    //インデックスデータ作成
    DWORD indices[] =
    {
        0, 1, 2,
        2, 1, 3,
    };

    //インデックスバッファ作成
    int indexBufferSize = sizeof(indices);
    D3D12_HEAP_PROPERTIES indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
    device->CreateCommittedResource(
        &indexHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &indexResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mIndexBuffer));

    UINT8* pIndexDataBegin;
    mIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices, sizeof(indices));
    mIndexBuffer->Unmap(0, NULL);

    //インデックスバッファビュー作成
    mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
    mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    mIndexBufferView.SizeInBytes = indexBufferSize;
    mNumTriangles = 2;
    mMaterial.hasTexture = true;
    mMaterial.textureName = textureName;
    mMaterial.textureFormat = GetTextureFileFormat(mMaterial.textureName);
    
    // テクスチャ作る
    Renderer renderer;
    renderer.CreateTexture(mMaterial.texture.Get(), mMaterial.textureName, mMaterial.textureFormat, mSrvHeapOffset->GetHeapOffset());

}


void LoadMeshesFromFile(ID3D12Device* device,const wchar_t* filename,std::vector<MyMesh>& meshes)
{
    if (filename == nullptr)
    {
        assert(!"ファイル名がNULL");
    }

   
    // auto& meshes = settings.meshes;
    //auto inverseU = settings.inverseU;
    //auto inverseV = settings.inverseV;

    const std::string path = ToUTF8(filename);


    Assimp::Importer importer;
    int flag = 0;
    // これでファイルをどうやってロードするかが決まる
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_PreTransformVertices;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_GenSmoothNormals;
    flag |= aiProcess_GenUVCoords;
    flag |= aiProcess_RemoveRedundantMaterials;
    flag |= aiProcess_OptimizeMeshes;

    const aiScene* scene = importer.ReadFile(path, flag);

    if (scene == nullptr)
    {
        // もし読み込みエラーがでたら表示する
        MessageBoxA(0,importer.GetErrorString(),"",MB_OK);
        assert(!"読み込み失敗");
    }

    // マテリアルの読み込み
    std::vector<MyMaterial> materials;
    materials.resize(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* material = scene->mMaterials[i];
        if (FAILED(LoadMaterial(materials[i], material, filename))) {  // material(読み込まれたデータ)をmaterials[i]に書き込む
            assert(!"マテリアルの読み込みに失敗");
        }
    }
    // 読み込んだデータを自分で定義したMesh構造体に変換する
    meshes.clear();
    meshes.resize(scene->mNumMeshes);
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        if (FAILED(LoadMesh(device, meshes[i], mesh))) {  // mesh(読み込まれたデータ)をmeshes[i]に書き込む
            assert(!"Meshのロードに失敗");
        }
        meshes[i].mMaterial = materials[mesh->mMaterialIndex];  // メッシュに対応したマテリアルを割り当てる
        if (!meshes[i].mMaterial.hasTexture) {
            Renderer renderer;
            MyMaterial& material = meshes[i].mMaterial;
            std::wstring blackTextureName = L"Asset/Black.png";
            DXGI_FORMAT format = GetTextureFileFormat(blackTextureName);
            renderer.CreateTexture(material.texture.Get(),blackTextureName,format,meshes[i].GetSrvHeapOffset());
        }
        else {
            Renderer renderer;
            MyMaterial& material = meshes[i].mMaterial;
            renderer.CreateTexture(material.texture.Get(),material.textureName,material.textureFormat,meshes[i].GetSrvHeapOffset());
        }
    }
    

    scene = nullptr;

}

//----------------------------------------
//  meshをロードする
//----------------------------------------
HRESULT LoadMesh(ID3D12Device* device,MyMesh& dst, const aiMesh* src)
{

    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

    std::vector<MyVertex> vertices;
    vertices.resize(src->mNumVertices);

    // 頂点情報のロード。頂点数の数だけループさせて読み込む
    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        auto position = &(src->mVertices[i]);   // 頂点位置
        auto normal = &(src->mNormals[i]);      // 頂点法線
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;  // 頂点uv
        //auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        //auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

        /*// 反転オプションがあったらUVを反転させる
        if (inverseU)
        {
            uv->x = 1 - uv->x;
        }
        if (inverseV)
        {
            uv->y = 1 - uv->y;
        }*/

        // 自分の作った頂点構造体にロードした頂点情報を入れる
        MyVertex vertex = {};
        vertex.pos = DirectX::XMFLOAT3(position->x, position->y, position->z);
        vertex.normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
        vertex.uv = DirectX::XMFLOAT2(uv->x, uv->y);
        //vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
        //vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);

        vertices[i] = vertex; // 入れ物に入れる
    }

    dst.mNumTriangles = src->mNumFaces;
    //バーテックスバッファー作成
    const UINT vertexbufferSize = sizeof(MyVertex) * src->mNumVertices;
    D3D12_HEAP_PROPERTIES vHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexbufferSize);
    device->CreateCommittedResource(
        &vHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(dst.mVertexBuffer.GetAddressOf()));
    //バーテックスバッファに頂点データを詰め込む
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    dst.mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vertexbufferSize);
    dst.mVertexBuffer->Unmap(0, NULL);
    //バーテックスバッファビューを初期化
    dst.mVertexBufferView.BufferLocation = dst.mVertexBuffer->GetGPUVirtualAddress();
    dst.mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    dst.mVertexBufferView.SizeInBytes = vertexbufferSize;

    std::vector<int> indices;
    indices.resize(src->mNumFaces * 3);
    // インデックスの読み込み。トライアングル数だけ繰り返す
    for (auto i = 0u; i < src->mNumFaces; ++i)
    {
        const aiFace& face = src->mFaces[i];
        assert(face.mNumIndices == 3);
        indices[i * 3 + 0] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    //インデックスバッファーを作成
    const UINT indexbufferSize = static_cast<UINT>(sizeof(int)) * static_cast<UINT>(indices.size());
    D3D12_HEAP_PROPERTIES iHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC iResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexbufferSize);
    device->CreateCommittedResource(
        &iHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &iResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(dst.mIndexBuffer.GetAddressOf()));
    //インデックスバッファにインデックスデータを詰め込む
    UINT8* pIndexDataBegin;
    CD3DX12_RANGE readRange2(0, 0);
    dst.mIndexBuffer->Map(0, &readRange2, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices.data(), indexbufferSize);
    dst.mIndexBuffer->Unmap(0, NULL);
    //インデックスバッファビューを初期化
    dst.mIndexBufferView.BufferLocation = dst.mIndexBuffer->GetGPUVirtualAddress();
    dst.mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    dst.mIndexBufferView.SizeInBytes = indexbufferSize;

    return S_OK;
}


//-----------------------------------------------
//   マテリアルをロードする
//-----------------------------------------------
HRESULT LoadMaterial(MyMaterial& dst, const aiMaterial* src,const wchar_t* modelFileName)
{
    aiColor3D color(0.0f, 0.0f, 0.0f);
    // 拡散反射光
    if (src->Get(AI_MATKEY_COLOR_DIFFUSE,color)== AI_SUCCESS) {
        dst.diffuse.x = color.r;
        dst.diffuse.y = color.g;
        dst.diffuse.z = color.b;
        dst.diffuse.w = 1.0f;
       
    }
    // ファイルに拡散反射光情報が無いなら適当な数値を入れる
    else {
        dst.diffuse.x = 0.f;
        dst.diffuse.y = 0.f;
        dst.diffuse.z = 0.f;
        dst.diffuse.w = 1.0f;
    }
    // 環境光
    if (src->Get(AI_MATKEY_COLOR_AMBIENT,color) == AI_SUCCESS) {
        dst.ambient.x = color.r;
        dst.ambient.y = color.g;
        dst.ambient.z = color.b;
        dst.ambient.w = 1.0f;
    }
    // ファイルに環境光の情報が無いなら適当な数値を入れる
    else {
        dst.ambient.x = 0.2f;
        dst.ambient.y = 0.2f;
        dst.ambient.z = 0.2f;
        dst.ambient.w = 1.0f;
    }
    // 鏡面反射光
    if (src->Get(AI_MATKEY_COLOR_SPECULAR,color) == AI_SUCCESS) {
        dst.specular.x = color.r;
        dst.specular.y = color.g;
        dst.specular.z = color.b;
        dst.specular.w = 1.0f;
    }
    // ファイルに鏡面反射光の情報が無いなら適当な数値を入れる
    else {
        dst.specular.x = 0.0f;
        dst.specular.y = 0.0f;
        dst.specular.z = 0.0f;
        dst.specular.w = 1.0f;
    }
    // テクスチャ
    aiString path;
    if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
    {
        // テクスチャパスは相対パスで入っているので、ファイルの場所とくっつける
        std::wstring dir = GetDirectoryPathW(modelFileName);
        std::string file = std::string(path.C_Str());
        file = RemoveDirectoryPathA(file);
        dst.textureName = dir + ToWideString(file);
        dst.textureFormat = GetTextureFileFormat(ToWideString(file));
        dst.hasTexture = true;
    }
    else
    {
        dst.textureName.clear();
    }
    return S_OK;
}

//-------------------------------------------
//  PMDファイルをロードする
//-------------------------------------------
HRESULT LoadPmdMeshes(ID3D12Device* device, const wchar_t* filename, std::vector<MyMesh>& meshes) {
    std::cout << "pmdですね" << "\n";
    std::string fileNameString = ToUTF8(filename);
    std::cout << fileNameString.c_str() << "\n";
    auto fp = fopen(fileNameString.c_str(), "rb");
    struct PMDHeader {
        float version;
        char model_name[20];
        char comment[256];
    };
    if (!fp) {
        assert(!"ファイルが開けなかった");
    }
    char signature[3];
    PMDHeader pmdHeader = {};
    fread(signature, sizeof(signature), 1, fp);
    fread(&pmdHeader, sizeof(pmdHeader), 1, fp);
    constexpr size_t pmdVertexSize = 38;  // パディング問題があるので取り合えずはべた書き
    unsigned numVert = 0;
    fread(&numVert, sizeof(numVert), 1, fp);
    std::vector<unsigned char> vertices(numVert * pmdVertexSize);
    fread(vertices.data(),vertices.size(),1,fp);

    //meshes[0].numTriangles = src->mNumFaces;
    //バーテックスバッファー作成
    D3D12_HEAP_PROPERTIES vHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size());
    device->CreateCommittedResource(
        &vHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(meshes[0].mVertexBuffer.GetAddressOf()));
    //バーテックスバッファに頂点データを詰め込む
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    meshes[0].mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vertices.size());
    meshes[0].mVertexBuffer->Unmap(0, NULL);
    //バーテックスバッファビューを初期化
    meshes[0].mVertexBufferView.BufferLocation = meshes[0].mVertexBuffer->GetGPUVirtualAddress();
    meshes[0].mVertexBufferView.StrideInBytes = sizeof(pmdVertexSize);
    meshes[0].mVertexBufferView.SizeInBytes = vertices.size();

    std::vector<int> indices;
    return S_OK;
}


//-------------------------------------------
//   テクスチャファイルのフォーマットを得る
//-------------------------------------------
DXGI_FORMAT GetTextureFileFormat(const std::wstring& textureFileName) {
    std::string extension = ToUTF8(FileExtension(textureFileName)); // 何故かワイド文字列だと文字列比較がうまくいかないのでstringにする
    if (extension == "png") {
        return DXGI_FORMAT_B8G8R8A8_UNORM;   // pngファイルのフォーマット
    }
    else if (extension == "jpg") {
        return DXGI_FORMAT_R8G8B8A8_UNORM;   // jpgファイルのフォーマット
    }
    else if (extension == "bmp") {
        return DXGI_FORMAT_R8G8B8A8_UNORM;   // bmpファイルのフォーマット
    }
    else {
        assert(!"現状対応してない拡張子");
    }
    return DXGI_FORMAT_UNKNOWN;  // コンパイラの警告だるいから適当に返しといてる、どうせassertされる
}



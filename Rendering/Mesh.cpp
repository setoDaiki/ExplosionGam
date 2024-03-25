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
//   �}�e���A���̃��[�h
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
//   ���_�o�b�t�@�r���[�̎擾
//--------------------------------------------
const D3D12_VERTEX_BUFFER_VIEW* MyMesh::GetVertexBufferView()const {
    return &mVertexBufferView;
}


//---------------------------------------------
//   �C���f�b�N�X�o�b�t�@�r���[�̎擾
//---------------------------------------------
const D3D12_INDEX_BUFFER_VIEW* MyMesh::GetIndexBufferView()const {
    return &mIndexBufferView;
}

//--------------------------------------------
//   �}�e���A���̎擾
//--------------------------------------------
MyMaterial MyMesh::GetMaterial()const {
    return mMaterial;
}

//---------------------------------------------
//    �|���S�����̎擾
//---------------------------------------------
DWORD MyMesh::GetNumTriangles()const {
    return mNumTriangles;
}

//----------------------------------------------------
//    �V�F�_�[���\�[�X�r���[�̃q�[�v�I�t�Z�b�g���擾
//----------------------------------------------------
unsigned MyMesh::GetSrvHeapOffset()const {
    return mSrvHeapOffset->GetHeapOffset();
}


//----------------------------------------------
//   �l�p�`�̃��b�V�������
//----------------------------------------------
void MyMesh::CreateSquareMesh(ID3D12Device* device ,const std::wstring& textureName) {
    //�o�[�e�b�N�X�@�f�[�^�쐬
    MyVertex triangleVertices[] =
    {
        { { -1.0f, 1.0f , 0.0f}, {0.0f,0.0f,0.0f}, {1.0f,0.0f} },
        { { -1.0f, -1.0f, 0.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f} },
        { {1.0f, 1.0f , 0.0f }, {0.0f,0.0f,0.0f} ,{0.0f,0.0f} },
        { { 1.0f, -1.0f , 0.0f}, {0.0f,0.0f,0.0f} ,{0.0f,1.0f} }
    };
  

    //�o�[�e�b�N�X�o�b�t�@�쐬
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

    //�o�[�e�b�N�X�o�b�t�@�r���[�쐬
    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    mVertexBufferView.SizeInBytes = vertexBufferSize;

    //�C���f�b�N�X�f�[�^�쐬
    DWORD indices[] =
    {
        0, 1, 2,
        2, 1, 3,
    };

    //�C���f�b�N�X�o�b�t�@�쐬
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

    //�C���f�b�N�X�o�b�t�@�r���[�쐬
    mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
    mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    mIndexBufferView.SizeInBytes = indexBufferSize;
    mNumTriangles = 2;
    mMaterial.hasTexture = true;
    mMaterial.textureName = textureName;
    mMaterial.textureFormat = GetTextureFileFormat(mMaterial.textureName);
    
    // �e�N�X�`�����
    Renderer renderer;
    renderer.CreateTexture(mMaterial.texture.Get(), mMaterial.textureName, mMaterial.textureFormat, mSrvHeapOffset->GetHeapOffset());

}


void LoadMeshesFromFile(ID3D12Device* device,const wchar_t* filename,std::vector<MyMesh>& meshes)
{
    if (filename == nullptr)
    {
        assert(!"�t�@�C������NULL");
    }

   
    // auto& meshes = settings.meshes;
    //auto inverseU = settings.inverseU;
    //auto inverseV = settings.inverseV;

    const std::string path = ToUTF8(filename);


    Assimp::Importer importer;
    int flag = 0;
    // ����Ńt�@�C�����ǂ�����ă��[�h���邩�����܂�
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
        // �����ǂݍ��݃G���[���ł���\������
        MessageBoxA(0,importer.GetErrorString(),"",MB_OK);
        assert(!"�ǂݍ��ݎ��s");
    }

    // �}�e���A���̓ǂݍ���
    std::vector<MyMaterial> materials;
    materials.resize(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* material = scene->mMaterials[i];
        if (FAILED(LoadMaterial(materials[i], material, filename))) {  // material(�ǂݍ��܂ꂽ�f�[�^)��materials[i]�ɏ�������
            assert(!"�}�e���A���̓ǂݍ��݂Ɏ��s");
        }
    }
    // �ǂݍ��񂾃f�[�^�������Œ�`����Mesh�\���̂ɕϊ�����
    meshes.clear();
    meshes.resize(scene->mNumMeshes);
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        if (FAILED(LoadMesh(device, meshes[i], mesh))) {  // mesh(�ǂݍ��܂ꂽ�f�[�^)��meshes[i]�ɏ�������
            assert(!"Mesh�̃��[�h�Ɏ��s");
        }
        meshes[i].mMaterial = materials[mesh->mMaterialIndex];  // ���b�V���ɑΉ������}�e���A�������蓖�Ă�
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
//  mesh�����[�h����
//----------------------------------------
HRESULT LoadMesh(ID3D12Device* device,MyMesh& dst, const aiMesh* src)
{

    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

    std::vector<MyVertex> vertices;
    vertices.resize(src->mNumVertices);

    // ���_���̃��[�h�B���_���̐��������[�v�����ēǂݍ���
    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        auto position = &(src->mVertices[i]);   // ���_�ʒu
        auto normal = &(src->mNormals[i]);      // ���_�@��
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;  // ���_uv
        //auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        //auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

        /*// ���]�I�v�V��������������UV�𔽓]������
        if (inverseU)
        {
            uv->x = 1 - uv->x;
        }
        if (inverseV)
        {
            uv->y = 1 - uv->y;
        }*/

        // �����̍�������_�\���̂Ƀ��[�h�������_��������
        MyVertex vertex = {};
        vertex.pos = DirectX::XMFLOAT3(position->x, position->y, position->z);
        vertex.normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
        vertex.uv = DirectX::XMFLOAT2(uv->x, uv->y);
        //vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
        //vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);

        vertices[i] = vertex; // ���ꕨ�ɓ����
    }

    dst.mNumTriangles = src->mNumFaces;
    //�o�[�e�b�N�X�o�b�t�@�[�쐬
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
    //�o�[�e�b�N�X�o�b�t�@�ɒ��_�f�[�^���l�ߍ���
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    dst.mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vertexbufferSize);
    dst.mVertexBuffer->Unmap(0, NULL);
    //�o�[�e�b�N�X�o�b�t�@�r���[��������
    dst.mVertexBufferView.BufferLocation = dst.mVertexBuffer->GetGPUVirtualAddress();
    dst.mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    dst.mVertexBufferView.SizeInBytes = vertexbufferSize;

    std::vector<int> indices;
    indices.resize(src->mNumFaces * 3);
    // �C���f�b�N�X�̓ǂݍ��݁B�g���C�A���O���������J��Ԃ�
    for (auto i = 0u; i < src->mNumFaces; ++i)
    {
        const aiFace& face = src->mFaces[i];
        assert(face.mNumIndices == 3);
        indices[i * 3 + 0] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    //�C���f�b�N�X�o�b�t�@�[���쐬
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
    //�C���f�b�N�X�o�b�t�@�ɃC���f�b�N�X�f�[�^���l�ߍ���
    UINT8* pIndexDataBegin;
    CD3DX12_RANGE readRange2(0, 0);
    dst.mIndexBuffer->Map(0, &readRange2, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices.data(), indexbufferSize);
    dst.mIndexBuffer->Unmap(0, NULL);
    //�C���f�b�N�X�o�b�t�@�r���[��������
    dst.mIndexBufferView.BufferLocation = dst.mIndexBuffer->GetGPUVirtualAddress();
    dst.mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    dst.mIndexBufferView.SizeInBytes = indexbufferSize;

    return S_OK;
}


//-----------------------------------------------
//   �}�e���A�������[�h����
//-----------------------------------------------
HRESULT LoadMaterial(MyMaterial& dst, const aiMaterial* src,const wchar_t* modelFileName)
{
    aiColor3D color(0.0f, 0.0f, 0.0f);
    // �g�U���ˌ�
    if (src->Get(AI_MATKEY_COLOR_DIFFUSE,color)== AI_SUCCESS) {
        dst.diffuse.x = color.r;
        dst.diffuse.y = color.g;
        dst.diffuse.z = color.b;
        dst.diffuse.w = 1.0f;
       
    }
    // �t�@�C���Ɋg�U���ˌ���񂪖����Ȃ�K���Ȑ��l������
    else {
        dst.diffuse.x = 0.f;
        dst.diffuse.y = 0.f;
        dst.diffuse.z = 0.f;
        dst.diffuse.w = 1.0f;
    }
    // ����
    if (src->Get(AI_MATKEY_COLOR_AMBIENT,color) == AI_SUCCESS) {
        dst.ambient.x = color.r;
        dst.ambient.y = color.g;
        dst.ambient.z = color.b;
        dst.ambient.w = 1.0f;
    }
    // �t�@�C���Ɋ����̏�񂪖����Ȃ�K���Ȑ��l������
    else {
        dst.ambient.x = 0.2f;
        dst.ambient.y = 0.2f;
        dst.ambient.z = 0.2f;
        dst.ambient.w = 1.0f;
    }
    // ���ʔ��ˌ�
    if (src->Get(AI_MATKEY_COLOR_SPECULAR,color) == AI_SUCCESS) {
        dst.specular.x = color.r;
        dst.specular.y = color.g;
        dst.specular.z = color.b;
        dst.specular.w = 1.0f;
    }
    // �t�@�C���ɋ��ʔ��ˌ��̏�񂪖����Ȃ�K���Ȑ��l������
    else {
        dst.specular.x = 0.0f;
        dst.specular.y = 0.0f;
        dst.specular.z = 0.0f;
        dst.specular.w = 1.0f;
    }
    // �e�N�X�`��
    aiString path;
    if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
    {
        // �e�N�X�`���p�X�͑��΃p�X�œ����Ă���̂ŁA�t�@�C���̏ꏊ�Ƃ�������
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
//  PMD�t�@�C�������[�h����
//-------------------------------------------
HRESULT LoadPmdMeshes(ID3D12Device* device, const wchar_t* filename, std::vector<MyMesh>& meshes) {
    std::cout << "pmd�ł���" << "\n";
    std::string fileNameString = ToUTF8(filename);
    std::cout << fileNameString.c_str() << "\n";
    auto fp = fopen(fileNameString.c_str(), "rb");
    struct PMDHeader {
        float version;
        char model_name[20];
        char comment[256];
    };
    if (!fp) {
        assert(!"�t�@�C�����J���Ȃ�����");
    }
    char signature[3];
    PMDHeader pmdHeader = {};
    fread(signature, sizeof(signature), 1, fp);
    fread(&pmdHeader, sizeof(pmdHeader), 1, fp);
    constexpr size_t pmdVertexSize = 38;  // �p�f�B���O��肪����̂Ŏ�荇�����ׂ͂�����
    unsigned numVert = 0;
    fread(&numVert, sizeof(numVert), 1, fp);
    std::vector<unsigned char> vertices(numVert * pmdVertexSize);
    fread(vertices.data(),vertices.size(),1,fp);

    //meshes[0].numTriangles = src->mNumFaces;
    //�o�[�e�b�N�X�o�b�t�@�[�쐬
    D3D12_HEAP_PROPERTIES vHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size());
    device->CreateCommittedResource(
        &vHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(meshes[0].mVertexBuffer.GetAddressOf()));
    //�o�[�e�b�N�X�o�b�t�@�ɒ��_�f�[�^���l�ߍ���
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    meshes[0].mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vertices.size());
    meshes[0].mVertexBuffer->Unmap(0, NULL);
    //�o�[�e�b�N�X�o�b�t�@�r���[��������
    meshes[0].mVertexBufferView.BufferLocation = meshes[0].mVertexBuffer->GetGPUVirtualAddress();
    meshes[0].mVertexBufferView.StrideInBytes = sizeof(pmdVertexSize);
    meshes[0].mVertexBufferView.SizeInBytes = vertices.size();

    std::vector<int> indices;
    return S_OK;
}


//-------------------------------------------
//   �e�N�X�`���t�@�C���̃t�H�[�}�b�g�𓾂�
//-------------------------------------------
DXGI_FORMAT GetTextureFileFormat(const std::wstring& textureFileName) {
    std::string extension = ToUTF8(FileExtension(textureFileName)); // ���̂����C�h�����񂾂ƕ������r�����܂������Ȃ��̂�string�ɂ���
    if (extension == "png") {
        return DXGI_FORMAT_B8G8R8A8_UNORM;   // png�t�@�C���̃t�H�[�}�b�g
    }
    else if (extension == "jpg") {
        return DXGI_FORMAT_R8G8B8A8_UNORM;   // jpg�t�@�C���̃t�H�[�}�b�g
    }
    else if (extension == "bmp") {
        return DXGI_FORMAT_R8G8B8A8_UNORM;   // bmp�t�@�C���̃t�H�[�}�b�g
    }
    else {
        assert(!"����Ή����ĂȂ��g���q");
    }
    return DXGI_FORMAT_UNKNOWN;  // �R���p�C���̌x�����邢����K���ɕԂ��Ƃ��Ă�A�ǂ���assert�����
}



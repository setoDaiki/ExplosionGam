#pragma once
#include <string>
#include <vector>
#include "packages/rapidjson/document.h"
#include "packages/rapidjson/stringbuffer.h"
#include "packages/rapidjson/prettywriter.h"

class JsonFile {
public:
	//-----------------------------------------------
	/// [�@�\] Json�t�@�C������͂���B�x�����璍��
	/// [����] ��͂���t�@�C����
	//-----------------------------------------------
	explicit JsonFile(const std::string& fileName);

	//----------------------------------------------------------------------
	/// [�@�\] �ύX���ꂽJson�t�@�C����ۑ�����B����܂��x���̂Œ���
	//----------------------------------------------------------------------
	void SaveFile();

	//--------------------------------------
	/// [�@�\] �t�@�C������l��ǂݎ��
	/// [����] �l���i�[���Ă���L�[�̖��O
	//--------------------------------------
	int GetInt(const char* keyName);
	float GetFloat(const char* keyName);
	bool GetBool(const char* keyName);
	std::string GetString(const char* keyName);
	rapidjson::GenericArray<false, rapidjson::Value> GetArray(const char* keyName);

	//--------------------------------------------------
	/// [�@�\]  �t�@�C���̒l������������
	/// [����1] ����������l���i�[���Ă���L�[�̖��O
	/// [����2] �i�[����l
	//--------------------------------------------------
	void SetInt(const char* keyName, int setValue);
	void SetFloat(const char* keyName, float setValue);
	void SetBool(const char* keyName,bool setValue);
	void SetString(const char* keyName,const rapidjson::GenericStringRef<char>& setValue);
	

private:
	rapidjson::GenericValue<rapidjson::UTF8<>>& GetProperty(const char* keyName);
	rapidjson::Document mFileData;
	std::string mFileName;
};

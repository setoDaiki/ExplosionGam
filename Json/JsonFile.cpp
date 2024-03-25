#include "JsonFile.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <memory>

JsonFile::JsonFile(const std::string& fileName)
	:mFileName(fileName)
{
	std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		assert(!"Json�t�@�C�����J���Ȃ�����");

	}
	std::ifstream::pos_type fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
	file.read(bytes.data(), static_cast<size_t>(fileSize));
	std::cout << "\n";
	mFileData.Parse(bytes.data());
	if (!mFileData.IsObject()) {
		assert(!"Json�̉�͂ł��Ȃ�����");
	}
}

void JsonFile::SaveFile() {
	rapidjson::StringBuffer buffer;

	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	mFileData.Accept(writer);
	const char* output = buffer.GetString();

	// Write output to file
	std::ofstream outFile(mFileName);
	if (outFile.is_open())
	{
		outFile << output;
	}
}

//---------------------------------------
//  �L�[�ɑΉ�����int�l���擾����
//---------------------------------------
int JsonFile::GetInt(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int�^����Ȃ�");
	}
	return property.GetInt();
}

//--------------------------------------
//  �L�[�ɑΉ�����int�l�ɃZ�b�g����
//--------------------------------------
void JsonFile::SetInt(const char* keyName,int setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int�^����Ȃ�");
	}
	property.SetInt(setValue);
}

//----------------------------------
// �L�[�ɑΉ�����floa�l���擾����
//----------------------------------
float JsonFile::GetFloat(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float�^����Ȃ�");
	}
	return property.GetFloat();
}

//----------------------------------------
//  �L�[�ɑΉ�����float�l�ɃZ�b�g����
//----------------------------------------
void JsonFile::SetFloat(const char* keyName,float setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float�^����Ȃ�");
	}
	property.SetFloat(setValue);
}

//-------------------------------------
//  �L�[�ɑΉ�����bool�l���擾����
//-------------------------------------
bool JsonFile::GetBool(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool�^����Ȃ�");
	}
	return property.GetBool();
}

//----------------------------------
//  �L�[�ɑΉ�����bool�l�ɃZ�b�g����
//----------------------------------
void JsonFile::SetBool(const char* keyName,bool setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool�^����Ȃ�");
	}
	property.SetBool(setValue);
}

//-----------------------------------
//  �L�[�ɑΉ����镶������擾����
//-----------------------------------
std::string JsonFile::GetString(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String�^����Ȃ�");
	}
	return property.GetString();
}

//--------------------------------------
//  �L�[�ɑΉ����镶����ɃZ�b�g����
//--------------------------------------
void JsonFile::SetString(const char* keyName,const rapidjson::GenericStringRef<char>& setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String�^����Ȃ�");
	}
	property.SetString(setValue);
}

rapidjson::GenericArray<false,rapidjson::Value> JsonFile::GetArray(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsArray()) {
		assert(!"�z�񂶂�Ȃ�");
	}
	return property.GetArray();
}


rapidjson::GenericValue<rapidjson::UTF8<>>& JsonFile::GetProperty(const char* keyName) {
	auto iter = mFileData.FindMember(keyName);
	if (iter == mFileData.MemberEnd()) {
		assert(!"�t�@�C���f�[�^����L�[�̖��O��������Ȃ�");
	}
	return iter->value;
}
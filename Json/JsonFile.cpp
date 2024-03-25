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
		assert(!"Jsonファイルが開けなかった");

	}
	std::ifstream::pos_type fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
	file.read(bytes.data(), static_cast<size_t>(fileSize));
	std::cout << "\n";
	mFileData.Parse(bytes.data());
	if (!mFileData.IsObject()) {
		assert(!"Jsonの解析できなかった");
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
//  キーに対応するint値を取得する
//---------------------------------------
int JsonFile::GetInt(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int型じゃない");
	}
	return property.GetInt();
}

//--------------------------------------
//  キーに対応するint値にセットする
//--------------------------------------
void JsonFile::SetInt(const char* keyName,int setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int型じゃない");
	}
	property.SetInt(setValue);
}

//----------------------------------
// キーに対応するfloa値を取得する
//----------------------------------
float JsonFile::GetFloat(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float型じゃない");
	}
	return property.GetFloat();
}

//----------------------------------------
//  キーに対応するfloat値にセットする
//----------------------------------------
void JsonFile::SetFloat(const char* keyName,float setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float型じゃない");
	}
	property.SetFloat(setValue);
}

//-------------------------------------
//  キーに対応するbool値を取得する
//-------------------------------------
bool JsonFile::GetBool(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool型じゃない");
	}
	return property.GetBool();
}

//----------------------------------
//  キーに対応するbool値にセットする
//----------------------------------
void JsonFile::SetBool(const char* keyName,bool setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool型じゃない");
	}
	property.SetBool(setValue);
}

//-----------------------------------
//  キーに対応する文字列を取得する
//-----------------------------------
std::string JsonFile::GetString(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String型じゃない");
	}
	return property.GetString();
}

//--------------------------------------
//  キーに対応する文字列にセットする
//--------------------------------------
void JsonFile::SetString(const char* keyName,const rapidjson::GenericStringRef<char>& setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String型じゃない");
	}
	property.SetString(setValue);
}

rapidjson::GenericArray<false,rapidjson::Value> JsonFile::GetArray(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsArray()) {
		assert(!"配列じゃない");
	}
	return property.GetArray();
}


rapidjson::GenericValue<rapidjson::UTF8<>>& JsonFile::GetProperty(const char* keyName) {
	auto iter = mFileData.FindMember(keyName);
	if (iter == mFileData.MemberEnd()) {
		assert(!"ファイルデータからキーの名前が見つからない");
	}
	return iter->value;
}
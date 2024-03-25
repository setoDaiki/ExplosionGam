#pragma once
#include <string>
#include <vector>
#include "packages/rapidjson/document.h"
#include "packages/rapidjson/stringbuffer.h"
#include "packages/rapidjson/prettywriter.h"

class JsonFile {
public:
	//-----------------------------------------------
	/// [機能] Jsonファイルを解析する。遅いから注意
	/// [引数] 解析するファイル名
	//-----------------------------------------------
	explicit JsonFile(const std::string& fileName);

	//----------------------------------------------------------------------
	/// [機能] 変更されたJsonファイルを保存する。これまた遅いので注意
	//----------------------------------------------------------------------
	void SaveFile();

	//--------------------------------------
	/// [機能] ファイルから値を読み取る
	/// [引数] 値を格納しているキーの名前
	//--------------------------------------
	int GetInt(const char* keyName);
	float GetFloat(const char* keyName);
	bool GetBool(const char* keyName);
	std::string GetString(const char* keyName);
	rapidjson::GenericArray<false, rapidjson::Value> GetArray(const char* keyName);

	//--------------------------------------------------
	/// [機能]  ファイルの値を書き換える
	/// [引数1] 書き換える値を格納しているキーの名前
	/// [引数2] 格納する値
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

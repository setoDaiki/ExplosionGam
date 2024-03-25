#pragma once
#include <unordered_map>
#include <string>
#include <memory>

class ModelManager {
public:
	
	static void Destroy();
	static std::shared_ptr<class Model> GetModel(const std::string& filePath);
	//void DeleteModel(const std::wstring& filePath);


	

private:
	ModelManager() {};
	~ModelManager() {};

	static std::unordered_map<std::string, std::shared_ptr<class Model>> mModels;

};

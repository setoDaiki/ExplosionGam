#include "ModelManager.h"
#include "ObjectComponent/Model.h"
#include "Common/FileUtil.h"
#include "Scene/Sequence.h"
#ifdef _DEBUG
#include <iostream>
#endif

std::unordered_map<std::string, ModelPtrS> ModelManager::mModels;


void ModelManager::Destroy() {
	mModels.clear();
}

ModelPtrS ModelManager::GetModel(const std::string& fileName) {
	auto iter = mModels.find(fileName);
	if (iter != mModels.end()) {
		return iter->second;
	}
	else {
#ifdef _DEBUG
		if (Sequence::IsRunTime()) {
			std::cout << "RunTime中のモデルロード : " << fileName << "\n";
			assert(!"ランタイム中に新しいモデルをロードしないで!!");
		}
#endif
		ModelPtrS model = std::make_shared<Model>(ToWideString(fileName).c_str());
		mModels.emplace(fileName, model);
		return mModels[fileName];
	}
}
#pragma once
#include "ModelStructs.h"
#include <list>

enum ModelName {
	CUBE,
	SPHERE,
	MODEL_COUNT
};

class ModelManager
{
public:
	// コンストラクタ
	ModelManager() = default;
	// デストラクタ
	~ModelManager() = default;

	// シングルトン
	static ModelManager* GetInstance();

	// 初期化
	void Initialize();

	// objファイルの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	// Getter
	ModelData* GetModelData() { return modelData_; }

private:
	ModelData modelData_[MODEL_COUNT];
};


#include "GameScene.h"

#include <string>
#include "json.hpp"
#include <fstream>
#include <cassert>
#include <vector>
#include <KamataEngine.h>

using namespace KamataEngine;
GameScene::GameScene() {}


GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();

	// ------------------------------------------------------------
	// レベルデータを格納するための構造体
	// ------------------------------------------------------------


	struct ObjectData
	{
		std::string type;
		std::string name;

		struct Transform
		{
			KamataEngine::Vector3 translation;
			KamataEngine::Vector3 rotation;
			KamataEngine::Vector3 scaling;
		};

		Transform transform;

		std::string file_name;
	};

	struct LevelData
	{
		std::string name;
		std::vector<ObjectData>objects;
	};

	// ------------------------------------------------------------
	// jsonファイルのデシリアライズ化
	// ------------------------------------------------------------

	// jsonファイルのパス名
	const std::string fullpath = std::string("Resources/levels/") + "scene.json";

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	nlohmann::json deserialized;

	// ファイルから読み込み、メモリへ格納
	file >> deserialized;


	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());

	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// ------------------------------------------------------------
	// レベルデータを構造体に格納していく
	// ------------------------------------------------------------

	LevelData* levelData = new LevelData();

	// "name" を文字列として取得
	levelData->name = deserialized["name"].get<std::string>();
	assert(levelData->name == "scene");    // それは "scene" か？

	// "objects" の全オブジェクトを走査
	for (const nlohmann::json& object : deserialized["objects"]) {

		// オブジェクト1つ分の妥当性のチェック
		assert(object.contains("type"));    // "type" が含まれているか

		if (object["type"].get<std::string>() == "MESH") {

			// 1個分の要素の準備
			levelData->objects.emplace_back(ObjectData{});
			ObjectData& objectData = levelData->objects.back();  // 追加要素の参照を用意して可読性もよくなる

			objectData.type = object["type"].get<std::string>();   // "type"
			objectData.name = object["name"].get<std::string>();   // "name"

			// トランスフォームのパラメータ読み込み
			nlohmann::json transform = object["transform"];
			// 平行移動 "translation"
			objectData.transform.translation.x = (float)transform["translation"][0];
			objectData.transform.translation.y = (float)transform["translation"][2];
			objectData.transform.translation.z = (float)transform["translation"][1];
			// 回転 "rotation"
			objectData.transform.rotation.x = -(float)transform["rotation"][0];
			objectData.transform.rotation.y = -(float)transform["rotation"][2];
			objectData.transform.rotation.z = -(float)transform["rotation"][1];
			// 拡縮 "scaling"
			objectData.transform.scaling.x = (float)transform["scaling"][0];
			objectData.transform.scaling.y = (float)transform["scaling"][2];
			objectData.transform.scaling.z = (float)transform["scaling"][1];

			// "file_name"
			if (object.contains("file_name")) {
				objectData.file_name = object["file_name"].get<std::string>();
			}
		}
	}

	Camera* camera = new Camera();
	camera->Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	for (auto& objectData : levelData->objects) {
		
		if (objectData.file_name.empty()) {
			continue;
		}

		WorldTransform* worldTransform = new WorldTransform();
		worldTransform->Initialize();
		worldTransform->translation_ = objectData.transform.translation;
		worldTransform->rotation_ = objectData.transform.rotation;
		worldTransform->scale_ = objectData.transform.scaling;
		worldTransform->UpdateMatrix();

		Model* model = nullptr;
		model = Model::CreateFromOBJ(objectData.file_name);

		models_.push_back(model);
		worldTransforms_.push_back(worldTransform);
	}
}

void GameScene::Update() {}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (int i = 0; i < models_.size(); ++i) {
		models_[i]->Draw(*worldTransforms_[i], *camera_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

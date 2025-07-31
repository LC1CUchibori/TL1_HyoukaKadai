#pragma once

#include "KamataEngine.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	std::vector<KamataEngine::Model*> models_;                     // モデルのポインタ配列
	std::vector<KamataEngine::WorldTransform*> worldTransforms_; 

	KamataEngine::Camera* camera_ = nullptr;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};

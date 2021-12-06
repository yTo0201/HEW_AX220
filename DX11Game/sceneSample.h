//===================================================
//		サンプルシーン[sceneSample.h]
//小楠裕子
//2021/11/30	ファイル作成
//=====================================================
#pragma once
#include "main.h"
#include "cube.h"

HRESULT InitSceneSample();
void UninitSceneSample();
void UpdateSceneSample();
void DrawSceneSample();

Cube* GetCube();
//===================================================
//		�Q�[���V�[��[sceneGame.h]
//����T�q
//=====================================================
#pragma once
#include "main.h"
#include "old.h"
#include "now.h"

HRESULT InitSceneGame();
void UninitSceneGame();
void UpdateSceneGame();
void DrawSceneGame();

Old* GetOld();
Now* GetNow();
//=============================================================================
//
// 敵機処理 [enemy.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);
bool CollisionEnemy(XMFLOAT3 pos, float radius);
bool CollisionEnemy(XMFLOAT3 pos, XMFLOAT3 size);

//========================================================
//
//ビルボード弾[bullet.h]
//
//========================================================
#pragma once

#include "main.h"

//********************************************************
//プロトタイプ宣言
//********************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int FireBullet(XMFLOAT3 pos, XMFLOAT3 dir);
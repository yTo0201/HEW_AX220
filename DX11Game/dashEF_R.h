//========================================================
//
//爆発処理[explosion.h]
//
//========================================================
#pragma once

#include "main.h"

//********************************************************
//プロトタイプ宣言
//********************************************************
HRESULT InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);

int StartExplosion(XMFLOAT3 pos, XMFLOAT2 size);
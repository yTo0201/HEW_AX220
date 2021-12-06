//========================================================
//
//炎エフェクト[fire.h]
//
//========================================================
#pragma once

#include "main.h"

//********************************************************
//プロトタイプ宣言
//********************************************************
HRESULT InitFire(void);
void UninitFire(void);
void UpdateFire(void);
void DrawFire(void);

int StartFire(XMFLOAT3 pos, float size, XMFLOAT4X4 world);
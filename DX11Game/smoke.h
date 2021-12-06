//========================================================
//
//煙エフェクト[smoke.h]
//
//========================================================
#pragma once

#include "main.h"

//********************************************************
//プロトタイプ宣言
//********************************************************
HRESULT InitSmoke(void);
void UninitSmoke(void);
void UpdateSmoke(void);
void DrawSmoke(void);

int StartSmoke(XMFLOAT3 pos, XMFLOAT2 size);
//========================================================
//
//��������[explosion.h]
//
//========================================================
#pragma once

#include "main.h"

//********************************************************
//�v���g�^�C�v�錾
//********************************************************
HRESULT InitDashEF(void);
void UninitDashEF(void);
void UpdateDashEF(void);
void DrawDashEF(void);

int StartDashEF(XMFLOAT3 pos, XMFLOAT2 size);
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
HRESULT InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);

int StartExplosion(XMFLOAT3 pos, XMFLOAT2 size);
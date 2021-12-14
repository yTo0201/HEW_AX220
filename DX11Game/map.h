//=============================================================================
//
// マップ [map.h]
// 小楠裕子
//=============================================================================
#pragma once
#include "main.h"
#include "box.h"
//*****構造体定義*****
typedef struct {
	int m_nCategory;	//オブジェクトの種類
	int m_nObject;		//オブジェクト管理番号
}OBJECT_INFO;

//*****列挙型*****
enum MAP_OBJECT { NONE, NORMAL, MOVE };		// オブジェクトの種類

//*****マクロ定義*****
#define MAP_WIDTH	19
#define MAP_HEIGHT	5

HRESULT InitMap();
void UninitMap();
void UpdateMap();
void DrawOldMap();
void DrawNowMap();

OBJECT_INFO CollisionOldMap(XMFLOAT2 pos, XMFLOAT2 size);
OBJECT_INFO	CollisionNowMap(XMFLOAT2 pos, XMFLOAT2 size);

Box* GetBox();
OBJECT_INFO* GetNowMap();

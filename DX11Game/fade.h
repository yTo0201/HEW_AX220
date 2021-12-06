//フェードイン・フェードアウト処理[fade.h]
#pragma once
#include "scene.h"

//状態
enum EFade {
	FADE_NONE = 0,		//何もしていない
	FADE_IN,			//フェードイン処理中
	FADE_OUT,			//フェードアウト処理中

	MAX_FADE
};

//関数プロトタイプ
HRESULT InitFade();		//初期化
void UninitFade();		//終了処理
void UpdateFade();		//更新処理
void DrawFade();		//描画

void StartFadeOut(EScene eNext);					//フェードアウト開始
EFade GetFade();									//状態取得
void SetFadeColor(float fR, float fG, float fB);	//フェードアウト色設定
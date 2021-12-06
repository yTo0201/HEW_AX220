// 背景 [bg.cpp]
#include "bg.h"
#include "scene.h"
#include "Texture.h"
#include "polygon.h"

// マクロ定義
#define BG_TEXTURE_PATH_SAMPLE	L"data/texture/sky001.jpg"
#define BG_TEXTURE_PATH_TITLE	L"data/texture/field000.jpg"
#define BG_POS_X		0.0f
#define BG_POS_Y		0.0f
#define BG_SIZE_X		SCREEN_WIDTH
#define BG_SIZE_Y		SCREEN_HEIGHT

//// グローバル変数
//static ID3D11ShaderResourceView* g_pTexture;
//
//// 初期化
//HRESULT InitBG()
//{
//	HRESULT hr = S_OK;
//	ID3D11Device* pDevice = GetDevice();
//
//	// テクスチャ読込
//	hr = CreateTextureFromFile(pDevice, PATH_BGTEXTURE, &g_pTexture);
//
//	return hr;
//}
//
//// 終了処理
//void UninitBG()
//{
//	// 背景テクスチャ解放
//	SAFE_RELEASE(g_pTexture);
//}
//
//// 更新
//void UpdateBG()
//{
//	// 何もしない
//}
//
//// 描画
//void DrawBG()
//{
//	ID3D11DeviceContext* pDC = GetDeviceContext();
//	SetPolygonSize(BG_WIDTH, BG_HEIGHT);
//	SetPolygonPos(BG_POS_X, BG_POS_Y);
//	SetPolygonTexture(g_pTexture);
//	SetPolygonUV(0.0f, 0.0f);
//	DrawPolygon(pDC);
//}


//==============================================================
//ｺﾝｽﾄﾗｸﾀ
//==============================================================
BG::BG()
	:m_pos(XMFLOAT2(BG_POS_X, BG_POS_Y))
	,m_size(XMFLOAT2(BG_SIZE_X, BG_SIZE_Y))
{
	//テクスチャ読み込み
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	switch (GetScene()) {
	case SCENE_TITLE:
		hr = CreateTextureFromFile(pDevice, BG_TEXTURE_PATH_TITLE, &m_pTexture);
		break;
	case SCENE_GAME:
		//hr = CreateTextureFromFile(pDevice, BG_TEXTURE_PATH_GAME, &m_pTexture);
		break;
	case SCENE_RESULT:
		//hr = CreateTextureFromFile(pDevice, BG_TEXTURE_PATH_RESULT, &m_pTexture);
		break;
	case SCENE_SAMPLE:
		hr = CreateTextureFromFile(pDevice, BG_TEXTURE_PATH_SAMPLE, &m_pTexture);
	}
}
//==============================================================
//ﾃﾞｽﾄﾗｸﾀ
//==============================================================
BG::~BG() {
	//テクスチャ解放
	SAFE_RELEASE(m_pTexture);
}
//==============================================================
//更新
//==============================================================
void BG::Update() {

}
//==============================================================
//描画
//==============================================================
void BG::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetPolygonSize(m_size.x, m_size.y);
	SetPolygonPos(m_pos.x, m_pos.y);
	SetPolygonTexture(m_pTexture);
	SetPolygonUV(0.0f, 0.0f);
	DrawPolygon(pDC);

	//元に戻す
}

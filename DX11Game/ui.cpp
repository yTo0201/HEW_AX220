//=============================================================================
//
// UI [ui.cpp]
// 小楠裕子
//2021/11/30			ファイル作成
//=============================================================================
#include "ui.h"
#include "Texture.h"
#include "polygon.h"

//*****定数定義*****
#define UI_POS_X		500.0f		//位置X
#define UI_POS_Y		500.0f		//位置Y
#define UI_SIZE_X	100.0f			//横幅
#define UI_SIZE_Y	100.0f			//縦幅
#define UI_TEXTURE_PATH		L"data/texture/number.png"
#define UI_TEXTURE_SPLIT_X	5		//テクスチャ分割数横
#define UI_TEXTURE_SPLIT_Y	5		//テクスチャ分割数縦
#define UI_FRAME			12		//表示する枠

//*****グローバル変数*****


//==============================================================
//ｺﾝｽﾄﾗｸﾀ
//==============================================================
UI::UI()
	:m_pos(XMFLOAT2(UI_POS_X, UI_POS_Y))
	, m_size(XMFLOAT2(UI_SIZE_X, UI_SIZE_Y))
{
	//テクスチャ読み込み
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	hr = CreateTextureFromFile(pDevice, UI_TEXTURE_PATH, &m_pTexture);
}
//==============================================================
//ﾃﾞｽﾄﾗｸﾀ
//==============================================================
UI::~UI() {
	//テクスチャ解放
	SAFE_RELEASE(m_pTexture);
}
//==============================================================
//更新
//==============================================================
void UI::Update() {

}
//==============================================================
//描画
//==============================================================
void UI::Draw() {
	SetBlendState(BS_ALPHABLEND);	// αブレンド
	SetZWrite(false);	// 半透明描画はZバッファを更新しない(Zチェックは行う)

	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetPolygonSize(m_size.x, m_size.y);
	SetPolygonPos(m_pos.x, m_pos.y);
	SetPolygonFrameSize(1.0f / UI_TEXTURE_SPLIT_X,1.0f/ UI_TEXTURE_SPLIT_Y);
	SetPolygonUV((UI_FRAME % UI_TEXTURE_SPLIT_X) / (float)UI_TEXTURE_SPLIT_X,
		(UI_FRAME / UI_TEXTURE_SPLIT_X) / (float)UI_TEXTURE_SPLIT_Y);
	SetPolygonTexture(m_pTexture);
	DrawPolygon(pDC);

	//元に戻す
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);

	SetBlendState(BS_NONE);	// αブレンド無効
	SetZWrite(true);	// 半透明描画はZバッファを更新しない(Zチェックは行う)
}
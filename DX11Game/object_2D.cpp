//=============================================================================
//
// ２Dオブジェクト [object_2D.cpp]
// 小楠裕子
//2021/11/30			ファイル作成
//=============================================================================
#include "object_2D.h"
#include "Texture.h"
#include "polygon.h"

//*****定数定義*****
#define OBJECT_2D_POS_X		0.0f		//位置X
#define OBJECT_2D_POS_Y		0.0f		//位置Y
#define OBJECT_2D_SIZE_X	100.0f		//横幅
#define OBJECT_2D_SIZE_Y	100.0f		//縦幅

#define OBJECT_2D_TEXTURE_PATH		L"data/texture/field000.jpg"

//*****グローバル変数*****


//==============================================================
//ｺﾝｽﾄﾗｸﾀ
//==============================================================
Object_2D::Object_2D() 
	:m_pos(XMFLOAT2(OBJECT_2D_POS_X, OBJECT_2D_POS_Y))
	,m_size(XMFLOAT2(OBJECT_2D_SIZE_X,OBJECT_2D_SIZE_Y))
{
	//テクスチャ読み込み
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	hr = CreateTextureFromFile(pDevice, OBJECT_2D_TEXTURE_PATH, &m_pTexture);
}
//==============================================================
//ﾃﾞｽﾄﾗｸﾀ
//==============================================================
Object_2D::~Object_2D() {
	//テクスチャ解放
	SAFE_RELEASE(m_pTexture);
}
//==============================================================
//更新
//==============================================================
void Object_2D::Update() {

}
//==============================================================
//描画
//==============================================================
void Object_2D::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetPolygonSize(m_size.x,m_size.y);
	SetPolygonPos(m_pos.x,m_pos.y);
	SetPolygonTexture(m_pTexture);
	SetPolygonUV(0.0f, 0.0f);
	DrawPolygon(pDC);

	//元に戻す
}
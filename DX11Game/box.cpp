//===================================================
//		箱[box.cpp]
//小楠裕子
//=====================================================
#include "box.h"
#include "Texture.h"
#include "Shader.h"
#include "bsphere.h"
#include "sceneGame.h"
#include "map.h"

//*********************************************************
//マクロ定義
//*********************************************************
#define BOX_MODEL_PATH	"data/model/box001.x"

#define BOX_COLLISION_SIZE_X	4.0f
#define BOX_COLLISION_SIZE_Y	4.0f

#define BOY_HUND_LONG			10.0f

//*********************************************************
//グローバル変数
//*********************************************************

//=============================
//		ｺﾝｽﾄﾗｸﾀ
//=============================
Box::Box(){

	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// 位置・回転・スケールの初期設定
	for (int i = 0; i < MAX_BOX; ++i) {
		m_box[i].m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_box[i].m_oldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_box[i].m_state = true;
		m_box[i].m_use = false;
	}

	// モデルデータの読み込み
	if (!m_model.Load(pDevice, pDeviceContext, BOX_MODEL_PATH)) {
		MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitModel", MB_OK);
	}

}

//=============================
//		ﾃﾞｽﾄﾗｸﾀ
//=============================
Box::~Box() {
	// モデルの解放
	m_model.Release();
}
//=============================
//		更新
//=============================
void Box::Update() {
	XMMATRIX mtxWorld,mtxTranslate;

	for (int i = 0; i < MAX_BOX; ++i) 
	{
		//未使用ならスキップ
		if (!m_box[i].m_use) {
			continue;
		}
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();
		// 移動を反映
		mtxTranslate = XMMatrixTranslation(m_box[i].m_pos.x, m_box[i].m_pos.y, m_box[i].m_pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックス設定
		XMStoreFloat4x4(&m_box[i].m_mtxWorld, mtxWorld);


	}
	
}

//=============================
//		描画
//=============================
void Box::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	for (int i = 0; i < MAX_BOX; ++i)
	{
		//未使用ならスキップ
		if (!m_box[i].m_use) {
			continue;
		}

		// 不透明部分を描画
		m_model.Draw(pDC, m_box[i].m_mtxWorld, eOpacityOnly);

		// 半透明部分を描画
		SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
		SetZWrite(false);				// Zバッファ更新しない
		m_model.Draw(pDC, m_box[i].m_mtxWorld, eTransparentOnly);
		SetZWrite(true);				// Zバッファ更新する
		SetBlendState(BS_NONE);			// アルファブレンド無効
	}
}

//=============================
//		描画
//=============================
void Box::Draw(int num) {
	ID3D11DeviceContext* pDC = GetDeviceContext();
		//未使用なら描画しない
		if (!m_box[num].m_use) {
			return;
		}
		//破壊されていたら描画しない
		if (!m_box[num].m_state) {
			return;
		}
		// 不透明部分を描画
		m_model.Draw(pDC, m_box[num].m_mtxWorld, eOpacityOnly);

		// 半透明部分を描画
		SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
		SetZWrite(false);				// Zバッファ更新しない
		m_model.Draw(pDC, m_box[num].m_mtxWorld, eTransparentOnly);
		SetZWrite(true);				// Zバッファ更新する
		SetBlendState(BS_NONE);			// アルファブレンド無効
}

//=============================
//	箱生成 引数 : モデル座標、サイズ、ワールドマトリックス
//=============================
int Box::Create(XMFLOAT3 pos, int nCat) {
	TBox* pBox = m_box;
	for (int i = 0; i < MAX_BOX; ++i, ++pBox) {
		if (pBox->m_use) continue;
		pBox->m_pos = pos;
		pBox->m_oldPos = pos;
		pBox->m_state = true;
		pBox->m_use = true;
		pBox->m_nCat = nCat;

		return i;
	}
	return -1;
}

//=============================
//	箱解放	引数 :キューブ番号
//=============================
void Box::Release(int num) {
	if (num < 0 || num >= MAX_BOX)
		return;
	m_box[num].m_use = false;
}

//=============================
//	箱　破壊
//=============================
bool Box::Destroy(int num) {
	if (num < 0 || num >= MAX_BOX)
		return false;
	if (!m_box[num].m_nCat == CARRY)
		return false;
	m_box[num].m_state = false;
	return true;
}

//=============================
//	箱の情報　取得
//=============================
TBox* Box::GetBox()
{
	return m_box;
}

//=============================
//	箱　座標取得
//=============================
XMFLOAT3 Box::GetPos(int num) {
	return m_box[num].m_pos;
}

//=============================
//	箱　座標設定
//=============================
void Box::SetBoxPos(int num, XMFLOAT3 pos,int time) {
	XMFLOAT3 boyPos = GetOld()->GetBoyPos();
	if (!m_box[num].m_nCat == CARRY)	// 今だけNORMALにしてあります(本来ならMOVE)
		return;

	// 過去用
	if (time == 0){
		if (pos.x > 0.0f)
			m_box[num].m_pos.x = boyPos.x + BOY_HUND_LONG;
		else if (pos.x < 0.0f)
			m_box[num].m_pos.x = boyPos.x - BOY_HUND_LONG;

		if (!(boyPos.y - m_box[num].m_pos.y >= BOY_HUND_LONG || boyPos.y - m_box[num].m_pos.y <= -BOY_HUND_LONG))
			m_box[num].m_pos.y += pos.y;
		if (!(boyPos.z - m_box[num].m_pos.z >= BOY_HUND_LONG || boyPos.y - m_box[num].m_pos.z <= -BOY_HUND_LONG))
			m_box[num].m_pos.z += pos.z;
	}
	// 未来用
	if (time == 1) {
		if (pos.x > 0.0f)
			m_box[num].m_oldPos.x = boyPos.x + BOY_HUND_LONG;
		else if (pos.x < 0.0f)
			m_box[num].m_oldPos.x = boyPos.x - BOY_HUND_LONG;

		if (!(boyPos.y - m_box[num].m_oldPos.y >= BOY_HUND_LONG || boyPos.y - m_box[num].m_oldPos.y <= -BOY_HUND_LONG))
			m_box[num].m_oldPos.y += pos.y;
		if (!(boyPos.z - m_box[num].m_oldPos.z >= BOY_HUND_LONG || boyPos.y - m_box[num].m_oldPos.z <= -BOY_HUND_LONG))
			m_box[num].m_oldPos.z += pos.z;
	}
#ifndef TAKEI_HARUTO
	PrintDebugProc("ﾎｿﾞﾝｻﾞﾋｮｳx:%2f,y:%2f,z:%2f\n", m_box[num].m_pos.x, m_box[num].m_pos.y, m_box[num].m_pos.z);
	PrintDebugProc("ﾊﾝｴｲｻﾞﾋｮｳx:%2f,y:%2f,z:%2f\n", 
		m_box[num].m_oldPos.x, m_box[num].m_oldPos.y, m_box[num].m_oldPos.z);
#endif
}

//=============================
//	箱　座標反映
//=============================
void Box::SetOldBoxPos(int num) {
	if (!m_box[num].m_nCat == CARRY)	// 今だけNORMALにしてあります(本来ならMOVE)
		return;

	m_box[num].m_pos.x = m_box[num].m_oldPos.x;
	m_box[num].m_pos.y = m_box[num].m_oldPos.y;
	m_box[num].m_pos.z = m_box[num].m_oldPos.z;

}

//=============================
//	箱　サイズ取得
//=============================
XMFLOAT2 Box::GetSize() {
	return XMFLOAT2(BOX_COLLISION_SIZE_X, BOX_COLLISION_SIZE_Y);
}

//=============================
//	箱　状態取得
//=============================
bool Box::GetState(int num) {
	return m_box[num].m_state;
}

//===============================================
//		今と過去を分けれる描画(0が今、1が過去)
//===============================================
void Box::DrawOldNow(int nTime) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	for (int i = 0; i < MAX_BOX; ++i)
	{
		if (!m_box[i].m_use) {
			continue;
		}
		if (!m_box[i].m_nTime == nTime)
		{
			continue;
		}
		// 不透明部分を描画
		m_model.Draw(pDC, m_box[i].m_mtxWorld, eOpacityOnly);

		// 半透明部分を描画
		SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
		SetZWrite(false);				// Zバッファ更新しない
		m_model.Draw(pDC, m_box[i].m_mtxWorld, eTransparentOnly);
		SetZWrite(true);				// Zバッファ更新する
		SetBlendState(BS_NONE);			// アルファブレンド無効
	}
}

//=======================================
//	キューブ生成(0が今、1が過去)
//=======================================
int Box::CreateOldNow(XMFLOAT3 pos, int nTime) {
	TBox* pBox = m_box;
	for (int i = 0; i < MAX_BOX; ++i, ++pBox) {
		if (pBox->m_use) continue;
		pBox->m_pos = pos;
		pBox->m_oldPos = pos;
		pBox->m_state = true;
		pBox->m_use = true;
		pBox->m_nTime = nTime;
		return i;
	}
	return -1;
}
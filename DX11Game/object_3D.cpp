//=============================================================================
//
// 3Dオブジェクト [object_3D.cpp]
// 小楠裕子
//2021/11/30			ファイル作成
//=============================================================================
#include "object_3D.h"
#include "input.h"
#include "sceneSample.h"
#include "enemy.h"

//*****定数定義*****
#define OBJECT_3D_MODEL_PATH			"data/model/hikouki_white.x"

#define	OBJECT_3D_VALUE_MOVE	(0.50f)		// 移動速度
#define	OBJECT_3D_RATE_MOVE		(0.20f)		// 移動慣性係数
#define	OBJECT_3D_VALUE_ROTATE	(9.0f)		// 回転速度
#define	OBJECT_3D_RATE_ROTATE	(0.20f)		// 回転慣性係数

#define OBJECT_3D_COLLISION_SIZE_X		2.5f
#define OBJECT_3D_COLLISION_SIZE_Y		2.5f
#define OBJECT_3D_COLLISION_SIZE_Z		2.5f

#define OBJECT_3D_COLLISION_SIZE_RAD	2.5f


//*****グローバル変数*****


//==============================================================
//ｺﾝｽﾄﾗｸﾀ
//==============================================================
Object_3D::Object_3D()

{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// 位置・回転・スケールの初期設定
	m_pos = XMFLOAT3(0.0f, 20.0f, 0.0f);
	m_move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotDest = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// モデルデータの読み込み
	if (!m_model.Load(pDevice, pDeviceContext, OBJECT_3D_MODEL_PATH)) {
		MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitModel", MB_OK);
	}

	m_nCube = GetCube()->Create(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f),m_mtxWorld);
}
//==============================================================
//ﾃﾞｽﾄﾗｸﾀ
//==============================================================
Object_3D::~Object_3D() {
	// モデルの解放
	m_model.Release();
	//立方体解放
	GetCube()->Release(m_nCube);
}
//==============================================================
//更新
//==============================================================
void Object_3D::Update() {
	// カメラの向き取得
	XMFLOAT3 rotCamera = CCamera::Get()->GetAngle();
	if (GetKeyPress(VK_LEFT)) {
		if (GetKeyPress(VK_UP)) {
			// 左前移動
			m_move.x -= SinDeg(rotCamera.y + 135.0f) * OBJECT_3D_VALUE_MOVE;
			m_move.z -= CosDeg(rotCamera.y + 135.0f) * OBJECT_3D_VALUE_MOVE;

			m_rotDest.y = rotCamera.y + 135.0f;
		}
		else if (GetKeyPress(VK_DOWN)) {
			// 左後移動
			m_move.x -= SinDeg(rotCamera.y + 45.0f) * OBJECT_3D_VALUE_MOVE;
			m_move.z -= CosDeg(rotCamera.y + 45.0f) * OBJECT_3D_VALUE_MOVE;

			m_rotDest.y = rotCamera.y + 45.0f;
		}
		else {
			// 左移動
			m_move.x -= SinDeg(rotCamera.y + 90.0f) * OBJECT_3D_VALUE_MOVE;
			m_move.z -= CosDeg(rotCamera.y + 90.0f) * OBJECT_3D_VALUE_MOVE;

			m_rotDest.y = rotCamera.y + 90.0f;
		}
	}
	else if (GetKeyPress(VK_RIGHT)) {

		if (GetKeyPress(VK_UP)) {
			// 右前移動
			m_move.x -= SinDeg(rotCamera.y - 135.0f) * OBJECT_3D_VALUE_MOVE;
			m_move.z -= CosDeg(rotCamera.y - 135.0f) * OBJECT_3D_VALUE_MOVE;

			m_rotDest.y = rotCamera.y - 135.0f;
		}
		else if (GetKeyPress(VK_DOWN)) {
			// 右後移動
			m_move.x -= SinDeg(rotCamera.y - 45.0f) * OBJECT_3D_VALUE_MOVE;
			m_move.z -= CosDeg(rotCamera.y - 45.0f) * OBJECT_3D_VALUE_MOVE;

			m_rotDest.y = rotCamera.y - 45.0f;
		}
		else {
			// 右移動
			m_move.x -= SinDeg(rotCamera.y - 90.0f) * OBJECT_3D_VALUE_MOVE;
			m_move.z -= CosDeg(rotCamera.y - 90.0f) * OBJECT_3D_VALUE_MOVE;

			m_rotDest.y = rotCamera.y - 90.0f;
		}
	}
	else if (GetKeyPress(VK_UP)) {

		// 前移動
		m_move.x -= SinDeg(180.0f + rotCamera.y) * OBJECT_3D_VALUE_MOVE;
		m_move.z -= CosDeg(180.0f + rotCamera.y) * OBJECT_3D_VALUE_MOVE;

		m_rotDest.y = 180.0f + rotCamera.y;
	}
	else if (GetKeyPress(VK_DOWN)) {

		// 後移動
		m_move.x -= SinDeg(rotCamera.y) * OBJECT_3D_VALUE_MOVE;
		m_move.z -= CosDeg(rotCamera.y) * OBJECT_3D_VALUE_MOVE;

		m_rotDest.y = rotCamera.y;
	}

	if (GetKeyPress(VK_I)) {

		m_move.y += OBJECT_3D_VALUE_MOVE;
	}
	if (GetKeyPress(VK_K)) {

		m_move.y -= OBJECT_3D_VALUE_MOVE;
	}

	if (GetKeyPress(VK_LSHIFT)) {
		// 左回転
		m_rotDest.y -= OBJECT_3D_VALUE_ROTATE;
		if (m_rotDest.y < -180.0f) {
			m_rotDest.y += 360.0f;
		}
	}
	if (GetKeyPress(VK_RSHIFT)) {
		// 右回転
		m_rotDest.y += OBJECT_3D_VALUE_ROTATE;
		if (m_rotDest.y >= 180.0f) {
			m_rotDest.y -= 360.0f;
		}
	}

	// 目的の角度までの差分
	float fDiffRotY = m_rotDest.y - m_rot.y;
	if (fDiffRotY >= 180.0f) {
		fDiffRotY -= 360.0f;
	}
	if (fDiffRotY < -180.0f) {
		fDiffRotY += 360.0f;
	}

	// 目的の角度まで慣性をかける
	m_rot.y += fDiffRotY * OBJECT_3D_RATE_ROTATE;
	if (m_rot.y >= 180.0f) {
		m_rot.y -= 360.0f;
	}
	if (m_rot.y < -180.0f) {
		m_rot.y += 360.0f;
	}

	// 位置移動
	m_pos.x += m_move.x;
	m_pos.y += m_move.y;
	m_pos.z += m_move.z;



	// 移動量に慣性をかける
	m_move.x += (0.0f - m_move.x) * OBJECT_3D_RATE_MOVE;
	m_move.y += (0.0f - m_move.y) * OBJECT_3D_RATE_MOVE;
	m_move.z += (0.0f - m_move.z) * OBJECT_3D_RATE_MOVE;

	if (m_pos.x < -310.0f) {
		m_pos.x = -310.0f;
	}
	if (m_pos.x > 310.0f) {
		m_pos.x = 310.0f;
	}
	if (m_pos.z < -310.0f) {
		m_pos.z = -310.0f;
	}
	if (m_pos.z > 310.0f) {
		m_pos.z = 310.0f;
	}
	if (m_pos.y < 0.0f) {
		m_pos.y = 0.0f;
	}
	if (m_pos.y > 80.0f) {
		m_pos.y = 80.0f;
	}

	if (GetKeyPress(VK_RETURN)) {
		// リセット
		m_pos = XMFLOAT3(0.0f, 20.0f, 0.0f);
		m_move = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_rotDest = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	XMMATRIX mtxWorld, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rot.x),
		XMConvertToRadians(m_rot.y), XMConvertToRadians(m_rot.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックス設定
	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);

	//立方体移動
	GetCube()->Move(m_nCube,m_mtxWorld);

	//当たり判定
	CollisionEnemy(m_pos, XMFLOAT3(OBJECT_3D_COLLISION_SIZE_X, OBJECT_3D_COLLISION_SIZE_Y, OBJECT_3D_COLLISION_SIZE_Z));
}
//==============================================================
//描画
//==============================================================
void Object_3D::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// 不透明部分を描画
	m_model.Draw(pDC, m_mtxWorld, eOpacityOnly);

	// 半透明部分を描画
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない
	m_model.Draw(pDC, m_mtxWorld, eTransparentOnly);
	SetZWrite(true);				// Zバッファ更新する
	SetBlendState(BS_NONE);			// アルファブレンド無効
}
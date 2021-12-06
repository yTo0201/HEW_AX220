//========================================================
//
//ビルボード弾[bullet.cpp]
//
//========================================================
#include "bullet.h"
#include "Camera.h"
#include "shadow.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"
#include "explosion.h"
#include "bsphere.h"
#include "enemy.h"
#include "smoke.h"
//*********************************************************
//マクロ定義
//*********************************************************
#define TEXTURE_FILENAME	L"data/texture/気弾6.png"	//テクスチャファイル名
#define M_DIFFUSE			XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_BULLET			100

#define BULLET_SPEED		5.0f
#define BULLET_RADIUS		3.0f

#define MIN_FIELD_X			-320.0f
#define MAX_FIELD_X			320.0f
#define MIN_FIELD_Z			-320.0f
#define MAX_FIELD_Z			320.0f

//***********************************************************
//構造体定義
//***********************************************************
struct TBullet {
	XMFLOAT3	pos;		//位置
	XMFLOAT3	vel;		//速度
	int			nShadow;	//影ID
	int			nBSphere;	//境界球ID
	bool		use;		//使用しているかどうか
};

//*************************************************************
//グローバル変数
//*************************************************************
static MESH		g_mesh;			//メッシュ情報
static MATERIAL	g_material;		//マテリアル
static TBullet	g_bullet[MAX_BULLET];		//弾情報

//**************************************************************
//プロトタイプ宣言
//**************************************************************
static HRESULT MakeVertexBullet(ID3D11Device* pDevice);

//==============================================================
//初期化
//==============================================================
HRESULT InitBullet(void) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//頂点情報の作成
	hr = MakeVertexBullet(pDevice);
	if (FAILED(hr)) {
		return hr;
	}

	//マテリアルの設定
	g_material.Diffuse = M_DIFFUSE;
	g_material.Ambient = M_AMBIENT;
	g_material.Specular = M_SPECULAR;
	g_material.Power = M_POWER;
	g_material.Emissive = M_EMISSIVE;
	g_mesh.pMaterial = &g_material;

	//テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice, TEXTURE_FILENAME, &g_mesh.pTexture);
	if (FAILED(hr)) {
		return hr;
	}
	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	//ワールドマトリックス初期化
	XMStoreFloat4x4(&g_mesh.mtxWorld, XMMatrixIdentity());

	//弾情報初期化
	for (int i = 0; i < MAX_BULLET; ++i) {
		g_bullet[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_bullet[i].vel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_bullet[i].use = false;
		g_bullet[i].nShadow = -1;
		g_bullet[i].nBSphere = -1;
	}
	
	return hr;
}

//==============================================================
//終了処理
//==============================================================
void UninitBullet(void) {
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		if (pBullet->use) {
			ReleaseShadow(pBullet->nShadow);
			pBullet->nShadow = -1;
			ReleaseBSphere(pBullet->nBSphere);
			pBullet->nBSphere = -1;
			pBullet->use = false;
		}
	}
	ReleaseMesh(&g_mesh);
}

//==============================================================
//更新
//==============================================================
void UpdateBullet(void) {
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		//未使用ならスキップ
		if (!pBullet->use) {
			continue;
		}
		//位置を更新
		pBullet->pos.x += pBullet->vel.x;
		pBullet->pos.y += pBullet->vel.y;
		pBullet->pos.z += pBullet->vel.z;
		//範囲チェック & 当たり判定
		if (pBullet->pos.x < MIN_FIELD_X || pBullet->pos.x > MAX_FIELD_X || pBullet->pos.z < MIN_FIELD_Z || pBullet->pos.z > MAX_FIELD_Z || CollisionEnemy(pBullet->pos,BULLET_RADIUS)) {
			pBullet->use = false;
			//丸影解放
			ReleaseShadow(pBullet->nShadow);
			pBullet->nShadow = -1;
			//境界球解放
			ReleaseBSphere(pBullet->nBSphere);
			pBullet->nBSphere = -1;
			//爆発エフェクト
			//StartExplosion(pBullet->pos, XMFLOAT2(10.0f, 10.0f));
			continue;
		}
		//丸影移動
		MoveShadow(pBullet->nShadow, pBullet->pos);
		//境界球移動
		g_mesh.mtxWorld._41 = pBullet->pos.x;
		g_mesh.mtxWorld._42 = pBullet->pos.y;
		g_mesh.mtxWorld._43 = pBullet->pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		MoveBSphere(pBullet->nBSphere, g_mesh.mtxWorld);
		//煙エフェクト
		StartSmoke(pBullet->pos, XMFLOAT2(5.0f, 5.0f));
	}
}

//==============================================================
//描画
//==============================================================
void DrawBullet(void) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	CLight::Get()->SetDisable();		//光源無効
	SetBlendState(BS_ALPHABLEND);		//αブレンディング有効
	XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		//未使用ならスキップ
		if (!pBullet->use) {
			continue;
		}
		//ビュー行列の回転成分の転置行列を設定
		g_mesh.mtxWorld._11 = mView._11;
		g_mesh.mtxWorld._12 = mView._21;
		g_mesh.mtxWorld._13 = mView._31;
		g_mesh.mtxWorld._14 = 0.0f;
		g_mesh.mtxWorld._21 = mView._12;
		g_mesh.mtxWorld._22 = mView._22;
		g_mesh.mtxWorld._23 = mView._32;
		g_mesh.mtxWorld._24 = 0.0f;
		g_mesh.mtxWorld._31 = mView._13;
		g_mesh.mtxWorld._32 = mView._23;
		g_mesh.mtxWorld._33 = mView._33;
		g_mesh.mtxWorld._34 = 0.0f;
		//位置を反映
		g_mesh.mtxWorld._41 = pBullet->pos.x;
		g_mesh.mtxWorld._42 = pBullet->pos.y;
		g_mesh.mtxWorld._43 = pBullet->pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		//描画
		DrawMesh(pDC, &g_mesh);
	}
	SetBlendState(BS_NONE);		//αブレンディング無効
	CLight::Get()->SetEnable();	//光源有効
}

//==============================================================
//発射
//==============================================================
int FireBullet(XMFLOAT3 pos, XMFLOAT3 dir) {
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		//使用中ならスキップ
		if (pBullet->use) {
			continue;
		}
		pBullet->pos = pos;
		pBullet->vel.x = dir.x * BULLET_SPEED;
		pBullet->vel.y = dir.y * BULLET_SPEED;
		pBullet->vel.z = dir.z * BULLET_SPEED;
		pBullet->nShadow = CreateShadow(pos, BULLET_RADIUS);
		pBullet->nBSphere = CreateBSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), BULLET_RADIUS, g_mesh.mtxWorld);
		pBullet->use = true;
		StartSmoke(pBullet->pos, XMFLOAT2(10.0f, 10.0f));
		return i;		//発射できた(0～弾番号)
	}
	return -1;			//発射できなかった
}

//==============================================================
//頂点情報の作成
//==============================================================
HRESULT MakeVertexBullet(ID3D11Device* pDevice) {
	//一時的な頂点配列を生成
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	//頂点配列の中身を埋める
	pVertexWk[0].vtx = XMFLOAT3(-BULLET_RADIUS, BULLET_RADIUS, 0.0f);
	pVertexWk[1].vtx = XMFLOAT3(BULLET_RADIUS, BULLET_RADIUS, 0.0f);
	pVertexWk[2].vtx = XMFLOAT3(-BULLET_RADIUS, -BULLET_RADIUS, 0.0f);
	pVertexWk[3].vtx = XMFLOAT3(BULLET_RADIUS, -BULLET_RADIUS, 0.0f);
	pVertexWk[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	//一時的なインデックス配列を生成
	g_mesh.nNumIndex = 4;
	int* pIndexWk = new int[4];

	//インデックス配列の中身を埋める
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	//頂点バッファ・インデックスバッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWk, pIndexWk);

	//一時的な頂点配列・インデックス配列を解放
	delete[] pIndexWk;
	delete[] pVertexWk;
	return hr;
}
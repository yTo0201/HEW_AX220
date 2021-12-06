//========================================================
//
//煙エフェクト[smoke.cpp]
//
//========================================================
#include "smoke.h"
#include "Camera.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"
#include "input.h"

//*********************************************************
//マクロ定義
//*********************************************************
#define TEXTURE_FILENAME	L"data/texture/smoke000.png"	//テクスチャファイル名
#define M_DIFFUSE			XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_SMOKE		1000

#define SMOKE_ANIM_COUNT	60			//一コマ当たりの表示回数

//***********************************************************
//構造体定義
//***********************************************************
struct TSmoke {
	XMFLOAT3	pos;		//位置
	XMFLOAT2	size;		//サイズ
	bool		use;		//使用しているかどうか
	int			count;		//アニメーションフレーム数
};

//*************************************************************
//グローバル変数
//*************************************************************
static MESH		g_mesh;			//メッシュ情報
static MATERIAL	g_material;		//マテリアル
static TSmoke	g_smoke[MAX_SMOKE];		//煙エフェクト情報

//**************************************************************
//プロトタイプ宣言
//**************************************************************
static HRESULT MakeVertexSmoke(ID3D11Device* pDevice);

//==============================================================
//初期化
//==============================================================
HRESULT InitSmoke(void) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//頂点情報の作成
	hr = MakeVertexSmoke(pDevice);
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

	//爆発情報初期化
	for (int i = 0; i < MAX_SMOKE; ++i) {
		g_smoke[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_smoke[i].size = XMFLOAT2(10.0f,10.0f);
		g_smoke[i].use = false;
		g_smoke[i].count = SMOKE_ANIM_COUNT;
	}
	
	return hr;
}

//==============================================================
//終了処理
//==============================================================
void UninitSmoke(void) {
	ReleaseMesh(&g_mesh);
}

//==============================================================
//更新
//==============================================================
void UpdateSmoke(void) {
	TSmoke* pSmoke = g_smoke;
	for (int i = 0; i < MAX_SMOKE; ++i, ++pSmoke) {
		//未使用ならスキップ
		if (!pSmoke->use) {
			continue;
		}
		//カウンタ更新
		--pSmoke->count;
		if (pSmoke->count <= 0) {
			pSmoke->use = false;	//消滅
		}
		//サイズ更新
		pSmoke->size.x += 0.3f;
		pSmoke->size.y += 0.1f;
		//位置更新
		pSmoke->pos.y += 0.5f;
	}
}

//==============================================================
//描画
//==============================================================
void DrawSmoke(void) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	CLight::Get()->SetDisable();		//光源無効
	SetBlendState(BS_ALPHABLEND);		//αブレンド
	SetZWrite(false);
	XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
	TSmoke* pSmoke = g_smoke;
	for (int i = 0; i < MAX_SMOKE; ++i, ++pSmoke) {
		//未使用ならスキップ
		if (!pSmoke->use) {
			continue;
		}
		//テクスチャマトリックス更新
		//ビュー行列の回転成分の転置行列を設定(+拡縮)
		g_mesh.mtxWorld._11 = mView._11 * pSmoke->size.x;
		g_mesh.mtxWorld._12 = mView._21 * pSmoke->size.x;
		g_mesh.mtxWorld._13 = mView._31 * pSmoke->size.x;
		g_mesh.mtxWorld._14 = 0.0f;
		g_mesh.mtxWorld._21 = mView._12 * pSmoke->size.y;
		g_mesh.mtxWorld._22 = mView._22 * pSmoke->size.y;
		g_mesh.mtxWorld._23 = mView._32 * pSmoke->size.y;
		g_mesh.mtxWorld._24 = 0.0f;
		g_mesh.mtxWorld._31 = mView._13;
		g_mesh.mtxWorld._32 = mView._23;
		g_mesh.mtxWorld._33 = mView._33;
		g_mesh.mtxWorld._34 = 0.0f;
		//位置を反映
		g_mesh.mtxWorld._41 = pSmoke->pos.x;
		g_mesh.mtxWorld._42 = pSmoke->pos.y;
		g_mesh.mtxWorld._43 = pSmoke->pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		//不透明度設定
		g_material.Diffuse.w = 0.3f - (0.005f * (SMOKE_ANIM_COUNT - pSmoke->count));
		if (g_material.Diffuse.w < 0.0f) {
			g_material.Diffuse.w = 0.0f;
		}
		//描画
		DrawMesh(pDC, &g_mesh);
	}
	SetBlendState(BS_NONE);		//αブレンド無効
	CLight::Get()->SetEnable();	//光源有効
	SetZWrite(true);
}

//==============================================================
//開始
//==============================================================
int StartSmoke(XMFLOAT3 pos, XMFLOAT2 size) {
	TSmoke* pSmoke = g_smoke;
	for (int i = 0; i < MAX_SMOKE; ++i, ++pSmoke) {
		//使用中ならスキップ
		if (pSmoke->use) {
			continue;
		}
		pSmoke->pos = pos;
		pSmoke->size = size;
		pSmoke->count = SMOKE_ANIM_COUNT;
		pSmoke->use = true;
		return i;		//爆発した(0～爆発番号）
	}
	return -1;			//爆発しなかった
}

//==============================================================
//頂点情報の作成
//==============================================================
HRESULT MakeVertexSmoke(ID3D11Device* pDevice) {
	//一時的な頂点配列を生成
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	//頂点配列の中身を埋める
	pVertexWk[0].vtx = XMFLOAT3(-0.5f,0.5f, 0.0f);
	pVertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	pVertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	pVertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
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
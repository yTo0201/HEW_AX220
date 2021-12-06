//========================================================
//
//炎エフェクト[fire.cpp]
//
//========================================================
#include "fire.h"
#include "Camera.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"
#include "input.h"

//*********************************************************
//マクロ定義
//*********************************************************
#define TEXTURE_FILENAME	L"data/texture/effect000.jpg"	//テクスチャファイル名
#define M_DIFFUSE			XMFLOAT4(1.0f,0.2f,0.6f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_FIRE		100

#define FIRE_ANIM_COUNT	15			//一コマ当たりの表示回数

//***********************************************************
//構造体定義
//***********************************************************
struct TFire {
	XMFLOAT3	pos;		//位置
	float		size;		//サイズ
	bool		use;		//使用しているかどうか
	int			count;		//アニメーションフレーム数
	XMFLOAT4X4	world;		// ワールド マトリックス
};

//*************************************************************
//グローバル変数
//*************************************************************
static MESH		g_mesh;			//メッシュ情報
static MATERIAL	g_material;		//マテリアル
static TFire	g_fire[MAX_FIRE];		//爆発情報

//**************************************************************
//プロトタイプ宣言
//**************************************************************
static HRESULT MakeVertexFire(ID3D11Device* pDevice);

//==============================================================
//初期化
//==============================================================
HRESULT InitFire(void) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//頂点情報の作成
	hr = MakeVertexFire(pDevice);
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
	for (int i = 0; i < MAX_FIRE; ++i) {
		g_fire[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_fire[i].size = 10.0f;
		g_fire[i].use = false;
		g_fire[i].count = FIRE_ANIM_COUNT;
	}
	
	return hr;
}

//==============================================================
//終了処理
//==============================================================
void UninitFire(void) {
	ReleaseMesh(&g_mesh);
}

//==============================================================
//更新
//==============================================================
void UpdateFire(void) {
	/*if (GetKeyPress(VK_F)) {
		StartFire(XMFLOAT3(0.0f, 50.0f, 0.0f), XMFLOAT2(10.0f, 10.0f));
	}*/
	TFire* pFire = g_fire;
	for (int i = 0; i < MAX_FIRE; ++i, ++pFire) {
		//未使用ならスキップ
		if (!pFire->use) {
			continue;
		}
		//カウンタ更新
		--pFire->count;
		if (pFire->count <= 0) {
			pFire->use = false;	//消滅
		}
	}
}

//==============================================================
//描画
//==============================================================
void DrawFire(void) {
	
	ID3D11DeviceContext* pDC = GetDeviceContext();
	CLight::Get()->SetDisable();
	SetBlendState(BS_ADDITIVE);	// 加算合成
	SetZWrite(false);	// 半透明描画はZバッファを更新しない(Zチェックは行う)
	TFire* pFire = g_fire;
	for (int i = 0; i < MAX_FIRE; ++i, ++pFire) {
		if (!pFire->use) {
			continue;
		}
		// 中心座標を移動
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMVector3TransformCoord(
			XMLoadFloat3(&pFire->pos),
			XMLoadFloat4x4(&pFire->world)));
		// サイズを反映
		g_mesh.mtxWorld._11 = g_mesh.mtxWorld._22 =
			g_mesh.mtxWorld._33 = pFire->size;
		// 位置を反映
		g_mesh.mtxWorld._41 = pos.x;
		g_mesh.mtxWorld._42 = pos.y;
		g_mesh.mtxWorld._43 = pos.z;
		//不透明度設定
		g_material.Diffuse.w = pFire->count * 0.06f;
		// 描画
		DrawMesh(pDC, &g_mesh);
	}
	SetZWrite(true);
	SetBlendState(BS_NONE);
	CLight::Get()->SetEnable();
}

		

//==============================================================
//爆発開始　引数 : モデル座標、サイズ、ワールドマトリックス
//==============================================================
int StartFire(XMFLOAT3 pos, float size, XMFLOAT4X4 world) {
	TFire* pFire = g_fire;
	for (int i = 0; i < MAX_FIRE; ++i, ++pFire) {
		//使用中ならスキップ
		if (pFire->use) {
			continue;
		}
		pFire->pos = pos;
		pFire->size = size;
		pFire->count = FIRE_ANIM_COUNT;
		pFire->world = world;
		pFire->use = true;
		return i;		//爆発した(0～爆発番号）
	}
	return -1;			//爆発しなかった
}

//==============================================================
//頂点情報の作成
//==============================================================
HRESULT MakeVertexFire(ID3D11Device* pDevice) {
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
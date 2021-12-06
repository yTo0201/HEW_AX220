//=============================================================================
//
// 木処理 [tree.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "tree.h"
#include "Camera.h"
#include "shadow.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_TREE	L"data/texture/tree000.png"	// 読み込むテクスチャファイル名
#define	TREE_WIDTH		(50.0f)						// 木の幅
#define	TREE_HEIGHT		(80.0f)						// 木の高さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexTree(ID3D11Device* pDevice);
void SetVertexTree(int idxTree, float width, float height);
void SetColorTree(int idxTree, XMFLOAT4 col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MESH				g_mesh;				// メッシュ情報
static MATERIAL			g_material;			// マテリアル
static TTree			g_tree[MAX_TREE];	// 木ワーク

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTree(void)
{
	ID3D11Device* pDevice = GetDevice();

	// 頂点情報の作成
	MakeVertexTree(pDevice);

	// テクスチャの読み込み
	CreateTextureFromFile(pDevice,				// デバイスへのポインタ
						  TEXTURE_TREE,			// ファイルの名前
						  &g_mesh.pTexture);	// 読み込むメモリ
	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	g_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Power = 0.0f;
	g_mesh.pMaterial = &g_material;

	for (int cntTree = 0; cntTree < MAX_TREE; ++cntTree) {
		g_tree[cntTree].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_tree[cntTree].col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_tree[cntTree].width = TREE_WIDTH;
		g_tree[cntTree].height = TREE_HEIGHT;
		g_tree[cntTree].use = false;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTree(void)
{
	for (int cntTree = 0; cntTree < MAX_TREE; ++cntTree) {
		if (g_tree[cntTree].use) {
			ReleaseShadow(g_tree[cntTree].idxShadow);
			g_tree[cntTree].idxShadow = -1;
			g_tree[cntTree].use = false;
		}
	}
	// メッシュの開放
	ReleaseMesh(&g_mesh);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTree(void)
{
	for (int cntTree = 0; cntTree < MAX_TREE; ++cntTree) {
		// 未使用ならスキップ
		if (!g_tree[cntTree].use) {
			continue;
		}
		// 影の位置設定
		MoveShadow(g_tree[cntTree].idxShadow, XMFLOAT3(g_tree[cntTree].pos.x, 0.1f, g_tree[cntTree].pos.z));
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTree(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxScale, mtxTranslate;

	CLight::Get()->SetDisable();	// 光源無効
	SetBlendState(BS_ALPHABLEND);	// αブレンディング有効

	// ビューマトリックスを取得
	XMFLOAT4X4& mtxView = CCamera::Get()->GetViewMatrix();

	for (int cntTree = 0; cntTree < MAX_TREE; ++cntTree) {
		// 未使用ならスキップ
		if (!g_tree[cntTree].use) {
			continue;
		}
		// ワールド マトリックス初期化
		mtxWorld = XMMatrixIdentity();
		XMStoreFloat4x4(&g_mesh.mtxWorld, mtxWorld);

		// 回転を反映
		g_mesh.mtxWorld._11 = mtxView._11;
		g_mesh.mtxWorld._12 = mtxView._21;
		g_mesh.mtxWorld._13 = mtxView._31;
		g_mesh.mtxWorld._21 = mtxView._12;
		g_mesh.mtxWorld._22 = mtxView._22;
		g_mesh.mtxWorld._23 = mtxView._32;
		g_mesh.mtxWorld._31 = mtxView._13;
		g_mesh.mtxWorld._32 = mtxView._23;
		g_mesh.mtxWorld._33 = mtxView._33;
		mtxWorld = XMLoadFloat4x4(&g_mesh.mtxWorld);

		// スケールを反映 (回転より先に反映)
		mtxScale = XMMatrixScaling(g_tree[cntTree].width, g_tree[cntTree].height, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_tree[cntTree].pos.x, g_tree[cntTree].pos.y, g_tree[cntTree].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		XMStoreFloat4x4(&g_mesh.mtxWorld, mtxWorld);

		// 色の設定
		g_material.Diffuse = g_tree[cntTree].col;

		// ポリゴンの描画
		DrawMesh(pDeviceContext, &g_mesh);
	}

	SetBlendState(BS_NONE);		// αブレンディング無効
	CLight::Get()->SetEnable();	// 光源有効
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexTree(ID3D11Device* pDevice)
{
	// 一時的な頂点配列を生成
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;

	// 頂点座標の設定
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2.0f, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2.0f, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3( 1.0f / 2.0f, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3( 1.0f / 2.0f, 1.0f, 0.0f);

	// 法線の設定
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	g_mesh.nNumIndex = 4;
	int* pIndexWk = new int[g_mesh.nNumIndex];
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWk, pIndexWk);

	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexTree(int idxTree, float width, float height)
{
	if (idxTree >= 0 && idxTree < MAX_TREE) {
		g_tree[idxTree].width = width;
		g_tree[idxTree].height = height;
	}
}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorTree(int idxTree, XMFLOAT4 col)
{
	if (idxTree >= 0 && idxTree < MAX_TREE) {
		g_tree[idxTree].col = col;
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SetTree(XMFLOAT3 pos, float width, float height, XMFLOAT4 col)
{
	int idxTree = -1;

	for (int cntTree = 0; cntTree < MAX_TREE; ++cntTree) {
		// 使用中ならスキップ
		if (g_tree[cntTree].use) {
			continue;
		}
		g_tree[cntTree].use = true;
		g_tree[cntTree].pos = pos;

		// 頂点座標の設定
		SetVertexTree(cntTree, width, height);

		// 頂点カラーの設定
		SetColorTree(cntTree, col);

		// 影の設定
		g_tree[cntTree].idxShadow = CreateShadow(g_tree[cntTree].pos, g_tree[cntTree].width * 0.5f);

		idxTree = cntTree;
		break;
	}

	return idxTree;
}

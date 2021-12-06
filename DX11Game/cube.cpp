//===================================================
//		立方体[cube.cpp]
//小楠裕子
//2021/12/1	ファイル作成
//2021 / 12 / 2	Create, Move, Release関数追加
//=====================================================
#include "cube.h"
#include "Texture.h"
#include "Shader.h"

//*********************************************************
//マクロ定義
//*********************************************************
#define CUBE_TEXTURE_PATH	/*L"data/texture/explosion000.png"*/	//テクスチャファイル名
#define M_DIFFUSE			XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

//*********************************************************
//グローバル変数
//*********************************************************
static MESH		g_mesh;							//メッシュ情報

//=============================
//		ｺﾝｽﾄﾗｸﾀ
//=============================
Cube::Cube(){
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//頂点情報の作成
	hr = MakeVertex(pDevice);

	//マテリアルの設定
	m_material.Diffuse = M_DIFFUSE;
	m_material.Ambient = M_AMBIENT;
	m_material.Specular = M_SPECULAR;
	m_material.Power = M_POWER;
	m_material.Emissive = M_EMISSIVE;
	g_mesh.pMaterial = &m_material;

	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	//ワールドマトリックス初期化
	XMStoreFloat4x4(&g_mesh.mtxWorld, XMMatrixIdentity());

	for (int i = 0; i < MAX_CUBE; i++) {
		//ワールドマトリックス初期化
		XMStoreFloat4x4(&m_cube[i].m_world, XMMatrixIdentity());
		m_cube[i].m_use = false;
	}

	m_visible = true;

}

//=============================
//		ﾃﾞｽﾄﾗｸﾀ
//=============================
Cube::~Cube() {
	ReleaseMesh(&g_mesh);
}
//=============================
//		更新
//=============================
void Cube::Update() {
}

//=============================
//		描画
//=============================
void Cube::Draw() {
	if (!m_visible) return;
	ID3D11DeviceContext* pDC = GetDeviceContext();
	TCube* pCube = m_cube;
	for (int i = 0; i < MAX_CUBE; ++i,++pCube) {
		if (!pCube->m_use) {
			continue;
		}
		// 中心座標を移動
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMVector3TransformCoord(
			XMLoadFloat3(&pCube->m_pos),
			XMLoadFloat4x4(&pCube->m_world)));
		// サイズを反映
		g_mesh.mtxWorld._11 = pCube->m_size.x;
		g_mesh.mtxWorld._22 = pCube->m_size.y;
		g_mesh.mtxWorld._33 = pCube->m_size.z;
		// 位置を反映
		g_mesh.mtxWorld._41 = pos.x;
		g_mesh.mtxWorld._42 = pos.y;
		g_mesh.mtxWorld._43 = pos.z;
		// 描画
		DrawMesh(pDC, &g_mesh);
	}

}
//=============================
//	キューブ生成 引数 : モデル座標、サイズ、ワールドマトリックス
//=============================
int Cube::Create(XMFLOAT3 pos, XMFLOAT3 size, XMFLOAT4X4 world) {
	TCube* pCube = m_cube;
	for (int i = 0; i < MAX_CUBE; ++i, ++pCube) {
		if (pCube->m_use) continue;
		pCube->m_pos = pos;
		pCube->m_size = size;
		pCube->m_world = world;
		pCube->m_use = true;
		return i;
	}
	return -1;
}
//=============================
//	キューブ移動　引数 : キューブ番号、ワールドマトリックス
//=============================
void Cube::Move(int nCube, XMFLOAT4X4 world) {
	if (nCube < 0 || nCube >= MAX_CUBE)
		return;
	m_cube[nCube].m_world = world;
}
//=============================
//	キューブ解放	引数 :キューブ番号
//=============================
void Cube::Release(int nCube) {
	if (nCube < 0 || nCube >= MAX_CUBE)
		return;
	m_cube[nCube].m_use = false;
}

//=============================
//		頂点情報の作成
//=============================
HRESULT Cube::MakeVertex(ID3D11Device* pDevice) {
	//一時的な頂点配列を生成
	g_mesh.nNumVertex = 24;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	//頂点配列の中身を埋める  手前
	pVertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	pVertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, -0.5f);
	pVertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	pVertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, -0.5f);
	pVertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	//左
	pVertexWk[4].vtx = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	pVertexWk[5].vtx = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	pVertexWk[6].vtx = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	pVertexWk[7].vtx = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	pVertexWk[4].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pVertexWk[5].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pVertexWk[6].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	pVertexWk[7].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	//奥
	pVertexWk[8].vtx = XMFLOAT3(0.5f, 0.5f, 0.5f);
	pVertexWk[9].vtx = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	pVertexWk[10].vtx = XMFLOAT3(0.5f, -0.5f, 0.5f);
	pVertexWk[11].vtx = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	pVertexWk[8].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pVertexWk[9].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pVertexWk[10].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	pVertexWk[11].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//右
	pVertexWk[12].vtx = XMFLOAT3(0.5f, 0.5f, -0.5f);
	pVertexWk[13].vtx = XMFLOAT3(0.5f, 0.5f, 0.5f);
	pVertexWk[14].vtx = XMFLOAT3(0.5f, -0.5f, -0.5f);
	pVertexWk[15].vtx = XMFLOAT3(0.5f, -0.5f, 0.5f);
	pVertexWk[12].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pVertexWk[13].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pVertexWk[14].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pVertexWk[15].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	//上
	pVertexWk[16].vtx = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	pVertexWk[17].vtx = XMFLOAT3(0.5f, 0.5f, 0.5f);
	pVertexWk[18].vtx = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	pVertexWk[19].vtx = XMFLOAT3(0.5f, 0.5f, -0.5f);
	pVertexWk[16].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pVertexWk[17].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pVertexWk[18].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pVertexWk[19].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//下
	pVertexWk[20].vtx = XMFLOAT3(0.5f, -0.5f, 0.5f);
	pVertexWk[21].vtx = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	pVertexWk[22].vtx = XMFLOAT3(0.5f, -0.5f, -0.5f);
	pVertexWk[23].vtx = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	pVertexWk[20].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	pVertexWk[21].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	pVertexWk[22].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	pVertexWk[23].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);

	for (int i = 0; i < g_mesh.nNumVertex; ++i) {
		pVertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		switch (i % 4) {
		case 0:
			pVertexWk[i].tex = XMFLOAT2(0.0f, 0.0f);
			break;
		case 1:
			pVertexWk[i].tex = XMFLOAT2(1.0f, 0.0f);
			break;
		case 2:
			pVertexWk[i].tex = XMFLOAT2(0.0f, 1.0f);
			break;
		case 3: 
			pVertexWk[i].tex = XMFLOAT2(1.0f, 1.0f);
			break;
		}
	}
	//一時的なインデックス配列を生成
	g_mesh.nNumIndex = 24;
	int* pIndexWk = new int[24];

	//インデックス配列の中身を埋める
	for (int i = 0; i < g_mesh.nNumIndex; ++i) {
		pIndexWk[i] = i;
	}

	//頂点バッファ・インデックスバッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWk, pIndexWk);

	//一時的な頂点配列・インデックス配列を解放
	delete[] pIndexWk;
	delete[] pVertexWk;
	return hr;
}
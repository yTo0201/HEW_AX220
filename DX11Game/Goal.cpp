//=============================================================================
//
//  ゴール[Goal.cpp]
// 武井遥都
//=============================================================================
#include "Goal.h"
#include "Camera.h"
#include "shadow.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"
#include "playerGirl.h"

//*********************************************************
//マクロ定義
//*********************************************************
#define TEXTURE_FILENAME	L"data/texture/goal.png"	//テクスチャファイル名
#define M_DIFFUSE			XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
//*********************************************************
//グローバル変数
//*********************************************************
static MESH		g_mesh;			//メッシュ情報
static MATERIAL	g_material;		//マテリアル
static XMFLOAT3 g_pos;
static bool g_use;

//**************************************************************
//プロトタイプ宣言
//**************************************************************
//static HRESULT MakeVertexBullet(ID3D11Device* pDevice);

HRESULT InitGoal()
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//頂点情報の作成
	//hr = MakeVertexBullet(pDevice);
	//if (FAILED(hr)) {
	//	return hr;
	//}

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

	g_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_use = false;
}

void UninitGoal()
{
	ReleaseMesh(&g_mesh);
	g_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_use = false;
}

void UpdateGoal()
{
	Player_Girl m_girl;
	XMFLOAT3 m_pos = m_girl.GetPos();
	if (m_pos.x > 640)
	{
		g_use = true;
	}
}

void DrawGoal()
{
	if (g_use == true)
	{
		ID3D11DeviceContext* pDC = GetDeviceContext();

		CLight::Get()->SetDisable();		//光源無効
		SetBlendState(BS_ALPHABLEND);		//αブレンディング有効
		XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
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
		g_mesh.mtxWorld._41 = g_pos.x;
		g_mesh.mtxWorld._42 = g_pos.y;
		g_mesh.mtxWorld._43 = g_pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		//描画
		DrawMesh(pDC, &g_mesh);
		SetBlendState(BS_NONE);		//αブレンディング無効
		CLight::Get()->SetEnable();	//光源有効
	}
}
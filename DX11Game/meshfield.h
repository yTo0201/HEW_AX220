//=============================================================================
//
// メッシュ地面処理 [meshfield.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"
#include "mesh.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//HRESULT InitMeshField(int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ, float fTexSizeX = 1.0f, float fTexSizeZ = 1.0f);
//void UninitMeshField(void);
//void UpdateMeshField(void);
//void DrawMeshField(void);

class MeshField {
public:
	MeshField(int nNumBlockX, int nNumBlockZ,
		float fSizeBlockX, float fSizeBlockZ, float fTexSizeX, float fTexSizeZ,XMFLOAT3 pos);
	~MeshField();
	void Update();
	void Draw();

	HRESULT MakeVertex(ID3D11Device* pDevice,
		int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
		float fTexSizeX, float fTexSizeZ);
private:
	MATERIAL						m_material;				// マテリアル

};

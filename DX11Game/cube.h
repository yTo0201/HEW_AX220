//===================================================
//		立方体[cube.h]
//小楠裕子
//2021/12/1	ファイル作成
//2021/12/2	Create,Move,Release関数追加
//=====================================================
#pragma once
#include "main.h"
#include "mesh.h"



//*****************************************************************************
// 構造体
//*****************************************************************************
struct TCube {
	bool		m_use;		// 使用しているかどうか
	XMFLOAT3	m_pos;		// 中心座標(モデル座標系)
	XMFLOAT4X4	m_world;	// ワールド マトリックス
	XMFLOAT3	m_size;		//サイズ
	XMFLOAT3	m_rot;		//回転
};

//*****マクロ定義*****
#define MAX_CUBE		(100)

class Cube {
public:
	Cube();
	~Cube();
	void Update();
	void Draw();

	int Create(XMFLOAT3 pos, XMFLOAT3 size, XMFLOAT4X4 world);
	void Move(int nCube, XMFLOAT4X4 world);
	void Release(int nCube);

	HRESULT MakeVertex(ID3D11Device* pDevice);
private:
	MATERIAL		m_material;			// マテリアル
	TCube			m_cube[MAX_CUBE];	//キューブの情報
	bool			m_visible;			// 表示ON/OFF
};

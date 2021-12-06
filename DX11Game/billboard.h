//=============================================================================
//
// ビルボード [billboard.h]
// 小楠裕子
//2021/12/1			ファイル作成
//=============================================================================
#pragma once
#include "main.h"
#include "mesh.h"

//***********************************************************
//構造体定義
//***********************************************************
struct TBillboard {
	XMFLOAT3	m_pos;			//位置
	XMFLOAT2	m_size;			//サイズ
	bool		m_use;			//使用しているかどうか
	int			m_anim;			//アニメーション再生枠
	int			m_count;		//アニメーションフレーム数
};

//*****定数定義*****
#define MAX_BILLBOARD		100

class Billboard {
public:
	Billboard();
	~Billboard();
	void Update();
	void Draw();
	int Start(XMFLOAT3 pos, XMFLOAT2 size);

	HRESULT MakeVertex(ID3D11Device* pDevice);
private:
	TBillboard	m_billboard[MAX_BILLBOARD];		//ビルボードの情報
	MATERIAL	m_material;						//マテリアル
};
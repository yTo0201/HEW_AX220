//=============================================================================
//
// 3Dオブジェクト [object_3D.h]
// 小楠裕子
//2021/11/30			ファイル作成
//=============================================================================
#pragma once
#include "main.h"
#include "AssimpModel.h"

class Object_3D {
public:
	Object_3D();
	~Object_3D();
	void Update();
	void Draw();
private:
	CAssimpModel	m_model;	//モデル
	XMFLOAT3		m_pos;		// 現在の位置
	XMFLOAT3		m_rot;		// 現在の向き
	XMFLOAT3		m_rotDest;	// 目的の向き
	XMFLOAT3		m_move;		// 移動量

	XMFLOAT4X4		m_mtxWorld;		// ワールドマトリックス

	int				m_nCube;		//立方体番号
};

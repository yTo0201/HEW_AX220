//=============================================================================
//
// ２Dオブジェクト [object_2D.h]
// 小楠裕子
//2021/11/30			ファイル作成
//=============================================================================
#pragma once
#include "main.h"


class Object_2D {
public:
	Object_2D();
	~Object_2D();
	void Update();
	void Draw();
private:
	XMFLOAT2	m_pos;
	XMFLOAT2	m_size;
	ID3D11ShaderResourceView* m_pTexture;
};
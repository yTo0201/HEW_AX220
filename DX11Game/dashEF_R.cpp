//========================================================
//
//���j����[explosion.cpp]
//
//========================================================
#include "dashEF_R.h"
#include "Camera.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"

//*********************************************************
//�}�N����`
//*********************************************************
#define TEXTURE_FILENAME	L"data/texture/R_Dash.png"	//�e�N�X�`���t�@�C����
#define M_DIFFUSE			XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_EXPLOSION		100

#define EXPLOSION_FRAME_X	4			//���t���[����
#define EXPLOSION_FRAME_Y	5			//�c�t���[����
#define EXPLOSION_ANIM_COUNT	4			//��R�}������̕\����

//***********************************************************
//�\���̒�`
//***********************************************************
struct TExplosion {
	XMFLOAT3	pos;		//�ʒu
	XMFLOAT2	size;		//�T�C�Y
	bool		use;		//�g�p���Ă��邩�ǂ���
	int			anim;		//�A�j���[�V�����Đ��ʒu
	int			count;		//�A�j���[�V�����t���[����
};

//*************************************************************
//�O���[�o���ϐ�
//*************************************************************
static MESH		g_mesh;			//���b�V�����
static MATERIAL	g_material;		//�}�e���A��
static TExplosion	g_explosion[MAX_EXPLOSION];		//�������

//**************************************************************
//�v���g�^�C�v�錾
//**************************************************************
static HRESULT MakeVertexExplosion(ID3D11Device* pDevice);

//==============================================================
//������
//==============================================================
HRESULT InitExplosion(void) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//���_���̍쐬
	hr = MakeVertexExplosion(pDevice);
	if (FAILED(hr)) {
		return hr;
	}

	//�}�e���A���̐ݒ�
	g_material.Diffuse = M_DIFFUSE;
	g_material.Ambient = M_AMBIENT;
	g_material.Specular = M_SPECULAR;
	g_material.Power = M_POWER;
	g_material.Emissive = M_EMISSIVE;
	g_mesh.pMaterial = &g_material;

	//�e�N�X�`���̓ǂݍ���
	hr = CreateTextureFromFile(pDevice, TEXTURE_FILENAME, &g_mesh.pTexture);
	if (FAILED(hr)) {
		return hr;
	}
	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	//���[���h�}�g���b�N�X������
	XMStoreFloat4x4(&g_mesh.mtxWorld, XMMatrixIdentity());

	//������񏉊���
	for (int i = 0; i < MAX_EXPLOSION; ++i) {
		g_explosion[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_explosion[i].size = XMFLOAT2(10.0f,10.0f);
		g_explosion[i].use = false;
		g_explosion[i].anim = 0;
		g_explosion[i].count = EXPLOSION_ANIM_COUNT;
	}
	
	return hr;
}

//==============================================================
//�I������
//==============================================================
void UninitExplosion(void) {
	ReleaseMesh(&g_mesh);
}

//==============================================================
//�X�V
//==============================================================
void UpdateExplosion(void) {
	TExplosion* pExplosion = g_explosion;
	for (int i = 0; i < MAX_EXPLOSION; ++i, ++pExplosion) {
		//���g�p�Ȃ�X�L�b�v
		if (!pExplosion->use) {
			continue;
		}
		//�J�E���^�X�V
		--pExplosion->count;
		if (pExplosion->count <= 0) {
			//�A�j���[�V�����X�V
			++pExplosion->anim;
			if (pExplosion->anim >= EXPLOSION_FRAME_X * EXPLOSION_FRAME_Y) {
				pExplosion->use = false;	//����
				continue;
			}
			//�J�E���^������
			pExplosion->count = EXPLOSION_ANIM_COUNT;
		}
	}
}

//==============================================================
//�`��
//==============================================================
void DrawExplosion(void) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	CLight::Get()->SetDisable();		//��������
	SetBlendState(BS_ALPHABLEND);		//���u�����f�B���O�L��
	XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
	TExplosion* pExplosion = g_explosion;
	for (int i = 0; i < MAX_EXPLOSION; ++i, ++pExplosion) {
		//���g�p�Ȃ�X�L�b�v
		if (!pExplosion->use) {
			continue;
		}
		//�e�N�X�`���}�g���b�N�X�X�V
		XMMATRIX mtxTex = XMMatrixScaling(1.0f / EXPLOSION_FRAME_X, 1.0f / EXPLOSION_FRAME_Y, 1.0f);
		int u = pExplosion->anim % EXPLOSION_FRAME_X;
		int v = pExplosion->anim / EXPLOSION_FRAME_X;
		mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation((float)u / EXPLOSION_FRAME_X, (float)v / EXPLOSION_FRAME_Y, 0.0f));
		XMStoreFloat4x4(&g_mesh.mtxTexture, mtxTex);
		//�r���[�s��̉�]�����̓]�u�s���ݒ�(+�g�k)
		g_mesh.mtxWorld._11 = mView._11 * pExplosion->size.x;
		g_mesh.mtxWorld._12 = mView._21 * pExplosion->size.x;
		g_mesh.mtxWorld._13 = mView._31 * pExplosion->size.x;
		g_mesh.mtxWorld._14 = 0.0f;
		g_mesh.mtxWorld._21 = mView._12 * pExplosion->size.y;
		g_mesh.mtxWorld._22 = mView._22 * pExplosion->size.y;
		g_mesh.mtxWorld._23 = mView._32 * pExplosion->size.y;
		g_mesh.mtxWorld._24 = 0.0f;
		g_mesh.mtxWorld._31 = mView._13;
		g_mesh.mtxWorld._32 = mView._23;
		g_mesh.mtxWorld._33 = mView._33;
		g_mesh.mtxWorld._34 = 0.0f;
		//�ʒu�𔽉f
		g_mesh.mtxWorld._41 = pExplosion->pos.x;
		g_mesh.mtxWorld._42 = pExplosion->pos.y;
		g_mesh.mtxWorld._43 = pExplosion->pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		//�`��
		DrawMesh(pDC, &g_mesh);
	}
	SetBlendState(BS_NONE);		//���u�����f�B���O����
	CLight::Get()->SetEnable();	//�����L��
}

//==============================================================
//�����J�n
//==============================================================
int StartExplosion(XMFLOAT3 pos, XMFLOAT2 size) {
	TExplosion* pExplosion = g_explosion;
	for (int i = 0; i < MAX_EXPLOSION; ++i, ++pExplosion) {
		//�g�p���Ȃ�X�L�b�v
		if (pExplosion->use) {
			continue;
		}
		pExplosion->pos = pos;
		pExplosion->size = size;
		pExplosion->anim = 0;
		pExplosion->count = EXPLOSION_ANIM_COUNT;
		pExplosion->use = true;
		return i;		//��������(0�`�����ԍ��j
	}
	return -1;			//�������Ȃ�����
}

//==============================================================
//���_���̍쐬
//==============================================================
HRESULT MakeVertexExplosion(ID3D11Device* pDevice) {
	//�ꎞ�I�Ȓ��_�z��𐶐�
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	//���_�z��̒��g�𖄂߂�
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

	//�ꎞ�I�ȃC���f�b�N�X�z��𐶐�
	g_mesh.nNumIndex = 4;
	int* pIndexWk = new int[4];

	//�C���f�b�N�X�z��̒��g�𖄂߂�
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	//���_�o�b�t�@�E�C���f�b�N�X�o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWk, pIndexWk);

	//�ꎞ�I�Ȓ��_�z��E�C���f�b�N�X�z������
	delete[] pIndexWk;
	delete[] pVertexWk;
	return hr;
}
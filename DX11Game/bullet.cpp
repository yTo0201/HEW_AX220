//========================================================
//
//�r���{�[�h�e[bullet.cpp]
//
//========================================================
#include "bullet.h"
#include "Camera.h"
#include "shadow.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"
#include "dashEF_R.h"
#include "bsphere.h"
#include "enemy.h"
#include "smoke.h"
//*********************************************************
//�}�N����`
//*********************************************************
#define TEXTURE_FILENAME	L"data/texture/�C�e6.png"	//�e�N�X�`���t�@�C����
#define M_DIFFUSE			XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER				(1.0f)
#define M_AMBIENT			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE			XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_BULLET			100

#define BULLET_SPEED		5.0f
#define BULLET_RADIUS		3.0f

#define MIN_FIELD_X			-320.0f
#define MAX_FIELD_X			320.0f
#define MIN_FIELD_Z			-320.0f
#define MAX_FIELD_Z			320.0f

//***********************************************************
//�\���̒�`
//***********************************************************
struct TBullet {
	XMFLOAT3	pos;		//�ʒu
	XMFLOAT3	vel;		//���x
	int			nShadow;	//�eID
	int			nBSphere;	//���E��ID
	bool		use;		//�g�p���Ă��邩�ǂ���
};

//*************************************************************
//�O���[�o���ϐ�
//*************************************************************
static MESH		g_mesh;			//���b�V�����
static MATERIAL	g_material;		//�}�e���A��
static TBullet	g_bullet[MAX_BULLET];		//�e���

//**************************************************************
//�v���g�^�C�v�錾
//**************************************************************
static HRESULT MakeVertexBullet(ID3D11Device* pDevice);

//==============================================================
//������
//==============================================================
HRESULT InitBullet(void) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	//���_���̍쐬
	hr = MakeVertexBullet(pDevice);
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

	//�e��񏉊���
	for (int i = 0; i < MAX_BULLET; ++i) {
		g_bullet[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_bullet[i].vel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_bullet[i].use = false;
		g_bullet[i].nShadow = -1;
		g_bullet[i].nBSphere = -1;
	}
	
	return hr;
}

//==============================================================
//�I������
//==============================================================
void UninitBullet(void) {
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		if (pBullet->use) {
			ReleaseShadow(pBullet->nShadow);
			pBullet->nShadow = -1;
			ReleaseBSphere(pBullet->nBSphere);
			pBullet->nBSphere = -1;
			pBullet->use = false;
		}
	}
	ReleaseMesh(&g_mesh);
}

//==============================================================
//�X�V
//==============================================================
void UpdateBullet(void) {
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		//���g�p�Ȃ�X�L�b�v
		if (!pBullet->use) {
			continue;
		}
		//�ʒu���X�V
		pBullet->pos.x += pBullet->vel.x;
		pBullet->pos.y += pBullet->vel.y;
		pBullet->pos.z += pBullet->vel.z;
		//�͈̓`�F�b�N & �����蔻��
		if (pBullet->pos.x < MIN_FIELD_X || pBullet->pos.x > MAX_FIELD_X || pBullet->pos.z < MIN_FIELD_Z || pBullet->pos.z > MAX_FIELD_Z || CollisionEnemy(pBullet->pos,BULLET_RADIUS)) {
			pBullet->use = false;
			//�ۉe���
			ReleaseShadow(pBullet->nShadow);
			pBullet->nShadow = -1;
			//���E�����
			ReleaseBSphere(pBullet->nBSphere);
			pBullet->nBSphere = -1;
			//�����G�t�F�N�g
			//StartExplosion(pBullet->pos, XMFLOAT2(10.0f, 10.0f));
			continue;
		}
		//�ۉe�ړ�
		MoveShadow(pBullet->nShadow, pBullet->pos);
		//���E���ړ�
		g_mesh.mtxWorld._41 = pBullet->pos.x;
		g_mesh.mtxWorld._42 = pBullet->pos.y;
		g_mesh.mtxWorld._43 = pBullet->pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		MoveBSphere(pBullet->nBSphere, g_mesh.mtxWorld);
		//���G�t�F�N�g
		StartSmoke(pBullet->pos, XMFLOAT2(5.0f, 5.0f));
	}
}

//==============================================================
//�`��
//==============================================================
void DrawBullet(void) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	CLight::Get()->SetDisable();		//��������
	SetBlendState(BS_ALPHABLEND);		//���u�����f�B���O�L��
	XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		//���g�p�Ȃ�X�L�b�v
		if (!pBullet->use) {
			continue;
		}
		//�r���[�s��̉�]�����̓]�u�s���ݒ�
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
		//�ʒu�𔽉f
		g_mesh.mtxWorld._41 = pBullet->pos.x;
		g_mesh.mtxWorld._42 = pBullet->pos.y;
		g_mesh.mtxWorld._43 = pBullet->pos.z;
		g_mesh.mtxWorld._44 = 1.0f;
		//�`��
		DrawMesh(pDC, &g_mesh);
	}
	SetBlendState(BS_NONE);		//���u�����f�B���O����
	CLight::Get()->SetEnable();	//�����L��
}

//==============================================================
//����
//==============================================================
int FireBullet(XMFLOAT3 pos, XMFLOAT3 dir) {
	TBullet* pBullet = g_bullet;
	for (int i = 0; i < MAX_BULLET; ++i, ++pBullet) {
		//�g�p���Ȃ�X�L�b�v
		if (pBullet->use) {
			continue;
		}
		pBullet->pos = pos;
		pBullet->vel.x = dir.x * BULLET_SPEED;
		pBullet->vel.y = dir.y * BULLET_SPEED;
		pBullet->vel.z = dir.z * BULLET_SPEED;
		pBullet->nShadow = CreateShadow(pos, BULLET_RADIUS);
		pBullet->nBSphere = CreateBSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), BULLET_RADIUS, g_mesh.mtxWorld);
		pBullet->use = true;
		StartSmoke(pBullet->pos, XMFLOAT2(10.0f, 10.0f));
		return i;		//���˂ł���(0�`�e�ԍ�)
	}
	return -1;			//���˂ł��Ȃ�����
}

//==============================================================
//���_���̍쐬
//==============================================================
HRESULT MakeVertexBullet(ID3D11Device* pDevice) {
	//�ꎞ�I�Ȓ��_�z��𐶐�
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	//���_�z��̒��g�𖄂߂�
	pVertexWk[0].vtx = XMFLOAT3(-BULLET_RADIUS, BULLET_RADIUS, 0.0f);
	pVertexWk[1].vtx = XMFLOAT3(BULLET_RADIUS, BULLET_RADIUS, 0.0f);
	pVertexWk[2].vtx = XMFLOAT3(-BULLET_RADIUS, -BULLET_RADIUS, 0.0f);
	pVertexWk[3].vtx = XMFLOAT3(BULLET_RADIUS, -BULLET_RADIUS, 0.0f);
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
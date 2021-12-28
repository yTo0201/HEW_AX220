//===================================================
//		�T���v���V�[��[sceneSample.cpp]
//����T�q
//2021/11/30	�t�@�C���쐬
//=====================================================
#include "sceneSample.h"
#include "fade.h"
#include "meshfield.h"
#include "model.h"
#include "shadow.h"
#include "bg.h"
#include "enemy.h"
#include "tree.h"
#include "bullet.h"
#include "dashEF_R.h"
#include "bsphere.h"
#include "fire.h"
#include "smoke.h"
#include "ui.h"
#include "object_3D.h"
#include "cube.h"
#include "Sound.h"
#include "dashEF_L.h"

//*****�O���[�o���ϐ�*****
static BG* g_pBG;		//�w�i
static UI* g_pUI;		//UI
static Object_3D* g_pObject3D;
static Cube*		g_pCube;
static MeshField* g_pMeshField;		//̨����

//=============================
//		������
//=============================
HRESULT InitSceneSample() {
	HRESULT hr = MB_OK;

	//�����̏�����
	g_pCube = new Cube;

	// ���E��������
	hr = InitBSphere();
	if (FAILED(hr))
		return hr;

	// �ۉe������
	hr = InitShadow();
	if (FAILED(hr))
		return hr;

	// ���f���\��������
	g_pObject3D = new Object_3D;

	// �G�@������
	hr = InitEnemy();
	if (FAILED(hr))
		return hr;

	// �t�B�[���h������
	/*hr = InitMeshField(8, 8, 80.0f, 80.0f);
	if (FAILED(hr))
		return hr;*/
	g_pMeshField = new MeshField(8, 8, 80.0f, 80.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 0.0f, 0.0f));

	// �w�i������
	g_pBG = new BG;

	//UI������
	g_pUI = new UI;

	// �؂̏�����
	InitTree();
	for (int nCntTree = 0; nCntTree < 10; ++nCntTree) {
		float fPosX = (rand() % 6200) / 10.0f - 310.0f;
		float fPosY = 0.0f;
		float fPosZ = (rand() % 6200) / 10.0f - 310.0f;
		SetTree(XMFLOAT3(fPosX, fPosY, fPosZ), 30.0f, 40.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//�r���{�[�h�e������
	hr = InitBullet();
	if (FAILED(hr)) {
		return hr;
	}

	//��������������
	hr = InitExplosion();
	if (FAILED(hr)) {
		return hr;
	}

	//�G�t�F�N�g������
	hr = InitDashEF();
	if (FAILED(hr)) {
		return hr;
	}


	//���G�t�F�N�g������
	hr = InitFire();
	if (FAILED(hr)) {
		return hr;
	}

	//���G�t�F�N�g������
	hr = InitSmoke();
	if (FAILED(hr)) {
		return hr;
	}

	CSound::Init();
	CSound::Play(BGM_000);
	return hr;
}

//=============================
//		�I��
//=============================
void UninitSceneSample() {
	CSound::Stop(BGM_000);
	CSound::Fin();
	//�����̏I��
	delete g_pCube;
	//���G�t�F�N�g�I������
	UninitSmoke();

	//���G�t�F�N�g�I������
	UninitFire();

	//�G�t�F�N�g�I������
	UninitDashEF();

	//���������I������
	UninitExplosion();

	//�r���{�[�h�e�I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// �w�i�I������
	delete g_pBG;

	//UI�I������
	delete g_pUI;

	// �t�B�[���h�I������
	//UninitMeshField();
	delete g_pMeshField;

	// �G�@�I������
	UninitEnemy();

	// ���f���\���I������
	delete g_pObject3D;

	// �ۉe�I������
	UninitShadow();

	// ���E���I������
	UninitBSphere();
}

//=============================
//		�X�V
//=============================
void UpdateSceneSample() {
	// ���f���X�V
	g_pObject3D->Update();

	//�r���{�[�h�e�X�V
	UpdateBullet();

	// �G�@�X�V
	UpdateEnemy();

	// �w�i�X�V
	g_pBG->Update();

	//UI�X�V
	g_pUI->Update();

	// �t�B�[���h�X�V
	//UpdateMeshField();
	g_pMeshField->Update();

	// �؂̍X�V
	UpdateTree();

	// �ۉe�X�V
	UpdateShadow();

	// ���E���X�V
	UpdateBSphere();

	//���������X�V
	UpdateExplosion();

	//�G�t�F�N�g�X�V
	UpdateDashEF();

	//���G�t�F�N�g�X�V
	UpdateFire();

	//���G�t�F�N�g�X�V
	UpdateSmoke();

	//�����̍X�V
	g_pCube->Update();

	CSound::Update();
}

//=============================
//		�`��
//=============================
void DrawSceneSample() {
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);

	// �w�i�`��
	g_pBG->Draw();


	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);

	// �t�B�[���h�`��
	//DrawMeshField();
	g_pMeshField->Draw();

	//�����̕`��
	g_pCube->Draw();

	// ���f���`��
	g_pObject3D->Draw();

	// �G�@�`��
	DrawEnemy();

	// �ۉe�`��
	DrawShadow();

	//�r���{�[�h�e�`��
	DrawBullet();

	// �؂̕`��
	DrawTree();

	// ���E���`��
	DrawBSphere();

	//���������`��
	DrawExplosion();

	//�G�t�F�N�g�`��
	DrawDashEF();

	//���G�t�F�N�g�`��
	DrawFire();

	//���G�t�F�N�g�`��
	DrawSmoke();

	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);

	//UI�`��
	g_pUI->Draw();
}

Cube* GetCube() {
	return g_pCube;
}
// player.h: interface for the player class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYER_H__0A695BD2_78E4_414C_94CC_CE61B9DD5A78__INCLUDED_)
#define AFX_PLAYER_H__0A695BD2_78E4_414C_94CC_CE61B9DD5A78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "model.h"
#include "level.h"
#include "input.h"

class player
{
friend class level;
struct PACMAN
{
	tModel*		mdl[18];		//������ �������
	int			pX;				//��������� �� ������ �� �
	int			pY;				//��������� �� ������ �� Y
	int			tX;				//������ �� ������� ������������� �� X
	int			tY;				//������ �� ������� ������������� �� Y
	int			iDir;			//����������� ��������
	D3DXVECTOR3	vPos;			//��������� ������ � ������������
	D3DXMATRIX	mOrg;			//������� ����������
	DWORD		Speed;			//����� ��� ����������� ������� � ������ �� ������ (��)
	DWORD		mTime;			//time to reach next tile
	bool		Stand;			//Standing or moving
};
struct PACANIM
{
	DWORD		frDelta;
	int			curFrame;
	bool		bAnim;
};

struct LVLPROP
{
	float stX;
	float stY;
	int objPicked;
};
	PACMAN	pacman;
	PACANIM	anim;
	LVLPROP	lvProp;

public:

	player();
	virtual ~player();

	bool player::LoadObjects(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice);
	bool player::Init(level* pLvl);
	void player::Update(DWORD delta,level* pLvl,input* pInput);
	void player::Release(Cmodel* pMdl);

	void StartMove(int iDir,level* pLvl);
	void UpdateMove(DWORD delta,level* pLvl);
	bool CanMove(int iDir,level* pLvl);

	void StopAnim();
	void StartAnim();
};

#endif // !defined(AFX_PLAYER_H__0A695BD2_78E4_414C_94CC_CE61B9DD5A78__INCLUDED_)

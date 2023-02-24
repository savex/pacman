// monster.h: interface for the monster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTER_H__64325987_EE55_486D_A621_3B89481A1326__INCLUDED_)
#define AFX_MONSTER_H__64325987_EE55_486D_A621_3B89481A1326__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "level.h"
#include "model.h"

#define	MAXPATH	128

class monster
{
friend class level;
struct ANIMS
{
	tModel*		sMove[18];
	tModel*		mMove[18];
	tModel*		bMove[6];
	tModel*		sDead[18];
	tModel*		mDead;
	tModel*		bDead[6];
};

struct MONANIM
{
	DWORD		frDelta;
	DWORD		curFrame;
	bool		bAnim;
	DWORD		frMove;
	DWORD		frDead;
	DWORD		dMove;
	DWORD		dDead;
};

/*
	������� ������:
	 ������		- ������ ����� � ������ �� �����
	 �������	- �������� �������, ���� �� ����� ����� ��� � ���������� ���� ��� �������
	 �������	- ������ ����� ��� ������ � ���� � ����
*/
struct MONSTER
{
	int			Type;			//��� ������� (������,�������,�������)
	int			nDir;			//��������� ����������� �������� ��� 
								//�������� �������, ����� �� ������� �������
	tModel*		curModel;		//��������� �� ������� ������ � ��������
	int			pX;				//��������� �� ������ �� �
	int			pY;				//��������� �� ������ �� Y
	int			tX;				//������ �� ������� ������������� �� X
	int			tY;				//������ �� ������� ������������� �� Y
	int			iDir;			//����������� ��������
	D3DXVECTOR3	vPos;			//��������� ������ � ������������
	D3DXMATRIX	mOrg;			//������� ����������
	DWORD		Speed;			//����� ��� ����������� ������� � ������ �� ������ (��)
	DWORD		mTime;			//time to reach next tile
	bool		bStand;			//����� �� ����� ��� ���
	bool		bSight;			//������ ��� ����� ��� ���

	MONANIM		aniPr;
	bool		bDead;
};

struct LVLPROP
{
	float stX;
	float stY;
};

	ANIMS	anims;
	LVLPROP	lvProp;

	char Path[MAXPATH];
	char Map[16][16];

public:
	enum {SMALL,MEDIUM,BIG};
	MONSTER	monsters[12];
	int		iTotal;			//total monsters on level
	char PathLen;

	monster();
	virtual ~monster();

	bool LoadMonsters(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice);
	void UpdateMonsters(DWORD delta,level* pLvl);
	void DrawMonsters(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice);
	void Release(Cmodel* pMdl);

	void AddMonster(int Type,int pX,int pY,level* pLvl);
	void Clear();

	int GetRandomDir(int iDir,int x,int y,level* pLvl);
	int LookFor(int x,int y,level* pLvl);
	void FillMap(level* pLvl);
	void PutNumbers(char N,int x,int y);
	void FindPath(int x,int y,level* pLvl);
	bool CanMove(int iDir,int mon,level* pLvl);
};

#endif // !defined(AFX_MONSTER_H__64325987_EE55_486D_A621_3B89481A1326__INCLUDED_)

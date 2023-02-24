// level.h: interface for the level class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVEL_H__077847D1_5885_4723_8391_E8AE95894A7B__INCLUDED_)
#define AFX_LEVEL_H__077847D1_5885_4723_8391_E8AE95894A7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "model.h"
#include "monster.h"
#include "psystem.h"
#include <fstream>

#define TLSIZE	10
#define	HTLSIZE	TLSIZE/2

#define MAPX	250
#define MAPY	50

#define MAXPS	12

class level  
{
friend class player;
friend class monster;
struct OBJ
{
	tModel*		mdl;			//������ �����
	char		type;			//��� �����
};

struct PSYSTEMS
{
	psystem*	pPS[MAXPS];
	D3DXVECTOR2	vPos[MAXPS];
	int			iNumPS;
};

struct _PACMAN
{
	tModel*		mdl;			//������ �������
	D3DXMATRIX	mOrg;			//������� ���������� � ���������
	int			x;				//Pacman X
	int			y;				//Pacman Y
};

//������ ����, ����������� �� ���������� ���� lenX*lenY
struct TILE
{
	char	objType;			//��� ������� 0-9
	char	tubeRot;			//���� ������ �����, �� ���� ��� ���������
	D3DXMATRIX mPos;
	D3DXMATRIX mRot;
	D3DXMATRIX mOrg;			//������� ��������� � ����������
};

struct LVLMAP
{
	char	Name[32];			//�������� ������
	unsigned char	lenX;		//������ ���� �� �
	unsigned char	lenY;		//������ ���� �� Y
	unsigned char	stPX;		//��������� ������� �� �
	unsigned char	stPY;		//��������� ������� �� Y
	TILE	Tiles[64][64];		//����� ������ �������� lenX*lenY
};

/*
	11,12,13,14		������� �����
	21,22			�����
	31,32,33,34		����� ������ �
	41				����������
	51,52,53,54		��������� ����� �� �����
	61				������
*/
	OBJ			Plane;
	OBJ			Obj[16];
	LVLMAP		lvlMap;

	LPDIRECT3DTEXTURE8 tObj[16];
	LPD3DXSPRITE pSpr;
	float Scale,sX,sY;
	unsigned char mSelX,mSelY;
	char Selected;
	char Angle;
	char highlight;

	//Objects stuff
	PSYSTEMS Part;

	//Reflection
	D3DXPLANE	RefPlane;
	D3DXMATRIX	mReflect,mScale;
	D3DXVECTOR3	vUL,vUR,vDL,vDR;
	D3DXVECTOR3	a,b,c;
public:
	_PACMAN		pman;
	monster*	Monst;

	bool bIncAngle;
	bool bReflect;
	bool bLoaded,bFname;
	char filename[20];

	level();
	virtual ~level();

	bool level::LoadLevel(char *filename);
	bool level::LoadObjects(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice);
	void level::UpdateLevel(DWORD delta);
	void level::DrawLevel(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice);
	void level::DrawReflect(Cmodel* pMdl,D3DXVECTOR3 vEyePt,LPDIRECT3DDEVICE8 pDevice);
	void level::Draw(Cmodel* pMdl,D3DXVECTOR3 vEyePt,LPDIRECT3DDEVICE8 pDevice);
	void level::Release(Cmodel* pMdl);

	//������� ��������� �������
	void ClearLvl();
	bool NewLevel(unsigned char ucLX,unsigned char ucLY);
	bool level::SaveLevel();
	void level::DrawMap(LPDIRECT3DDEVICE8 pDevice);
	void level::DrawObject(LPDIRECT3DDEVICE8 pDevice,char obj);
	void SelObj(char obj);
	void IncAngle();
	char ChkSel(int x,int y);
	bool ChkMap(int x,int y);
	void SetHL(char obj);
	void SetMapObj();
	bool CalcMatrices();

	int GetMaxLen();
};

#endif // !defined(AFX_LEVEL_H__077847D1_5885_4723_8391_E8AE95894A7B__INCLUDED_)

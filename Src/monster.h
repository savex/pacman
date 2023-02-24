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
	Монстры бывают:
	 слабый		- просто ходит и ничего не видит
	 средний	- замечает пакмана, если он прямо перед ним и запоминает куда тот свернул
	 сильный	- всегда знает где пакман и идет к нему
*/
struct MONSTER
{
	int			Type;			//тип монстра (слабый,средний,сильный)
	int			nDir;			//следующее направление движения для 
								//среднего монстра, когда он заметил пакмана
	tModel*		curModel;		//указатель на текущую модель в анимации
	int			pX;				//положение на уровне по Х
	int			pY;				//положение на уровне по Y
	int			tX;				//клетка на которую передвигается по X
	int			tY;				//клетка на которую передвигается по Y
	int			iDir;			//направление движения
	D3DXVECTOR3	vPos;			//положение модели в пространстве
	D3DXMATRIX	mOrg;			//матрица ориентации
	DWORD		Speed;			//время для перемещения пакмана с клетки на клетку (мс)
	DWORD		mTime;			//time to reach next tile
	bool		bStand;			//стоит на месте или нет
	bool		bSight;			//пакман был виден или нет

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

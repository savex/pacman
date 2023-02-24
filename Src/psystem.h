// psystem.h: interface for the psystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PSYSTEM_H__5C7F622F_2CE1_405B_AC69_38B082F17563__INCLUDED_)
#define AFX_PSYSTEM_H__5C7F622F_2CE1_405B_AC69_38B082F17563__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		D3D_OVERLOADS
#define		randf	(float)(rand()%1000 / 1000.0f)
#define		NUM_PARTICLES	128

#define		D3DFVF_MYVERT (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct VERTEX
{
  float x, y, z;   // координаты
  DWORD Color;     // диффузный цвет
};

class psystem  
{

typedef struct _PARTICLE
{
	D3DXVECTOR3	pos;
	D3DXVECTOR3	vel;
	float		life;
	bool		alive;
};

typedef struct _P_ATRIB
{
	D3DXVECTOR3	spos;
	WORD	wRate;
	WORD	fLife;
	float	fSpeed;
	WORD	wTotal;
	bool	bStop;
};

	_PARTICLE	tPS[NUM_PARTICLES];

	DWORD		dwRateDelta;

	LPDIRECT3DTEXTURE8 pTex;
	LPDIRECT3DVERTEXBUFFER8	pVB;

public:
	_P_ATRIB	tAttr;
	psystem();
	virtual ~psystem();

	bool Init(IDirect3DDevice8 * pDevice,char* fn);
	void Update(DWORD delta);
	void Render(D3DXMATRIX *matr,IDirect3DDevice8 * pDevice);
	void Release();
	void SpawnParticle();
};

#endif // !defined(AFX_PSYSTEM_H__5C7F622F_2CE1_405B_AC69_38B082F17563__INCLUDED_)

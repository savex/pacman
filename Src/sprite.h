// sprite.h: interface for the sprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__7D870362_DE65_402F_980B_FB5C3025566B__INCLUDED_)
#define AFX_SPRITE_H__7D870362_DE65_402F_980B_FB5C3025566B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <stdlib.h>

class sprite  
{
protected:

struct SPR_INFO					//Заголовок SPR файла
{
	char	head[14];			//"SHOOTERSPRITE"
	int		rate;				//частота кадров <=30
	int		frames;				//количество кадров <=999
	bool	loop;				//замыкается или нет
};

struct FRAME					//по одной структуре на каждый кадр
{
	long	StartPos;
	long	Len;
};

struct TRANSITION
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 rotate;
	float	rAngle;
	D3DXVECTOR2* scale;
	RECT*	clip;
	D3DXCOLOR color;
};

struct CONTROL
{
	int		curFrame;
	int		Dir;
};

	SPR_INFO	desc;
	FRAME*		pSizes;
	void*		pData;

	LPDIRECT3DTEXTURE8 *pFrames;
	LPD3DXSPRITE pSpr;
	DWORD		frDelta;

public:
	UINT		Height,Width;
	TRANSITION	tran;
	CONTROL		cl;

	sprite();
	virtual ~sprite();

	bool Init(IDirect3DDevice8 * pDevice,char* fn);
	void Update(DWORD delta);
	void Render(IDirect3DDevice8 * pDevice);
	void Release();
};

#endif // !defined(AFX_SPRITE_H__7D870362_DE65_402F_980B_FB5C3025566B__INCLUDED_)

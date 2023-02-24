// bground.h: interface for the bground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BGROUND_H__283FF1E5_C859_40E3_84E7_161028EFBB05__INCLUDED_)
#define AFX_BGROUND_H__283FF1E5_C859_40E3_84E7_161028EFBB05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sprite.h"

#define	MAXSTARS 64

class bground  
{
protected:
struct STAR
{
	D3DXVECTOR2	pos;
	float		spMul;
	int			frame;
};
struct STATIC
{
	sprite*	pSpr;
	DWORD	Time;
};

STAR	Stars[MAXSTARS];
sprite	Star;
STATIC*	Statics;
UCHAR	numStat;

DWORD	eTime;
float	bgSpeed;
D3DXCOLOR color;
DWORD	Height;
DWORD	Width;

//плавное изменение скорости
float	newSpeed;
DWORD	slideTime;
bool	bSlide;

public:
	bground();
	virtual ~bground();

	bool InitBG(IDirect3DDevice8 * pDevice,DWORD height,DWORD width);
	void UpdateBG(DWORD delta);
	void RenderBG(IDirect3DDevice8 * pDevice);
	void SetSpeed(float speed);
	void SetSpeedSlide(float speed,DWORD time);

	bool AddStatic(IDirect3DDevice8 * pDevice,char* fn,float x,float y,DWORD time);
	void FreeStatics();

	void Release();
};

#endif // !defined(AFX_BGROUND_H__283FF1E5_C859_40E3_84E7_161028EFBB05__INCLUDED_)

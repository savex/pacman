// vsprite.h: interface for the vsprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSPRITE_H__1D2A4C24_ABB6_4118_B1E8_B481C6CE2475__INCLUDED_)
#define AFX_VSPRITE_H__1D2A4C24_ABB6_4118_B1E8_B481C6CE2475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		randf	(float)(rand()%1000 / 1000.0f)
#define		SPEED		0.2f
#define		INVSPEED	(1.0f/SPEED)

#define D3DFVF_MYVERT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct VERTEX
{
  float x, y, z;   // координаты
  DWORD Color;     // диффузный цвет
  float tu,tv;		//текстурные координаты
};


class vsprite  
{
protected:

struct VSPR_INFO					//Заголовок SPR файла
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
	D3DXVECTOR3 pos;
	float	rAngle;
	D3DXVECTOR2* scale;
	D3DXCOLOR color;
};

struct CONTROL
{
	int		curFrame;
	int		Dir;
};

public:
	
	VERTEX vert[4];		//вершины для вывода текстуры

	LPDIRECT3DTEXTURE8 pTex;	//текстура

	vsprite();
	virtual ~vsprite();

	bool Init(IDirect3DDevice8 * pDevice,char* fn,DWORD height,DWORD width);
	void Update(DWORD delta);
	void Render(IDirect3DDevice8 * pDevice);
	void Release();

};

#endif // !defined(AFX_VSPRITE_H__1D2A4C24_ABB6_4118_B1E8_B481C6CE2475__INCLUDED_)

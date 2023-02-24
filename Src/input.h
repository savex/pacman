// input.h: interface for the input class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__27DA39F5_4FC3_434C_B408_05302AABBDC4__INCLUDED_)
#define AFX_INPUT_H__27DA39F5_4FC3_434C_B408_05302AABBDC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dinput.h>

class input  
{
friend class player;
friend class UI;
struct KEYS
{
	bool	Up;
	bool	Down;
	bool	Left;
	bool	Right;
	bool	Action;
	char	buff[256];	//последние состояния клавиш
};

struct MOUSE
{
	long	pX;
	long	pY;
	long	pZ;		//Mouse Wheel
	bool	b1;
	bool	b2;
	bool	b3;
};

	LPDIRECTINPUT8			pDI;
	LPDIRECTINPUTDEVICE8	pKeybrd;
	LPDIRECTINPUTDEVICE8	pMouse;

	bool Chk(char buff[256],int idx);
	void GetBuff();
public:
	KEYS					Keys;
	MOUSE					Mouse;

	input();
	virtual ~input();

	bool CreateInput(HINSTANCE hInst,HWND hWnd);
	void CheckStates();
	void Release();

	bool ChkState(int idx);
	bool ChkKeyUp(int idx);
};

#endif // !defined(AFX_INPUT_H__27DA39F5_4FC3_434C_B408_05302AABBDC4__INCLUDED_)

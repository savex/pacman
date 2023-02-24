// ui.h: interface for the ui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UI_H__A44BFF20_A394_40B2_B2A6_C0AFFCD76660__INCLUDED_)
#define AFX_UI_H__A44BFF20_A394_40B2_B2A6_C0AFFCD76660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sprite.h"
#include "input.h"

#define MAXICONS	32
#define MAXIMG		32
#define MAXNOTE		80
#define MAXTEXT		32
#define LOGODELTA	10000
#define FDELTA		500
#define MDELTA		500

#define MI_W		256
#define MI_H		32
#define MIS_W		2
#define MIS_H		16
#define I_W			32
#define	I_H			32
#define MT_W		220
#define MT_H		32

#define	COLSC		0xFFE0CF14

class ui  
{
friend class level;
struct FADE
{
	bool	bFadeOut;
	bool	bFading;
	float	fColor;
	DWORD	dFadeDelta;
	int		nextState;
};
struct MOVE
{
	bool	bDone;
	float	fDelta;
	DWORD	dTranDelta;
	D3DXVECTOR2 stPos;
	D3DXVECTOR2 curPos;
	D3DXVECTOR2 endPos;
};
struct IMAGE
{
	char*				fname;
	FADE				stFade;
	LPDIRECT3DTEXTURE8	texImage;
	D3DXVECTOR2			pos;
	bool				bLoaded;
};
struct ICON
{
	LPDIRECT3DTEXTURE8	texIcon;
	char				note[MAXNOTE];
	char				text[MAXTEXT];
	FADE				stFade;
	MOVE				stMove;
	D3DXVECTOR2			pos;
	D3DXVECTOR2*		scale;
	D3DXCOLOR			color;
	bool				bActive;
	bool				bClick;
};
struct HITEM
{
	char	cName[12];			//Имя игрока в таблице рекордов
	char	cFile[12];			//Какой сценарий играл
	DWORD	dwScore;			//Набранные очки
};
struct STATS
{
	int		iPoints;
	int		iExtraItems;
	int		iMExterm;
	DWORD	dLvlTime;
	DWORD	dScore;
	HITEM	hiTable[8];
};
struct MENU
{
	ICON		items[6];		//icons and text for menu items
	int			nItems;			//всего пунктов меню
	bool		bHor;			//Горизонтально или вертикально
	bool		bFirst;			//Первый проход или нет
	bool		bText;			//Рисовать текст напротив иконок или нет
	bool		bNote;			//Рисовать подсказку или нет
	bool		bMoving;		//Двигается или нет
	int			tgMenu;			//Следующее меню, если bMoving=true
	D3DXVECTOR2 zPos;			//точка отсчета иконок
	char		text[MAXTEXT];	//заголовок меню
};

	IMAGE			Images[MAXIMG];
	int				iTotal;
	ICON			Icons[MAXICONS];
	DWORD			dHelpDelay;

	LPDIRECT3DTEXTURE8	texItemBg,texItemBgS;
	MENU			Menus[15];
	int				cMenu;

	RECT			rc,trc,hrc,rcHelp;
	STATS			Stats;
	char*			sErr;
	int				errTime;
	
	LPD3DXSPRITE	pSpr;
	sprite*			pExeLogo;
	sprite*			pCoolPacman;
	sprite			Logo;
	DWORD			dwLogoDelta;
	bool			bLogo;
	bool			bExe;
	bool			bCool;

	LPD3DXFONT		smFont;
	LPD3DXFONT		medFont;
	LPD3DXFONT		bigFont;
	DWORD			vHight,vWidth;
public:
	int				uiState;
	bool			bErr;
	float			debug;
	enum Mn{MAIN,OPTIONS,HISCORE,EDITOR,NEW,LOAD,SAVE,CREATESRT,EXIT,INGAME,START,
		SPLASH,CREDITS,HUD,QUIT};

	ui();
	virtual ~ui();

	int LoadImage(char *fname,LPDIRECT3DDEVICE8 pDevice);
	void ReleaseImage(int idx);
	bool LoadIcons(char *fname,LPDIRECT3DDEVICE8 pDevice);
	bool LoadMenus(char *fname,LPDIRECT3DDEVICE8 pDevice);

	bool InitUI(DWORD vH,DWORD vW,LPDIRECT3DDEVICE8 pDevice);
	void UpdateUI(DWORD delta,input* pInput,LPDIRECT3DDEVICE8 pDevice);
	void RenderUI(LPDIRECT3DDEVICE8 pDevice);
	void Release();

	void FirstTime(int curMenu);
	void GoToMenu(int menu,int curMenu);


	void FadeIn(FADE* pFade,DWORD dtl);
	void FadeOut(int uiState,FADE* pFade,DWORD dtl);
	void Fade(int idx,DWORD delta,FADE* pFade);

	void MoveBeg(int scMenu,int tgMenu);
	void Move(DWORD delta,int scMenu);

	void ShowError(char* err);
};

#endif // !defined(AFX_UI_H__A44BFF20_A394_40B2_B2A6_C0AFFCD76660__INCLUDED_)

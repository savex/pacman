// Pacman.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "d3dutil.h"
#include <time.h>
#include <commctrl.h>

//Классы
#include "sprite.h"
#include "psystem.h"
#include "bground.h"
#include "model.h"
#include "level.h"
#include "player.h"
#include "input.h"
#include "ui.h"

//Константы
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define WSIZE_X 640
#define WSIZE_Y 500

//Стандартные цвета
#define White		D3DCOLOR_XRGB(255,255,255);
#define Red			D3DCOLOR_XRGB(255,0,0);
#define Yellow		D3DCOLOR_XRGB(255,255,0);
#define Green		D3DCOLOR_XRGB(0,255,0);
#define Blue		D3DCOLOR_XRGB(0,0,255);
#define Purple		D3DCOLOR_XRGB(255,0,255);

#define lightBlue	D3DCOLOR_XRGB(123,123,255);

//Состояния программы
#define	LOAD	1
#define MENU	2
#define LEVEL	3
#define MOVIE	4
#define EDITOR	5
#define CREDITS	6

//uiStates
#define uiMAIN		0
#define uiOPTIONS	1
#define uiHISCORE	2
#define uiEDITOR	3
#define uiNEW		4
#define uiLOAD		5
#define uiSAVE		6
#define uiCREATESRT	7
#define uiEXIT		8
#define uiINGAME	9
#define uiSTART		10
#define uiSPLASH	11
#define uiCREDITS	12
#define uiHUD		13

struct WORLD
{
	//Lights
	D3DLIGHT8	lSun;

    //Camera
	D3DXVECTOR3 vUpVec;
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt;
	D3DXVECTOR3 vEyeSt;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mViewEdit;
	
	D3DXMATRIX mPos;
	D3DXMATRIX mRot;
	D3DXMATRIX mScl;
	D3DXMATRIX mOut;

	D3DXMATRIX mId;
};

//Глобальные переменные
HINSTANCE g_hInst;
HWND g_hWnd;
IDirect3D8 * pD3d;
IDirect3DDevice8 * pDevice;
bool bActive=false;
bool bHardVP=true;

//Переменные игры
WORLD		Wld;
level*		pLvl;
player*		pPM;
input*		pInput;
unsigned char ucLX=4,ucLY=4;

bool	bSound=true;	//звук вкл/выкл
int		iState;			//Состояние программы
char	stLoad[80];		//Состояние загрузки

//Классы
Cmodel* pMdl=NULL;			//Класс по загрузке обьектов
audio* pAudio=NULL;			//Класс по воспроизведению звука
sprite* pSpr=NULL;			//Иконка курсора
bground* pBG=NULL;			//Фон
psystem* pPS;				
ui* pUI=NULL;

//UI objects


//Отладочные переменные
LPD3DXFONT	sysFont;
DWORD		fpsTime,curTime,deltaTime,lastTime,fpsDeltaTime=1;
short		sFps,oFps;

float debug;

char* sErr[]=
{
	"Ошибка загрузки обьектов!",
	"Ошибка создания уровня!",
	"Ошибка создания диалога!",
	"Ошибка записи уровня!",
	"Ошибка загрузки уровня!"
};
int iErr=0;

//Прототипы ф-й
float rnd(float max);
bool Init( HWND hWnd );
bool CreateObjects();
void UpdateFrame();
void Render();
void ReleaseAll();

float rnd(int max)
{
	return (float)((rand() % max) + (rand() % 100)*0.01);
}

bool Init( HWND hWnd )
{
  if(pD3d = Direct3DCreate8( D3D_SDK_VERSION )){
    D3DDISPLAYMODE d3ddm; // отсюда нам нужно поле Format
    if( FAILED( pD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm )))
      return false;

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) ); // выставляем в 0 все поля d3dpp
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    if( FAILED( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pDevice))){
		bHardVP=false;
		if( FAILED( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &pDevice))){
			return false;
		}
    }

	//Создадим устройства ввода
	pInput=new input();
	pInput->CreateInput(g_hInst,g_hWnd);

	//Cursor
	pSpr=new sprite();
	if (!pSpr->Init(pDevice,".\\data\\cursor.spr")) return false;

	//Создадим шрифт
	HFONT font;
	font=(HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	if (FAILED(D3DXCreateFont(pDevice,font,&sysFont)))
		return false;

	Wld.vLookatPt=D3DXVECTOR3(0,0,0);
	Wld.vEyeSt=D3DXVECTOR3(0.0f,60.f,-300.f);
	Wld.vUpVec=D3DXVECTOR3(0,0.0f,-1.0f);
	D3DXMatrixLookAtLH(&Wld.mViewEdit, &Wld.vEyePt, &Wld.vLookatPt, &Wld.vUpVec);

	Wld.vLookatPt=D3DXVECTOR3(0,0,0);
	Wld.vEyeSt=D3DXVECTOR3(0.0f,-60.0f,ucLX*TLSIZE*1.3f);
	Wld.vUpVec=D3DXVECTOR3(0,0.0f,1.0f);
	D3DXMatrixLookAtLH(&Wld.mView, &Wld.vEyePt, &Wld.vLookatPt, &Wld.vUpVec);

	D3DXMatrixIdentity(&Wld.mId);

    FLOAT fAspect = 1.33f;
    D3DXMatrixPerspectiveFovLH( &Wld.mProj, D3DX_PI/4, fAspect, 1.0f, 1000.0f );
    pDevice->SetTransform( D3DTS_PROJECTION, &Wld.mProj );

	Wld.lSun.Type=D3DLIGHT_DIRECTIONAL;
	Wld.lSun.Diffuse=D3DXCOLOR(0.7f,0.7f,0.5f,1.0f);
	Wld.lSun.Ambient=D3DXCOLOR(0.45f,0.45f,0.45f,1.0f);
	Wld.lSun.Specular=D3DXCOLOR(0.2f,0.2f,0.2f,0.2f);
	Wld.lSun.Position=Wld.vEyePt;
	Wld.lSun.Direction=D3DXVECTOR3(0.3f,0.4f,-1.0f);

	D3DXMatrixTranslation(&Wld.mPos,0.0f,0.0f,0.0f);
	D3DXMatrixScaling(&Wld.mScl,0.1f,0.1f,0.1f);
	D3DXMatrixMultiply(&Wld.mOut,&Wld.mPos,&Wld.mScl);
	D3DXMatrixRotationZ(&Wld.mRot,curTime/500.0f);

	pDevice->SetLight(0,&Wld.lSun);
	pDevice->LightEnable(0,true);

    return true;
  }

  return false;
}

bool CreateObjects()
{
//    HRESULT hr;

	sprintf(stLoad,"Loading objects...");
	Render();
	
	pUI=new ui();
	if (!pUI->InitUI(WSIZE_Y,WSIZE_X,pDevice)) return false;
	
	pPS=new psystem();
	pPS->Init(pDevice,".\\data\\smoke.bmp");

	pBG=new bground();
	if (!pBG->InitBG(pDevice,WSIZE_Y,WSIZE_X)) return false;
	pBG->SetSpeed(0.3f);
//	pBG->SetSpeedSlide(10.0f,5000);

	pMdl=new Cmodel();	//Класс по обслуживанию моделей

	pLvl=new level();
	if (!pLvl->NewLevel(ucLX,ucLY) ||
		!pLvl->LoadObjects(pMdl,pDevice))
		return false;

	pPM=new player();
	if ((!pPM->LoadObjects(pMdl,pDevice)) ||
		(!pPM->Init(pLvl)))
		return false;
	
	sprintf(stLoad,"Initializing DirectMusic...");
	Render();


	//Init Audio;
/*	pAudio=new audio();
	if (!pAudio->Init(hWnd)) {
		bSound=false;
		MessageBox(hWnd,"Error loading audio. Turning sound off","Shooter",MB_OK);
	}
	
	sprintf(stLoad,"Loading music and sounds...");
	Render();
	if (bSound) pAudio->LoadFiles();
*/
	//Загрузка тестового уровня
	if (!pLvl->LoadLevel("noname")) iErr=5;
	pPM->Init(pLvl);

	iState=LEVEL;			//Обьекты созданы, можно переходить в следующее состояние
	return true;
}

void UpdateFrame()
{
	pInput->CheckStates();
	pSpr->tran.pos.x=(float)pInput->Mouse.pX-pSpr->Width;
	pSpr->tran.pos.y=(float)pInput->Mouse.pY-pSpr->Height;
	//Подсчет количества кадров
	curTime=timeGetTime();
	deltaTime=curTime-lastTime;
	lastTime=curTime;

	sFps++;
	if((curTime-fpsTime)>1000)
	{
		oFps=sFps;
		sFps=0;
		fpsDeltaTime=curTime-fpsTime;
		fpsTime=curTime;
	}
	
	if (pUI) {
		switch (pUI->uiState) {
		case uiEDITOR:
			iState=EDITOR;
			break;
		case uiINGAME:
			iState=LEVEL;
			pUI->UpdateUI(deltaTime,pInput,pDevice);
			return;		//since we are showing ingame menu do not update anything
			break;
		case uiHUD:
			iState=LEVEL;
			pUI->UpdateUI(deltaTime,pInput,pDevice);
			break;
		case uiMAIN:
			iState=MENU;
		}
	}

	//Поворот камеры при помощи мыши
	if (pLvl) Wld.vEyeSt=D3DXVECTOR3(0.0f,-60.0f,pLvl->GetMaxLen()*TLSIZE*1.3f);
	Wld.vEyePt=Wld.vEyeSt-(pInput->Mouse.pZ/800.f)*(-Wld.vEyeSt);
	
/*	D3DXMatrixRotationX(&Wld.mRot,-(pInput->Mouse.pY/100.f));
	D3DXVec3TransformCoord(&Wld.vEyePt,&Wld.vEyePt,&Wld.mRot);
	D3DXMatrixRotationZ(&Wld.mRot,pInput->Mouse.pX/100.f);
	D3DXVec3TransformCoord(&Wld.vEyePt,&Wld.vEyePt,&Wld.mRot);
*/	D3DXMatrixLookAtLH( &Wld.mView, &Wld.vEyePt, &Wld.vLookatPt, &Wld.vUpVec);

	//Обновление сцены
	switch (iState) {
	case LOAD:
		if (!CreateObjects()) iErr=1;
		break;
	case MENU:
		pUI->UpdateUI(deltaTime,pInput,pDevice);
		break;
	case LEVEL:
		if (pUI->uiState==uiMAIN) iState=MENU;

		pBG->UpdateBG(deltaTime);
		pLvl->UpdateLevel(deltaTime);
		pPM->Update(deltaTime,pLvl,pInput);
/*
		if (bSound) {
			if (!pAudio->IsPlaying()) pAudio->PlayMusic();
			pAudio->Update(deltaTime);
		}
*/
		break;
	case MOVIE:
		break;
	case CREDITS:
		break;
	case EDITOR:
		if (!pLvl->bLoaded) pLvl->NewLevel(ucLX,ucLY);
		pBG->UpdateBG(deltaTime);

		//Обработка мыши
		if (pInput->Mouse.b1) {
			pLvl->SelObj(pLvl->ChkSel(pInput->Mouse.pX,pInput->Mouse.pY));
			if (pLvl->ChkMap(pInput->Mouse.pX,pInput->Mouse.pY)) pLvl->SetMapObj();
		}
		if (pInput->Mouse.b2) pLvl->bIncAngle=true;
		else if ((pLvl->bIncAngle==true) && 
			(pLvl->ChkMap(pInput->Mouse.pX,pInput->Mouse.pY))) {
			pLvl->IncAngle();
			pLvl->bIncAngle=false;
		}
		else pLvl->bIncAngle=false;

		pLvl->SetHL(pLvl->ChkSel(pInput->Mouse.pX,pInput->Mouse.pY));
		pLvl->ChkMap(pInput->Mouse.pX,pInput->Mouse.pY);

		pUI->UpdateUI(deltaTime,pInput,pDevice);
		break;
	}

}

void Render()
{
  // Закрашиваем экран черным цветом
  DWORD dwBlack = D3DCOLOR_XRGB(0,0,0);
  pDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, dwBlack, 1.0f, 0 );

	pDevice->BeginScene();

	//Setting our RenderStates
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
    pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE);

	switch (iState) {
	case LOAD:
		RECT rct;
		rct.top=WSIZE_Y/2+150;
		rct.bottom=WSIZE_Y/2+170;
		rct.left=0;
		rct.right=WSIZE_X;
		sysFont->DrawTextA(stLoad,strlen(stLoad),&rct,DT_CENTER | DT_VCENTER,0xFFFFFFFF);
		break;
	case MENU:
		pDevice->SetTransform(D3DTS_VIEW,&Wld.mView);
		pBG->RenderBG(pDevice);	//background
		pUI->RenderUI(pDevice);
		pSpr->Render(pDevice);	//Cursor
		break;
	case LEVEL:
		pDevice->SetTransform(D3DTS_VIEW,&Wld.mView);

		pBG->RenderBG(pDevice);

		pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE);

		if (pLvl->bLoaded) pLvl->Draw(pMdl,Wld.vEyePt,pDevice);
//		pPM->Render(pMdl,pDevice);

		pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pUI->RenderUI(pDevice);
		break;
	case MOVIE:
		break;
	case CREDITS:
		break;
	case EDITOR:
		pDevice->SetTransform(D3DTS_VIEW,&Wld.mViewEdit);
		pBG->RenderBG(pDevice);

		for(int i=0;i<16;i++) pLvl->DrawObject(pDevice,i);
		pLvl->DrawMap(pDevice);

		pUI->RenderUI(pDevice);

		pSpr->Render(pDevice);	//Cursor
		break;
	}


	//Вывод счетчика кадров и отладочных данных
	RECT rc;
	char stFps[64];

	sprintf(stFps,"%4.1f",oFps*1000.0/fpsDeltaTime);
	rc.left=rc.top=0;
	rc.right=50;
	rc.bottom=20;
	sysFont->DrawTextA(stFps,strlen(stFps),&rc,DT_CENTER | DT_VCENTER,0xFFFFFF00);


//	float debug;
	if (pUI) debug=pUI->debug;			//отладочные данные
	sprintf(stFps,"%05d",debug);
/*	sprintf(stFps,"%5d\nx=%4d, y=%4d, z=%4d, %d %d %d\nU:%d\nD:%d\nL:%d\nR:%d\nA:%d\n",
					debug,
					pInput->Mouse.pX,
					pInput->Mouse.pY,
					pInput->Mouse.pZ,
					(pInput->Mouse.b1==true) ? 1:0,
					(pInput->Mouse.b2==true) ? 1:0,
					(pInput->Mouse.b3==true) ? 1:0,
					(pInput->Keys.Up==true)?1:0,
					(pInput->Keys.Down==true)?1:0,
					(pInput->Keys.Left==true)?1:0,
					(pInput->Keys.Right==true)?1:0,
					(pInput->Keys.Action==true)?1:0);
*/	rc.left=0;
	rc.top=30;
	rc.right=WSIZE_X;
	rc.bottom=WSIZE_Y;
	sysFont->DrawTextA(stFps,strlen(stFps),&rc,DT_LEFT | DT_TOP,0xFFFFFF00);
	
  pDevice->EndScene();

  pDevice->Present(0, 0, 0, 0);
}


// функция обрабатывающая сообщения главного окна приложения
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_ACTIVATEAPP:
		bActive=!bActive;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F1:
			if (iState==MENU) pUI->GoToMenu(uiEDITOR,pUI->uiState);
			break;
		case VK_F2:
			if (iState==MENU) pUI->GoToMenu(uiHUD,pUI->uiState);
			break;
		case VK_F3:
			if (iState==EDITOR) pLvl->CalcMatrices();
			pUI->GoToMenu(uiMAIN,pUI->uiState);
			break;
		case VK_F4:
			pLvl->bReflect=!pLvl->bReflect;
			break;
		case VK_F6:
			if (!pLvl->SaveLevel()) iErr=4;
			break;
		case VK_F9:
			iState=EDITOR;
			if (!pLvl->LoadLevel("noname")) iErr=5;
			pPM->Init(pLvl);
			break;
		}
		break;
	case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
  RECT rc;
	
	
  g_hInst=hInst;
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                    GetModuleHandle(0), 0, 0, 0, 0, "Pacman_cl", 0};
  RegisterClassEx( &wc );


  // Создание главного окна приложения
  GetClientRect(GetDesktopWindow(),&rc);
  g_hWnd = CreateWindow( "Pacman_cl", "Pacman",
							WS_OVERLAPPED|WS_SYSMENU, 
							rc.right/2-WSIZE_X/2, 
							rc.bottom/2-WSIZE_Y/2, 
							WSIZE_X, WSIZE_Y,
							GetDesktopWindow(), NULL, wc.hInstance, NULL );

 	fpsTime=timeGetTime();
	srand( (unsigned)time( NULL ) );

  if(Init(g_hWnd))
  {
    ShowWindow( g_hWnd, SW_SHOWDEFAULT );
    UpdateWindow( g_hWnd );
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );

	iState=LOAD;			//Создание окон завершено, можно загружать обьекты

    while( msg.message!=WM_QUIT )
    {
      if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
      {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
      }
      else
        if (bActive)
		{
			UpdateFrame();
			if (iErr>0) {
				MessageBox(g_hWnd,sErr[iErr-1],"Shooter",MB_OK);
				break;
			}
			Render();
		}
    }
  }
  else MessageBox(g_hWnd,"Ошибка инициализации!","Shooter",MB_OK);
  
  ReleaseAll();
  UnregisterClass( "Pacman_cl", wc.hInstance );
  return 0;
}


void ReleaseAll()
{
  // Освобождаются в обратном порядке создания

	SAFE_RELEASE(pSpr);
	SAFE_RELEASE(pInput);
	SAFE_RELEASE(pAudio);
	SAFE_RELEASE(pBG);
	if (pLvl) pLvl->Release(pMdl);
	if (pPM) pPM->Release(pMdl);
	if (pPS) pPS->Release();
	SAFE_RELEASE(pUI);

	SAFE_DELETE(pSpr);
	SAFE_DELETE(pInput);
	SAFE_DELETE(pLvl);
	SAFE_DELETE(pAudio);
	SAFE_DELETE(pBG);
	SAFE_DELETE(pPS);
	SAFE_DELETE(pUI);

	SAFE_DELETE(pMdl)
	
	SAFE_RELEASE(sysFont);
	if(pDevice) 
		pDevice->Release();
	if(pD3d)
		pD3d->Release();
}

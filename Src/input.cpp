// input.cpp: implementation of the input class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

input::input()
{
	Keys.Up=false;
	Keys.Down=false;
	Keys.Left=false;
	Keys.Right=false;
	Keys.Action=false;

	Mouse.b1=false;
	Mouse.b2=false;
	Mouse.b3=false;
	Mouse.pX=0;
	Mouse.pY=0;
	Mouse.pZ=0;
}

input::~input()
{

}


bool input::CreateInput(HINSTANCE hInst,HWND hWnd)
{

    HRESULT hr;
	POINT cPos;

	cPos.x=0;
	cPos.y=0;
	GetCursorPos(&cPos);
	Mouse.pX=cPos.x;
	Mouse.pY=cPos.y;

    // Create the DirectInput object. 
    hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, 
                            IID_IDirectInput8, (void**)&pDI, NULL); 
    if FAILED(hr) return FALSE; 
    hr = pDI->CreateDevice(GUID_SysKeyboard, &pKeybrd, NULL); 
    if FAILED(hr) return false;
    hr = pDI->CreateDevice(GUID_SysMouse, &pMouse, NULL); 
    if FAILED(hr) return false;
 
    // Set the data format using the predefined keyboard data 
    // format provided by the DirectInput object for keyboards. 
    hr = pKeybrd->SetDataFormat(&c_dfDIKeyboard); 
    if FAILED(hr) return false;
	hr = pMouse->SetDataFormat(&c_dfDIMouse );
    if FAILED(hr) return false;

    // Set the cooperative level 
    hr = pKeybrd->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
    if FAILED(hr) return false;
	hr = pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if FAILED(hr) return false;
 
    // Get access to the input device. 
    hr = pKeybrd->Acquire(); 
    if FAILED(hr) return false;
	hr = pMouse->Acquire();
    if FAILED(hr) return false;

	return true;
}

void input::GetBuff()
{
    HRESULT hr;

	hr = pKeybrd->GetDeviceState(sizeof(Keys.buff),(LPVOID)&Keys.buff); 
    if FAILED(hr) 
    { 
		//≈сли произошла ошибка, то значит другое приложение 
		//перехватило ввод.
		hr = pKeybrd->Acquire();
		while (hr==DIERR_INPUTLOST) 
			hr = pKeybrd->Acquire();
         return; 
    } 
}

void input::CheckStates()
{
	//ѕровер€ем статус клавиш на влавиатуре
    HRESULT  hr; 
 
	GetBuff();

	Keys.Up=Chk(Keys.buff,DIK_UP);
	Keys.Down=Chk(Keys.buff,DIK_DOWN);
	Keys.Left=Chk(Keys.buff,DIK_LEFT);    
	Keys.Right=Chk(Keys.buff,DIK_RIGHT);
	Keys.Action=Chk(Keys.buff,DIK_LCONTROL);

	//ѕровер€ем статус мыши
    DIMOUSESTATE dims;      // DirectInput mouse state structure
    ZeroMemory( &dims, sizeof(dims) );
    hr = pMouse->GetDeviceState( sizeof(DIMOUSESTATE), &dims );
    if( FAILED(hr) ) 
    {
        //≈сли устройство перехвачено, продолжать пытатьс€ захватить его снова
        hr = pMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = pMouse->Acquire();
        return; 
    }
    
	//Some mouse drivers acts strangely with wheels
	//Genius NetScroll Optical returns absolute Z-axis
	//number, other than native W'2000 drivers returns
	//differense in the current momentum
	Mouse.pX+=dims.lX;
	Mouse.pY+=dims.lY;
	Mouse.pZ=dims.lZ;	//≈сли значение по€вл€етс€ и исчезает,
						//то надо заменить '=' на '+='
	Mouse.b1=(dims.rgbButtons[0] & 0x80) ? true : false;
	Mouse.b2=(dims.rgbButtons[1] & 0x80) ? true : false;
	Mouse.b3=(dims.rgbButtons[2] & 0x80) ? true : false;
	if (Mouse.pX<15) Mouse.pX=15;
	if (Mouse.pY<15) Mouse.pY=15;
	if (Mouse.pX>650) Mouse.pX=650;
	if (Mouse.pY>485) Mouse.pY=485;
}

bool input::Chk(char buff[256],int idx)
{
	if (buff[idx]&0x80) return true;
	else return false;
}


void input::Release()
{

    if (pDI) 
    { 
        if (pKeybrd) 
        { 
            pKeybrd->Unacquire(); 
            pKeybrd->Release();
            pKeybrd=NULL; 
        } 
        if (pMouse) 
        { 
            pMouse->Unacquire(); 
            pMouse->Release();
            pMouse=NULL; 
        } 
        pDI->Release();
        pDI=NULL; 
    } 

}

bool input::ChkState(int idx)
{
	if (Keys.buff[idx]&0x80) return true;
	else return false;
}

bool input::ChkKeyUp(int idx)
{
	if (Keys.buff[idx]&0x80) {
		while (Keys.buff[idx]&0x80) 
			GetBuff();
		return true;
	}
	else return false;
}

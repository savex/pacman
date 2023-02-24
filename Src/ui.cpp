// ui.cpp: implementation of the ui class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui.h"

#include "windows.h"
#include "commdlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ui::ui()
{
	uiState=SPLASH;
	cMenu=SPLASH;

	iTotal=0;
	dHelpDelay=1500;
	dwLogoDelta=0;

	rc.top=0;
	rc.bottom=480;
	rc.left=0;
	rc.right=640;
	errTime=2000;
	bErr=false;

	bLogo=false;
	bExe=false;
	bCool=false;

	ZeroMemory(Icons,sizeof(ICON)*MAXICONS);
	ZeroMemory(Menus,sizeof(MENU)*15);
	pSpr=NULL;
	smFont=
	medFont=
	bigFont=NULL;
	pExeLogo=NULL;
	pCoolPacman=NULL;

	vHight=vWidth=0;

	hrc.top=10;
	hrc.bottom=100;
	hrc.left=0;
	hrc.right=0;

	ZeroMemory(&Stats,sizeof(STATS));
}

ui::~ui()
{

}

//Загрузка картинки
int ui::LoadImage(char *fname,LPDIRECT3DDEVICE8 pDevice)
{
	HRESULT hr;

	for (int i=0;i<iTotal;i++)
		if (strcmp(fname,Images[i].fname)==0) return i;

	ZeroMemory(Images+iTotal,sizeof(IMAGE));
	hr=D3DXCreateTextureFromFile(pDevice,fname,&Images[iTotal].texImage);
	if (hr!=D3D_OK) {
		ShowError("Ошибка загрузки файла!");
		return -1;
	}
	else Images[iTotal].fname=fname;
	Images[iTotal].pos=D3DXVECTOR2(0,0);
	Images[iTotal].bLoaded=true;

	return (iTotal++);
}

//Удаление картинки из памяти
void ui::ReleaseImage(int idx)
{
	if (Images[idx].texImage) 
		Images[idx].texImage->Release();
	iTotal--;
	for(int i=idx;i<iTotal;i++)
		memcpy(Images+i,Images+i+1,sizeof(IMAGE));
	ZeroMemory(Images+(++i),sizeof(IMAGE));
}

bool StrEx(char* out,char* sLine)
{
	char* tmp=strstr(sLine,"=\"");
	if (tmp==NULL) return false;
	tmp+=2;
	int i=0;
	while (tmp[i]!='\"') out[i]=tmp[i++];
	out[i]=0;

	return true;
}
//Загрузка всех текстовых сообщений
bool ui::LoadIcons(char *fname,LPDIRECT3DDEVICE8 pDevice)
{
	using namespace std;
	fstream	in;
	HRESULT hr;
	char		sTag[4],sLine[80];
	char*		cNum;
	char		tagIcon[]="ICON";
	char		tagNote[]="NOTE";
	char		tagText[]="TEXT";

	char		ficon[128];
	int			cnt=0,i;

	in.open(fname,ios_base::in);
	if (!in.is_open()) return false;
	
	while (!in.eof()) {
		in.getline(sLine,80,'\n');
		for (i=0;i<4;i++) sTag[i]=sLine[i];
		sTag[i]=0;
		if (strcmp(sTag,tagIcon)==0) {
			strcpy(ficon,".\\data\\");
			if (!StrEx(ficon+strlen(ficon),sLine)) return false;
			cNum=sLine+4;
			i=0;
			while (cNum[i]!='=') i++;
			cNum[i]=0;
			int num=atoi(cNum);
			cNum[i]='=';
			hr=D3DXCreateTextureFromFile(pDevice,ficon,&Icons[--num].texIcon);
			if (hr!=D3D_OK) {
				ShowError("Ошибка загрузки файла!");
				return false;
			}
			Icons[num].color=0xFFFFFFFF;
			Icons[num].scale=NULL;
			continue;
		}
		if (strcmp(sTag,tagNote)==0) {
			cNum=sLine+4;
			i=0;
			while (cNum[i]!='=') i++;
			cNum[i]=0;
			int num=atoi(cNum);
			cNum[i]='=';
			if (!StrEx(Icons[--num].note,sLine)) return false;
			continue;
		}
		if (strcmp(sTag,tagText)==0) {
			cNum=sLine+4;
			i=0;
			while (cNum[i]!='=') i++;
			cNum[i]=0;
			int num=atoi(cNum);
			cNum[i]='=';
			if (!StrEx(Icons[--num].text,sLine)) return false;
			continue;
		}
	}
	in.close();


	return true;
}

//Загрузка файла разметки меню
bool ui::LoadMenus(char *fname,LPDIRECT3DDEVICE8 pDevice)
{
	using namespace std;
	fstream	in;
	char*		strMenu[14]=
	{
		"[MAIN]",
		"[OPTIONS]",
		"[HISCORE]",
		"[EDITOR]",
		"[NEW]",
		"[LOAD]",
		"[SAVE]",
		"[CREATESRT]",
		"[EXIT]",
		"[INGAME]",
		"[START]",
		"[SPLASH]",
		"[CREDITS]",
		"[HUD]"
	};
	char	Line1[80],Line2[80],Line3[80];
	int		nIcons;
	char	xPos[5],yPos[5];
	bool	bFlag;

	in.open(fname,ios_base::in);
	if (!in.is_open()) return false;

	while (!in.eof()) {
		int curMenu;
		in.getline(Line1,80,'\n');
		in.getline(Line2,80,'\n');
		in.getline(Line3,80,'\n');

		if (Line1[0]!='[' ||
			Line2[0]!='/') return false;

		//detecting corresponding array pos for current menu
		for (int i=0;i<14;i++) 
			if (strcmp(Line1,strMenu[i])==0) {
				curMenu=i;
				ZeroMemory(&Menus[curMenu],sizeof(MENU));
				break;
			}
		
		//gettung properties
		nIcons=Line2[1]-0x30;
		if (Line2[2]=='V') Menus[curMenu].bHor=false;
		else if (Line2[2]=='H') Menus[curMenu].bHor=true;
		else return false;

		if (Line2[3]=='T') Menus[curMenu].bText=true;
		else Menus[curMenu].bText=false;
		if (Line2[4]=='N') Menus[curMenu].bNote=true;
		else Menus[curMenu].bNote=false;

		//getting position for current menu
		char* tmp=strstr(Line2,",");
		if (tmp==NULL) return false;
		tmp++;
		int j=0;
		i=0;
		bFlag=false;
		while (tmp[i]!=',') {
			if (tmp[i]==':') {
				bFlag=true;
				xPos[i]=0;
				i++;
			}
			else if (!bFlag) xPos[i]=tmp[i++];
			else yPos[j++]=tmp[i++];
		}
		yPos[j]=0;

		//if center pos is present count it
		if (strcmp(xPos,"C")!=0) Menus[curMenu].zPos.x=(float)atoi(xPos);
		else if (Menus[curMenu].bHor) 
			Menus[curMenu].zPos.x=(float)vWidth/2-(MIS_W*(nIcons-1))/2-(MI_W*nIcons)/2;
		else Menus[curMenu].zPos.x=vWidth/2.f-MI_W/2.f;

		if (strcmp(yPos,"C")!=0) Menus[curMenu].zPos.y=(float)atoi(yPos);
		else if (Menus[curMenu].bHor) 
			Menus[curMenu].zPos.y=vHight/2.f-MI_H/2.f;
		else Menus[curMenu].zPos.y=(float)vHight/2-(MIS_H*(nIcons-1))/2-(MI_H*nIcons)/2;

		//getting menu header text
		tmp=strstr(Line2,",\"");
		if (tmp==NULL) return false;
		tmp+=2;
		i=0;
		while (tmp[i]!='\"') Menus[curMenu].text[i]=tmp[i++];
		Menus[curMenu].text[i]=0;

		//getting icon numbers
		tmp=Line3;
		i=0;j=0;
		int k=0;
		while (tmp[i]!=0) {
			if (tmp[i]==',') {
				xPos[j]=0;
				int num=atoi(xPos);
				memcpy(&Menus[curMenu].items[k++],&Icons[num],sizeof(ICON));
				i++;
				j=0;
			}
			else xPos[j++]=tmp[i++];
		}
		xPos[j]=0;
		int num=atoi(xPos);
		memcpy(&Menus[curMenu].items[k],&Icons[num],sizeof(ICON));

		Menus[curMenu].nItems=nIcons;
		for(i=0;i<nIcons;i++) 
			if (Menus[curMenu].bHor) {
				if (Menus[curMenu].bText)
					Menus[curMenu].items[i].pos.x=Menus[curMenu].zPos.x+MI_W*i+MIS_W*i;
				else Menus[curMenu].items[i].pos.x=Menus[curMenu].zPos.x+I_W*i+MIS_W*i;
				Menus[curMenu].items[i].pos.y=Menus[curMenu].zPos.y;
			}
			else {
				Menus[curMenu].items[i].pos.x=Menus[curMenu].zPos.x;
				if (Menus[curMenu].bText)
					Menus[curMenu].items[i].pos.y=Menus[curMenu].zPos.y+MI_H*i+MIS_H*i;
				else Menus[curMenu].items[i].pos.y=Menus[curMenu].zPos.y+I_H*i+MIS_H*i;
			}
		Menus[curMenu].bFirst=true;
		Menus[curMenu].bMoving=false;
	}
	in.close();

	return true;
}

//Инициализация всего необходимого для работы
bool ui::InitUI(DWORD vH,DWORD vW,LPDIRECT3DDEVICE8 pDevice)
{
	HRESULT hr;

	vHight=vH-20;
	vWidth=vW;
	hrc.right=vWidth;
	rcHelp.top=vHight-MI_H;
	rcHelp.bottom=vHight;
	rcHelp.left=0;
	rcHelp.right=vWidth;
	if (!LoadIcons(".\\data\\icons.str",pDevice)) return false;
	if (!LoadMenus(".\\data\\menu.def",pDevice)) return false;

	hr=D3DXCreateTextureFromFile(pDevice,".\\data\\menuItem.tga",&texItemBg);
	if (hr!=D3D_OK) {
		ShowError("Ошибка загрузки файла!");
		return false;
	}
	hr=D3DXCreateTextureFromFile(pDevice,".\\data\\menuItemS.tga",&texItemBgS);
	if (hr!=D3D_OK) {
		ShowError("Ошибка загрузки файла!");
		return false;
	}

	Logo.Init(pDevice,".\\data\\pmLogo.spr");
	Logo.tran.pos.x=(float)vWidth/2-Logo.Width/2;
	Logo.tran.pos.y=20;
	Logo.cl.Dir=0;
	LOGFONT lFnt;

	ZeroMemory(&lFnt,sizeof(lFnt));
	lFnt.lfHeight=-11;
	lFnt.lfWeight=400;
	lFnt.lfCharSet=204;
	lFnt.lfOutPrecision=3;
	lFnt.lfClipPrecision=2;
	lFnt.lfQuality=ANTIALIASED_QUALITY;
	lFnt.lfPitchAndFamily=34;
	strcpy(lFnt.lfFaceName,"Tahoma");

	hr=D3DXCreateFontIndirect(pDevice,&lFnt,&smFont);
	if (hr!=D3D_OK) return false;

	lFnt.lfHeight=-16;
	lFnt.lfWeight=1000;
	lFnt.lfQuality=ANTIALIASED_QUALITY;
	D3DXCreateFontIndirect(pDevice,&lFnt,&medFont);
	if (hr!=D3D_OK) return false;

	lFnt.lfHeight=-32;
	lFnt.lfQuality=ANTIALIASED_QUALITY;
	D3DXCreateFontIndirect(pDevice,&lFnt,&bigFont);
	if (hr!=D3D_OK) return false;

	D3DXCreateSprite(pDevice,&pSpr);

	return true;
}

//Обновеление
void ui::UpdateUI(DWORD delta,input* pInput,LPDIRECT3DDEVICE8 pDevice)
{

	int idx,i;
	if (bErr) {
		errTime-=delta;
		if (errTime<0) PostQuitMessage(1);
	}
	else {
		//debug
		debug=Menus[0].items[0].stMove.stPos.y;

		bLogo=false;
		cMenu=uiState;
		Move(delta,cMenu);
		for(i=0;i<iTotal;i++) 
			if (Images[i].bLoaded) Fade(i,delta,&Images[i].stFade);

		switch (uiState) {
		case SPLASH:
			if (Menus[cMenu].bFirst) {
				idx=LoadImage(".\\data\\title.jpg",pDevice);
				FadeIn(&Images[idx].stFade,FDELTA);
				Menus[cMenu].bFirst=false;
			}
			if (pInput->ChkKeyUp(DIK_ESCAPE)) {
				for(i=0;i<iTotal;i++) 
					if (Images[i].bLoaded) FadeOut(-1,&Images[i].stFade,FDELTA);
					
				MoveBeg(cMenu,MAIN);
			}
			
			/*			if (!Images[idx].stFade.bFading && Images[idx].stFade.bFadeOut)
			MoveBeg(cMenu,MAIN);
			*/
			break;
		case MAIN:
			cMenu=MAIN;
			bLogo=true;

			Logo.Update(delta);
			if (dwLogoDelta>LOGODELTA) {
			Logo.cl.curFrame=0;
			Logo.cl.Dir=1;
			dwLogoDelta=0;
			}
			dwLogoDelta+=delta;
			Logo.tran.color=D3DXCOLOR(Menus[cMenu].items[0].stFade.fColor,
									  Menus[cMenu].items[0].stFade.fColor,
									  Menus[cMenu].items[0].stFade.fColor,
									  Menus[cMenu].items[0].stFade.fColor);

			if (pInput->ChkKeyUp(DIK_ESCAPE)) 
				MoveBeg(cMenu,EXIT);
			if (pInput->ChkKeyUp(DIK_F1)) 
				MoveBeg(cMenu,INGAME);
			break;
		case OPTIONS:
			
			break;
		case HISCORE:
			
			break;
		case EDITOR:
			cMenu=EDITOR;
			FirstTime(cMenu);
			
			for(i=0;i<Menus[cMenu].nItems;i++)
				Fade(-1,delta,&Menus[cMenu].items[i].stFade);

			if (pInput->ChkKeyUp(DIK_ESCAPE)) GoToMenu(MAIN,cMenu);
			break;
		case CREDITS:
			
			break;
		case NEW:
			
			break;
		case LOAD:
			
			break;
		case SAVE:
			
			break;
		case CREATESRT:
			
			break;
		case EXIT:
			Move(delta,cMenu);

			if (pInput->ChkKeyUp(DIK_ESCAPE)) MoveBeg(cMenu,QUIT);
			break;
		case INGAME:
			cMenu=INGAME;
			FirstTime(cMenu);
			
			for(i=0;i<Menus[cMenu].nItems;i++)
				Fade(-1,delta,&Menus[cMenu].items[i].stFade);

			if (pInput->ChkKeyUp(DIK_ESCAPE)) GoToMenu(HUD,cMenu);
			break;
		case HUD:
			if (pInput->ChkKeyUp(DIK_ESCAPE)) {
				uiState=INGAME;
				Menus[uiState].bFirst=true;
			}
			break;
		case START:

			break;
		case QUIT:

			PostQuitMessage(0);
			break;
		}
	}
}

//Вывод Интерфейса на экран
void ui::RenderUI(LPDIRECT3DDEVICE8 pDevice)
{
	int i;
	if (bErr) medFont->DrawTextA(sErr,strlen(sErr),&rc,DT_CENTER | DT_VCENTER,0xFFFFFFFF);
	else {
		//Выведем все загруженные картинки
		pSpr->Begin();
		for(i=0;i<iTotal;i++) {
			float col=Images[i].stFade.fColor;
			pSpr->Draw(Images[i].texImage,
				NULL,
				NULL,
				NULL,
				0,
				&Images[i].pos,
				D3DXCOLOR(col,col,col,col));
		}
		pSpr->End();

		if (bLogo) Logo.Render(pDevice);

		if (uiState==HUD) {
			char	tmp[32];

			itoa(Stats.dScore,tmp,10);
			bigFont->DrawTextA(tmp,-1,&hrc,DT_LEFT | DT_VCENTER,COLSC);
		}

		//Выведем все активные иконки и текст
		if (cMenu!=-1) {
			bigFont->DrawTextA(Menus[cMenu].text,
				-1,
				&hrc,
				DT_CENTER | DT_VCENTER,
				0xFFFFFFFF);
			
			if (Menus[cMenu].bMoving) {
					//cMenu && bMoving
				for (i=0;i<Menus[cMenu].nItems;i++) {
					float col=Menus[cMenu].items[i].stFade.fColor;
					trc.top=(long)Menus[cMenu].items[i].stMove.curPos.y;
					trc.bottom=trc.top+MI_H;
					trc.left=(long)Menus[cMenu].items[i].stMove.curPos.x+32+MIS_W;
					trc.right=trc.left+MT_W;
					pSpr->Draw(Menus[cMenu].items[i].texIcon,NULL,NULL,NULL,0,
						&Menus[cMenu].items[i].stMove.curPos,
						D3DXCOLOR(col,col,col,col));
					if (Menus[cMenu].bText) {
						pSpr->Draw(texItemBg,NULL,NULL,NULL,0,
							&D3DXVECTOR2(Menus[cMenu].items[i].stMove.curPos.x-MIS_W*4,
							Menus[cMenu].items[i].stMove.curPos.y-MIS_H),
							D3DXCOLOR(col,col,col,col));
						medFont->DrawTextA(Menus[cMenu].items[i].text,
							-1,
							&trc,
							DT_CENTER | DT_VCENTER,
							D3DXCOLOR(col*0.7f,col*0.7f,col*0.7f,col));
						
					}
				}
					//tgMenu && bMoving
				int tgMenu=Menus[cMenu].tgMenu;
				for (i=0;i<Menus[tgMenu].nItems;i++) {
					float col=Menus[tgMenu].items[i].stFade.fColor;
					trc.top=(long)Menus[tgMenu].items[i].stMove.curPos.y;
					trc.bottom=trc.top+MI_H;
					trc.left=(long)Menus[tgMenu].items[i].stMove.curPos.x+32+MIS_W;
					trc.right=trc.left+MT_W;
					pSpr->Draw(Menus[tgMenu].items[i].texIcon,NULL,NULL,NULL,0,
						&Menus[tgMenu].items[i].stMove.curPos,
						D3DXCOLOR(col,col,col,col));
					if (Menus[tgMenu].bText) {
						pSpr->Draw(texItemBg,NULL,NULL,NULL,0,
							&D3DXVECTOR2(Menus[tgMenu].items[i].stMove.curPos.x-MIS_W*4,
							Menus[tgMenu].items[i].stMove.curPos.y-MIS_H),
							D3DXCOLOR(col,col,col,col));
						medFont->DrawTextA(Menus[tgMenu].items[i].text,
							-1,
							&trc,
							DT_CENTER | DT_VCENTER,
							D3DXCOLOR(col*0.7f,col*0.7f,col*0.7f,col));
						
					}
				}
			}
			else {
				for (i=0;i<Menus[cMenu].nItems;i++) {
					float col=Menus[cMenu].items[i].stFade.fColor;
					trc.top=(long)Menus[cMenu].items[i].pos.y;
					trc.bottom=trc.top+MI_H;
					trc.left=(long)Menus[cMenu].items[i].pos.x+32+MIS_W;
					trc.right=trc.left+MT_W;
					pSpr->Draw(Menus[cMenu].items[i].texIcon,NULL,NULL,NULL,0,
						&Menus[cMenu].items[i].pos,
						D3DXCOLOR(col,col,col,col));
					if (Menus[cMenu].bText) {
						if (Menus[cMenu].items[i].bActive) {
							pSpr->Draw(texItemBgS,NULL,NULL,NULL,0,
								&D3DXVECTOR2(Menus[cMenu].items[i].pos.x-MIS_W*4,
								Menus[cMenu].items[i].pos.y-MIS_H),
								D3DXCOLOR(col,col,col,col));
							medFont->DrawTextA(Menus[cMenu].items[i].text,
								-1,
								&trc,
								DT_CENTER | DT_VCENTER,
								D3DXCOLOR(col,col,col,col));
						}
						else {
							pSpr->Draw(texItemBg,NULL,NULL,NULL,0,
								&D3DXVECTOR2(Menus[cMenu].items[i].pos.x-MIS_W*4,
								Menus[cMenu].items[i].pos.y-MIS_H),
								D3DXCOLOR(col,col,col,col));
							medFont->DrawTextA(Menus[cMenu].items[i].text,
								-1,
								&trc,
								DT_CENTER | DT_VCENTER,
								D3DXCOLOR(col*0.7f,col*0.7f,col*0.7f,col));
						}
						
					}
					if (Menus[cMenu].items[i].bActive && Menus[cMenu].bNote) {
						smFont->DrawTextA(Menus[cMenu].items[i].note,
							-1,
							&rcHelp,
							DT_CENTER | DT_VCENTER,
							0xFFFFFFFF);
					}
				} //for
			} //if bMoving
		} //if cMenu!=-1

		//Выведем все активные подсказки

	} //if err
} //func

//Освобождение памяти
void ui::Release()
{
	for (int i=0;i<iTotal;i++) Images[i].texImage->Release();
	for (i=0;i<MAXICONS;i++) 
		if (Icons[i].texIcon) Icons[i].texIcon->Release();

	Logo.Release();

	if (pExeLogo) pExeLogo->Release();
	if (pSpr) pSpr->Release();
	if (smFont) smFont->Release();
	if (medFont) medFont->Release();
	if (bigFont) bigFont->Release();

	delete pExeLogo;
}

void ui::FirstTime(int curMenu)
{
	if (Menus[curMenu].bFirst) 
		for(int i=0;i<Menus[curMenu].nItems;i++) 
			FadeIn(&Menus[curMenu].items[i].stFade,FDELTA);
	Menus[curMenu].bFirst=false;
}

//Начать переход в следующее меню
void ui::GoToMenu(int menu,int curMenu)
{
	for(int i=0;i<Menus[curMenu].nItems-1;i++) 
		FadeOut(-1,&Menus[curMenu].items[i].stFade,FDELTA);
	FadeOut(menu,&Menus[curMenu].items[i].stFade,FDELTA);
}


void ui::ShowError(char* err)
{
	sErr=err;
	bErr=true;
}

void ui::FadeIn(FADE* pFade,DWORD dtl)
{
	pFade->bFadeOut=false;
	pFade->bFading=true;
	pFade->fColor=0.f;
	pFade->dFadeDelta=dtl;
}

void ui::FadeOut(int uiState,FADE* pFade,DWORD dtl)
{
	pFade->bFadeOut=true;
	pFade->bFading=true;
	pFade->dFadeDelta=dtl;
	pFade->nextState=uiState;
}

void ui::Fade(int idx,DWORD delta,FADE* pFade)
{
	if (pFade->bFading) {
		if (pFade->bFadeOut) {
			pFade->fColor-=(float)delta/pFade->dFadeDelta;
			if (pFade->fColor<=0) {
				if (pFade->nextState>=0) {
					uiState=pFade->nextState;
					Menus[uiState].bFirst=true;
				}
				if (idx>=0) {
					ReleaseImage(idx);
					return;
				}
				pFade->bFading=false;
				pFade->fColor=0.f;
			}
		}
		else if (pFade->fColor<1.f)
			pFade->fColor+=(float)delta/pFade->dFadeDelta;
		else {
			pFade->fColor=1.f;
			pFade->bFading=false;
		}
	}
}

//Начало движения
void ui::MoveBeg(int scMenu,int tgMenu)
{
	int i,j;

	if (scMenu<0 && tgMenu<0) return;

	//если в scMenu или в tgMenu нет пунктов то и цикл не пройдет
	
	//начало
	Menus[scMenu].bMoving=true;
	Menus[scMenu].tgMenu=tgMenu;
	for(i=0;i<Menus[scMenu].nItems;i++) {
		Menus[scMenu].items[i].stMove.dTranDelta=MDELTA+i*100;
		Menus[scMenu].items[i].stMove.fDelta=0;
		Menus[scMenu].items[i].stMove.bDone=false;
		Menus[scMenu].items[i].stMove.stPos=Menus[scMenu].items[i].pos;
		Menus[scMenu].items[i].stMove.endPos=Menus[scMenu].items[i].pos;
		Menus[scMenu].items[i].stMove.endPos.y+=vHight;
		FadeOut(-1,&Menus[scMenu].items[i].stFade,MDELTA+i*100);
	}
	
	//конец
	for(i=0;i<Menus[tgMenu].nItems;i++) {
		Menus[tgMenu].items[i].stMove.dTranDelta=MDELTA+i*100;
		Menus[tgMenu].items[i].stMove.fDelta=0;
		Menus[tgMenu].items[i].stMove.bDone=false;
		Menus[tgMenu].items[i].stMove.stPos=Menus[tgMenu].items[i].pos;
		Menus[tgMenu].items[i].stMove.stPos.y+=vHight;
		Menus[tgMenu].items[i].stMove.endPos=Menus[tgMenu].items[i].pos;
		FadeIn(&Menus[tgMenu].items[i].stFade,MDELTA+i*100);
	}
	
	//scMenu(i)-->tgMenu(j)
	for(i=0,j=0;i<Menus[scMenu].nItems && j<Menus[tgMenu].nItems;i++,j++) {
		Menus[scMenu].items[i].stMove.endPos=Menus[tgMenu].items[j].pos;
		Menus[tgMenu].items[j].stMove.stPos=Menus[scMenu].items[i].pos;
	}
	
	return;

/*	if (scMenu==-1) {
		//есть только конец

		return;
	}
	else {
		//есть только начало

		return;
	}
*/	
}

//Обновление движения
void ui::Move(DWORD delta,int scMenu)
{
	MOVE* pMove=NULL;

	if (Menus[scMenu].bMoving) {
		for(int i=0;i<Menus[scMenu].nItems;i++) {
			pMove=&Menus[scMenu].items[i].stMove;
			
			if (pMove->fDelta<pMove->dTranDelta)
				pMove->curPos=pMove->stPos+
		(pMove->endPos-pMove->stPos)*(float)sin((D3DX_PI/2)*(pMove->fDelta/pMove->dTranDelta));
			else pMove->curPos=pMove->endPos;
	
			Fade(-1,delta,&Menus[scMenu].items[i].stFade);

			if (fabs(pMove->endPos.x-pMove->curPos.x)<0.1f &&
				fabs(pMove->endPos.y-pMove->curPos.y)<0.1f)
				pMove->bDone=true;

			pMove->fDelta+=delta;
		}

		int tgMenu=Menus[scMenu].tgMenu;
		for(i=0;i<Menus[tgMenu].nItems;i++) {
			pMove=&Menus[tgMenu].items[i].stMove;
			
			if (pMove->fDelta<pMove->dTranDelta)
				pMove->curPos=pMove->stPos+
		(pMove->endPos-pMove->stPos)*(float)sin((D3DX_PI/2)*(pMove->fDelta/pMove->dTranDelta));
			else pMove->curPos=pMove->endPos;
			
			Fade(-1,delta,&Menus[tgMenu].items[i].stFade);

			if (fabs(pMove->endPos.x-pMove->curPos.x)<0.1f &&
				fabs(pMove->endPos.y-pMove->curPos.y)<0.1f)
				pMove->bDone=true;

			pMove->fDelta+=delta;
		}

		int scD=0,tgD=0;
		for(i=0;i<Menus[scMenu].nItems;i++) 
			if (Menus[scMenu].items[i].stMove.bDone==true) scD++;
		for(i=0;i<Menus[tgMenu].nItems;i++)
			if (Menus[tgMenu].items[i].stMove.bDone==true) tgD++;
		if (scD==Menus[scMenu].nItems && tgD==Menus[tgMenu].nItems) {
			Menus[scMenu].bMoving=false;
			uiState=Menus[scMenu].tgMenu;
		}
	}
}

// bground.cpp: implementation of the bground class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bground.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bground::bground()
{
	color=0xD0D0D0D0;
	bgSpeed=0;

	newSpeed=0;
	slideTime=0;
	bSlide=false;

	Statics=NULL;
	numStat=0;
	eTime=0;
}

bground::~bground()
{

}

//Инициализация звезд
bool bground::InitBG(IDirect3DDevice8 * pDevice,DWORD height,DWORD width)
{
	//Загрузка звезды
	if (!Star.Init(pDevice,"data\\star.spr")) return false;
	Star.cl.curFrame=0;
	Star.cl.Dir=0;

	//Сохранение размеров экрана
	Height=height;
	Width=width;

	for (int i=0;i<MAXSTARS;i++) {
		DWORD rnd=rand()%MAXSTARS;
		DWORD d1=MAXSTARS/3;
		DWORD d2=2*MAXSTARS/3;
		
		if (rnd>=0 && rnd<d1) Stars[i].spMul=0.75f;
		if (rnd>=d1 && rnd<d2) Stars[i].spMul=0.85f;
		if (rnd>=d2 && rnd<MAXSTARS) Stars[i].spMul=0.95f;

		Stars[i].pos.x=(float)(rand()%Width);
		Stars[i].pos.y=(float)(rand()%Height);

		Stars[i].frame=0;
	}


	return true;
}

//Обновить звезды в массиве в соответствии с прошедшим временем delta
void bground::UpdateBG(DWORD delta)
{
	//Расчет текущего изменения скорости
	if (bSlide) {
		float chSpeed=(newSpeed-bgSpeed)*delta/slideTime;
		bgSpeed+=chSpeed;
		if (chSpeed<0.0001) {
			bgSpeed=newSpeed;
			newSpeed=0.f;
			slideTime=0;
			bSlide=false;
		}
	}

	float vel=bgSpeed*(delta/10.0f);
	for(int i=0;i<MAXSTARS;i++) {
		//Расчет текущего кадра в зависимости от скорости
		Stars[i].frame=(int)(bgSpeed*Stars[i].spMul);
		if (Stars[i].frame>49) Stars[i].frame=49;

		Stars[i].pos.y+=vel*Stars[i].spMul;

		//Удаление звезд с Y>Height и добавление их сверху
		if (Stars[i].pos.y>(Height)) {
			Stars[i].pos.x=(float)(rand()%Width);
			Stars[i].pos.y=-(float)(rand()%Height);
		}
	}

	//Обновление статических обьектов
	for(i=0;i<numStat;i++)
		if (eTime>Statics[i].Time)
			Statics[i].pSpr->tran.pos.y+=vel;
	FreeStatics();
	eTime+=delta;
}

//Вывести все звезды и статические обьекты
void bground::RenderBG(IDirect3DDevice8 * pDevice)
{
	D3DXCOLOR	Dark=0x10101010;
	for(int i=0;i<MAXSTARS;i++) {
		Star.cl.curFrame=Stars[i].frame;
		D3DXColorLerp(&Star.tran.color,&Dark,&color,Stars[i].spMul);
		Star.tran.pos=Stars[i].pos;
		Star.Render(pDevice);
	}
	if (numStat>0) 
		for(i=0;i<numStat;i++) 
			Statics[i].pSpr->Render(pDevice);
}

//Немедленное изменение скорости
void bground::SetSpeed(float speed)
{
	bgSpeed=speed;
}

//Плавное изменение скорости в speed за время time
void bground::SetSpeedSlide(float speed,DWORD time)
{
	newSpeed=speed;
	slideTime=time;
	bSlide=true;
}

bool bground::AddStatic(IDirect3DDevice8 * pDevice,char* fn,float x,float y,DWORD time)
{
	STATIC* tmpStat;
	tmpStat=new STATIC[numStat+1];

	for(int i=0;i<numStat;i++) {
		tmpStat[i].pSpr=Statics[i].pSpr;
		tmpStat[i].Time=Statics[i].Time;
	}

	tmpStat[numStat].Time=time;
	tmpStat[numStat].pSpr=new sprite();
	if (!tmpStat[numStat].pSpr->Init(pDevice,fn)) return false;
	tmpStat[numStat].pSpr->tran.pos.x=x;
	tmpStat[numStat].pSpr->tran.pos.y=y;

	delete Statics;
	Statics=tmpStat;
	numStat++;

	return true;
}

void bground::FreeStatics()
{
	STATIC* tmpStat=NULL;

	for(int i=0;i<numStat;i++) {
		if (Statics[i].pSpr->tran.pos.y>(Height+Statics[i].pSpr->Height)) {
				Statics[i].pSpr->Release();
				delete Statics[i].pSpr;
				Statics[i].pSpr=NULL;
		}
	}

	if (numStat>1) {
		i=0;
		while (i<numStat) {
			if (Statics[i].pSpr==NULL) {
				tmpStat=new STATIC[numStat-1];
				for(int j=0;j<numStat;j++) {
					if (j==i) continue;
					if (j<i) {
						tmpStat[j].pSpr=Statics[j].pSpr;
						tmpStat[j].Time=Statics[j].Time;
					}
					if (j>i) {
						tmpStat[j-1].pSpr=Statics[j].pSpr;
						tmpStat[j-1].Time=Statics[j].Time;
					}
				}
				delete Statics;
				Statics=NULL;
				Statics=tmpStat;
				numStat--;
			}
			else i++;
		}
	}
	if (numStat==1) 
		if (Statics[0].pSpr==NULL) {
			delete Statics;
			Statics=NULL;
			numStat--;
		}

}

void bground::Release()
{
	Star.Release();
	for(int i=0;i<numStat;i++) {
		Statics[i].pSpr->Release();
		if (Statics[i].pSpr) delete Statics[i].pSpr;
	}
	if (Statics) delete Statics;
}
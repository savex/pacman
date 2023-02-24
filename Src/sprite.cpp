// sprite.cpp: implementation of the sprite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sprite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

sprite::sprite()
{
	frDelta=0;

	tran.pos.x=0;
	tran.pos.y=0;
	tran.rAngle=0;
	tran.rotate.x=0;
	tran.rotate.y=0;
	tran.color=0xFFFFFFFF;
	tran.clip=NULL;
	tran.scale=NULL;

	cl.curFrame=0;
	cl.Dir=1;

	pSpr=NULL;
	pFrames=NULL;
}	

sprite::~sprite()
{

}

//Загрузка кадров из SPR файла
bool sprite::Init(IDirect3DDevice8 * pDevice,char *fn)
{
	using namespace std;
	fstream in;
	in.open(fn,ios_base::binary|ios_base::in);
	if (!in.is_open()) return false;
	
	in.read((char *)&desc,sizeof(SPR_INFO));
	pSizes=new FRAME[desc.frames];
	pFrames=new LPDIRECT3DTEXTURE8[desc.frames];

	for (int i=0;i<desc.frames;i++) {
		in.read((char *)&pSizes[i].StartPos,sizeof(long));
		in.read((char *)&pSizes[i].Len,sizeof(long));
	}
	for (i=0;i<desc.frames;i++) {
		pData=new unsigned char[pSizes[i].Len];
		in.seekg(pSizes[i].StartPos,ios_base::beg);
		in.read((char *)pData,pSizes[i].Len);
		D3DXCreateTextureFromFileInMemory(pDevice,pData,pSizes[i].Len,&pFrames[i]);
		if (pData) delete pData;
	}
	in.close();
	delete pSizes;

	D3DSURFACE_DESC Desc;
	pFrames[0]->GetLevelDesc(0,&Desc);

	Width=Desc.Width;
	Height=Desc.Height;
	tran.rotate.x=Width/2.f;
	tran.rotate.y=Height/2.f;

	D3DXCreateSprite(pDevice,&pSpr);

	return true;
}

//Переключение кадров по времени
void sprite::Update(DWORD delta)
{
	if (frDelta>=(DWORD)(1000/desc.rate)) {
		frDelta-=(DWORD)(1000/desc.rate);
		if (cl.Dir>0) {
			if (cl.curFrame!=desc.frames) cl.curFrame++;
			else if (desc.loop) cl.curFrame=0;
		}
		if (cl.Dir<0) {
			if (cl.curFrame!=0) cl.curFrame--;
			else if (desc.loop) cl.curFrame=desc.frames-1;
		}
	}
	frDelta+=delta;
}

//Вывод текущего кадра согласно параметрам в TRANSITION
void sprite::Render(IDirect3DDevice8 * pDevice)
{
	pSpr->Begin();

	pDevice->SetRenderState(D3DRS_ZVISIBLE,FALSE); 

	// Turn on anisotropy
/*
    pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
*/
	pSpr->Draw(pFrames[cl.curFrame%desc.frames],
		tran.clip,
		tran.scale,
		&tran.rotate,
		tran.rAngle,
		&tran.pos,
		tran.color);

	pSpr->End();
}

//Освобождение памяти
void sprite::Release()
{
	if (pSpr) pSpr->Release();
	for (int i=0;i<desc.frames;i++) pFrames[i]->Release();
	if (pFrames) delete pFrames;
}
